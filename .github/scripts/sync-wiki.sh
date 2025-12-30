#!/bin/bash
set -e

SCRIPT_NAME="sync-wiki.sh"
VERSION="1.0.0"

DRY_RUN=false
FORCE=false
WIKI_REPO=""
VERSION_TAG=""
MAX_RETRIES=3
RETRY_DELAYS=(0 30 90)

show_help() {
    cat << EOF
GitHub Wiki Synchronization Script
Version: ${VERSION}

Usage: ${SCRIPT_NAME} [OPTIONS] <version_tag>

Options:
  --dry-run                 Preview changes without pushing
  --force                   Force push even if conflicts exist
  --wiki-repo <url>         Custom wiki repository URL
  --help                    Show help message

Environment Variables:
  GITHUB_TOKEN              GitHub token with wiki write permissions (required)
  GITHUB_REPOSITORY         Repository name (e.g., user/repo) (required)

Examples:
  # Sync docs to wiki for v1.0.0
  ./sync-wiki.sh v1.0.0

  # Dry run to preview changes
  ./sync-wiki.sh --dry-run v1.2.0

  # Force sync (use with caution)
  ./sync-wiki.sh --force v1.0.0

Exit Codes:
  0   Success or non-blocking failure
  1   Fatal error (missing token, wiki disabled)
  2   Prerequisite check failed
EOF
}

check_prerequisites() {
    if [ -z "$GITHUB_TOKEN" ]; then
        echo "::error::GITHUB_TOKEN not set"
        echo "Set token with: export GITHUB_TOKEN=<your_token>"
        exit 1
    fi
    
    if [ -z "$GITHUB_REPOSITORY" ]; then
        echo "::error::GITHUB_REPOSITORY not set"
        echo "Set with: export GITHUB_REPOSITORY=user/repo"
        exit 1
    fi
    
    if ! [[ "$VERSION_TAG" =~ ^v[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
        echo "::error::Invalid version format: $VERSION_TAG"
        echo "Expected: v1.2.3"
        exit 1
    fi
    
    if [ ! -d "docs" ]; then
        echo "::error::docs/ directory not found"
        exit 1
    fi
    
    if command -v gh &> /dev/null; then
        if ! gh api "repos/$GITHUB_REPOSITORY" --jq '.has_wiki' 2>/dev/null | grep -q true; then
            echo "::warning::Wiki not enabled for this repository"
            echo "Enable at: https://github.com/$GITHUB_REPOSITORY/settings"
            exit 0
        fi
    else
        echo "::warning::gh CLI not found, skipping wiki enabled check"
    fi
}

convert_to_wiki_name() {
    local path="$1"
    local name="${path%.md}"
    
    name="${name//\//-}"
    
    if [[ "$name" == *"-README" ]]; then
        name="${name%-README}"
    fi
    
    echo "${name}.md"
}

add_version_footer() {
    local file="$1"
    local version="$2"
    
    cat >> "$file" << EOF

---

**Version**: ${version}  
**Last Updated**: $(date +"%Y-%m-%d")  
**Auto-generated from**: [docs/](https://github.com/${GITHUB_REPOSITORY}/tree/${version}/docs)
EOF
}

clone_wiki() {
    local wiki_url="https://x-access-token:${GITHUB_TOKEN}@github.com/${GITHUB_REPOSITORY}.wiki.git"
    local wiki_dir="wiki-temp-$(date +%Y%m%d-%H%M%S)"
    
    if [ -n "$WIKI_REPO" ]; then
        wiki_url="$WIKI_REPO"
    fi
    
    echo "Cloning wiki repository..."
    if ! git clone "$wiki_url" "$wiki_dir" 2>&1; then
        echo "::error::Failed to clone wiki repository"
        echo "Check: Wiki is enabled, GITHUB_TOKEN has wiki write permissions"
        return 1
    fi
    
    echo "$wiki_dir"
}

process_docs() {
    local wiki_dir="$1"
    local version="$2"
    local count=0
    
    echo "Processing documentation files..."
    
    while IFS= read -r -d '' file; do
        local relative_path="${file#docs/}"
        local wiki_name=$(convert_to_wiki_name "$relative_path")
        local wiki_path="${wiki_dir}/${wiki_name}"
        
        echo "  Processing: $file → $wiki_name"
        
        cp "$file" "$wiki_path"
        add_version_footer "$wiki_path" "$version"
        
        count=$((count + 1))
    done < <(find docs -name "*.md" -type f -print0)
    
    echo "Processed $count documentation files"
}

commit_and_push() {
    local wiki_dir="$1"
    local version="$2"
    
    cd "$wiki_dir"
    
    git config user.name "GitHub Actions"
    git config user.email "actions@github.com"
    
    git add .
    
    if ! git diff --staged --quiet; then
        echo "Committing changes..."
        git commit -m "docs: sync documentation for ${version}

- Auto-generated from docs/ directory
- Version: ${version}
- Updated: $(date +"%Y-%m-%d %H:%M:%S")"
        
        if [ "$DRY_RUN" = true ]; then
            echo "DRY RUN: Would push changes to wiki"
            git log -1 --stat
            return 0
        fi
        
        echo "Pushing to wiki..."
        if [ "$FORCE" = true ]; then
            git push --force
        else
            git push
        fi
        
        echo "✓ Wiki successfully updated"
    else
        echo "No changes to commit"
    fi
    
    cd - > /dev/null
}

cleanup() {
    local wiki_dir="$1"
    if [ -d "$wiki_dir" ]; then
        echo "Cleaning up temporary directory..."
        rm -rf "$wiki_dir"
    fi
}

create_github_issue() {
    local title="$1"
    local body="Wiki synchronization failed for version ${VERSION_TAG}

**Error Details:**
\`\`\`
$(cat /tmp/wiki-sync-error.log 2>/dev/null || echo "No error log available")
\`\`\`

**Next Steps:**
1. Verify GITHUB_TOKEN has wiki write permissions
2. Ensure wiki is enabled in repository settings
3. Check that docs/ directory contains valid markdown files
4. Try manual sync: \`.github/scripts/sync-wiki.sh ${VERSION_TAG}\`

**Auto-generated by wiki-sync workflow**"
    
    if command -v gh &> /dev/null; then
        echo "Creating GitHub issue..."
        gh issue create \
            --title "$title" \
            --body "$body" \
            --label "documentation,automation" || true
    else
        echo "::warning::gh CLI not available, cannot create issue"
    fi
}

sync_with_retry() {
    local attempt=1
    local wiki_dir=""
    
    while [ $attempt -le $MAX_RETRIES ]; do
        echo "Attempt $attempt of $MAX_RETRIES..."
        
        if [ $attempt -gt 1 ]; then
            local delay=${RETRY_DELAYS[$((attempt - 1))]}
            echo "Waiting ${delay}s before retry..."
            sleep $delay
        fi
        
        wiki_dir=$(clone_wiki) && {
            process_docs "$wiki_dir" "$VERSION_TAG"
            commit_and_push "$wiki_dir" "$VERSION_TAG"
            cleanup "$wiki_dir"
            return 0
        } || {
            echo "::warning::Attempt $attempt failed" | tee -a /tmp/wiki-sync-error.log
            cleanup "$wiki_dir"
        }
        
        attempt=$((attempt + 1))
    done
    
    echo "::error::All retry attempts exhausted"
    create_github_issue "Wiki sync failed for ${VERSION_TAG}"
    return 0
}

main() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --dry-run)
                DRY_RUN=true
                shift
                ;;
            --force)
                FORCE=true
                shift
                ;;
            --wiki-repo)
                WIKI_REPO="$2"
                shift 2
                ;;
            --help)
                show_help
                exit 0
                ;;
            *)
                if [ -z "$VERSION_TAG" ]; then
                    VERSION_TAG="$1"
                else
                    echo "ERROR: Unknown argument: $1"
                    show_help
                    exit 2
                fi
                shift
                ;;
        esac
    done
    
    if [ -z "$VERSION_TAG" ]; then
        echo "ERROR: Version tag required"
        show_help
        exit 2
    fi
    
    echo "GitHub Wiki Synchronization"
    echo "============================"
    echo "Version: $VERSION_TAG"
    echo "Repository: $GITHUB_REPOSITORY"
    echo "Dry Run: $DRY_RUN"
    echo ""
    
    check_prerequisites
    sync_with_retry
}

main "$@"
