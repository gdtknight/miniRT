# Contract: Wiki Synchronization

**Version**: 1.0.0  
**Date**: 2025-12-18  
**Status**: Specification

## Overview

This contract defines the behavior, interface, and guarantees for automated GitHub Wiki synchronization from the docs/ directory to the repository's GitHub Wiki.

---

## Script Interface

### Script Location
`.github/scripts/sync-wiki.sh`

### Usage
```bash
sync-wiki.sh [OPTIONS] <version_tag>

Options:
  --dry-run                 Preview changes without pushing
  --force                   Force push even if conflicts exist
  --wiki-repo <url>         Custom wiki repository URL
  --help                    Show help message

Examples:
  # Sync docs to wiki for v1.0.0
  ./sync-wiki.sh v1.0.0

  # Dry run to preview changes
  ./sync-wiki.sh --dry-run v1.2.0

  # Force sync (use with caution)
  ./sync-wiki.sh --force v1.0.0
```

### Exit Codes
- `0`: Success or non-blocking failure (see below)
- `1`: Fatal error (GITHUB_TOKEN missing, wiki disabled)
- `2`: Prerequisite check failed

---

## Prerequisites Check

### Function: check_prerequisites()

**Checks performed:**

1. **GITHUB_TOKEN exists**
```bash
if [ -z "$GITHUB_TOKEN" ]; then
    echo "::error::GITHUB_TOKEN not set"
    exit 1
fi
```

2. **Version tag exists and matches semver**
```bash
if ! [[ "$VERSION" =~ ^v[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "::error::Invalid version format: $VERSION"
    echo "Expected: v1.2.3"
    exit 1
fi
```

3. **Wiki enabled in repository**
```bash
gh api "repos/$GITHUB_REPOSITORY" --jq '.has_wiki' | grep -q true || {
    echo "::warning::Wiki not enabled for this repository"
    echo "Enable at: https://github.com/$GITHUB_REPOSITORY/settings"
    exit 0  # Non-blocking
}
```

4. **docs/ directory exists**
```bash
if [ ! -d "docs" ]; then
    echo "::error::docs/ directory not found"
    exit 1
fi
```

---

## Synchronization Process

### Step 1: Clone Wiki Repository

```bash
WIKI_URL="https://x-access-token:${GITHUB_TOKEN}@github.com/${GITHUB_REPOSITORY}.wiki.git"
WIKI_DIR="wiki-temp-${TIMESTAMP}"

git clone "$WIKI_URL" "$WIKI_DIR" || {
    echo "::error::Failed to clone wiki repository"
    echo "Check: Wiki is enabled, GITHUB_TOKEN has wiki write permissions"
    create_github_issue "Wiki clone failed for $VERSION"
    exit 0  # Non-blocking
}
```

### Step 2: Process Documentation Files

```bash
process_doc_file() {
    local source_file="$1"
    local relative_path="${source_file#docs/}"
    
    # Convert directory structure to wiki page names
    # docs/01-시작하기/README.md → 01-시작하기.md
    # docs/02-개발/테스트.md → 02-개발-테스트.md
    
    local wiki_name=$(convert_to_wiki_name "$relative_path")
    local wiki_path="${WIKI_DIR}/${wiki_name}"
    
    # Copy content
    cp "$source_file" "$wiki_path"
    
    # Add version footer
    add_version_footer "$wiki_path" "$VERSION"
}

# Process all markdown files
find docs -name "*.md" -type f | while read -r file; do
    process_doc_file "$file"
done
```

### Step 3: Convert Directory Names to Wiki Page Names

**Conversion Rules:**
```bash
convert_to_wiki_name() {
    local path="$1"
    
    # Remove .md extension
    local name="${path%.md}"
    
    # Convert slashes to hyphens
    # 01-시작하기/설치.md → 01-시작하기-설치
    name="${name//\//-}"
    
    # Handle README.md → use directory name
    # 01-시작하기/README.md → 01-시작하기
    if [[ "$name" == *"-README" ]]; then
        name="${name%-README}"
    fi
    
    # Add .md extension back
    echo "${name}.md"
}
```

**Example Conversions:**
| Source Path | Wiki Page Name |
|-------------|----------------|
| docs/README.md | Home.md |
| docs/00-전체설명/README.md | 00-전체설명.md |
| docs/01-시작하기/설치.md | 01-시작하기-설치.md |
| docs/02-개발/테스트.md | 02-개발-테스트.md |

### Step 4: Add Version Footer

```bash
add_version_footer() {
    local file="$1"
    local version="$2"
    
    cat >> "$file" << EOF

---

**Version**: $version  
**Last Updated**: $(date +%Y-%m-%d)  
**Auto-generated** from [docs/](https://github.com/${GITHUB_REPOSITORY}/tree/main/docs)
EOF
}
```

### Step 5: Commit and Push

```bash
cd "$WIKI_DIR"

git config user.name "github-actions[bot]"
git config user.email "github-actions[bot]@users.noreply.github.com"

git add .
git commit -m "Update wiki for version $VERSION" || {
    echo "::notice::No changes to commit"
    cd ..
    rm -rf "$WIKI_DIR"
    exit 0
}

# Push with retry logic
push_with_retry || {
    echo "::error::Failed to push to wiki after retries"
    create_github_issue "Wiki push failed for $VERSION"
    cd ..
    rm -rf "$WIKI_DIR"
    exit 0  # Non-blocking
}

cd ..
rm -rf "$WIKI_DIR"
```

---

## Retry Logic

### Function: push_with_retry()

**Strategy**: Exponential backoff with 3 attempts

```bash
push_with_retry() {
    local max_attempts=3
    local attempt=1
    local delay=0
    
    while [ $attempt -le $max_attempts ]; do
        echo "Push attempt $attempt/$max_attempts..."
        
        if [ $attempt -gt 1 ]; then
            echo "Waiting ${delay}s before retry..."
            sleep $delay
            
            # Pull latest changes in case of conflicts
            git pull --rebase origin master || {
                echo "::warning::Conflict detected, attempting resolution"
                git rebase --abort
                git pull --strategy-option=theirs origin master
            }
        fi
        
        if git push origin master; then
            echo "✓ Successfully pushed to wiki"
            return 0
        fi
        
        echo "::warning::Push failed (attempt $attempt)"
        
        # Exponential backoff: 0s, 30s, 90s
        delay=$((delay + 30 * attempt))
        attempt=$((attempt + 1))
    done
    
    return 1
}
```

**Backoff Schedule:**
- Attempt 1: Immediate (0s delay)
- Attempt 2: 30s delay
- Attempt 3: 90s delay (30 + 60)

---

## Failure Handling

### Non-Blocking Failures

The following failures do NOT block the release workflow:
- Wiki clone failure (wiki might be disabled)
- Push conflicts that can't be resolved
- Network timeouts
- Any error after prerequisites pass

**Behavior**:
```bash
on_non_blocking_failure() {
    local error_message="$1"
    
    echo "::warning::$error_message"
    log_to_artifact "$error_message"
    create_github_issue "$error_message"
    exit 0  # Success exit code (don't block release)
}
```

### Fatal Failures (Blocking)

The following failures WILL block the sync script:
- GITHUB_TOKEN not set
- Invalid version format
- docs/ directory missing

**Behavior**:
```bash
on_fatal_failure() {
    local error_message="$1"
    
    echo "::error::$error_message"
    exit 1  # Failure exit code
}
```

---

## GitHub Issue Creation

### Function: create_github_issue()

**Triggered on**: Non-blocking failures after all retries exhausted

```bash
create_github_issue() {
    local error_summary="$1"
    
    local issue_title="🚨 Wiki Sync Failure: $VERSION"
    local issue_body
    read -r -d '' issue_body << EOF
## Wiki Synchronization Failed

**Version**: $VERSION  
**Date**: $(date --iso-8601=seconds)  
**Error**: $error_summary

### Details

The automated wiki synchronization failed during the release process. The release has completed successfully, but the wiki was not updated.

### Manual Sync Instructions

1. Clone the wiki repository:
   \`\`\`bash
   git clone https://github.com/${GITHUB_REPOSITORY}.wiki.git
   \`\`\`

2. Checkout the release tag in the main repository:
   \`\`\`bash
   git checkout $VERSION
   \`\`\`

3. Copy documentation to wiki:
   \`\`\`bash
   cp -r docs/* ../$(basename ${GITHUB_REPOSITORY}).wiki/
   \`\`\`

4. Update version footers:
   \`\`\`bash
   find ../$(basename ${GITHUB_REPOSITORY}).wiki -name "*.md" -exec \
     sed -i '' 's/Version: .*/Version: $VERSION/' {} \;
   \`\`\`

5. Commit and push:
   \`\`\`bash
   cd ../$(basename ${GITHUB_REPOSITORY}).wiki
   git add .
   git commit -m "Manual sync for $VERSION"
   git push
   \`\`\`

### Logs

See [workflow run artifacts]($GITHUB_SERVER_URL/$GITHUB_REPOSITORY/actions/runs/$GITHUB_RUN_ID) for detailed logs.

---
*This issue was automatically created by the wiki sync workflow.*
EOF

    gh issue create \
        --title "$issue_title" \
        --body "$issue_body" \
        --label "documentation,bug,automated" \
        --repo "$GITHUB_REPOSITORY" || {
        echo "::warning::Failed to create GitHub issue"
    }
}
```

---

## Dry Run Mode

### Purpose
Preview wiki changes without actually pushing.

### Implementation
```bash
if [ "$DRY_RUN" = true ]; then
    echo "=== DRY RUN MODE ==="
    echo "Would sync the following files:"
    
    find docs -name "*.md" -type f | while read -r file; do
        local wiki_name=$(convert_to_wiki_name "${file#docs/}")
        echo "  $file → wiki/$wiki_name"
    done
    
    echo ""
    echo "Would commit message:"
    echo "  Update wiki for version $VERSION"
    echo ""
    echo "=== END DRY RUN ==="
    exit 0
fi
```

### Usage in CI
```yaml
- name: Preview Wiki Changes
  run: .github/scripts/sync-wiki.sh --dry-run ${{ github.ref_name }}
```

---

## Integration with Workflows

### Called from release.yml

```yaml
- name: Sync Wiki
  id: wiki-sync
  continue-on-error: true  # Don't fail release on wiki errors
  run: |
    .github/scripts/sync-wiki.sh ${{ github.ref_name }}
  env:
    GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}

- name: Check Wiki Sync Status
  if: steps.wiki-sync.outcome == 'failure'
  run: |
    echo "::warning::Wiki sync encountered issues"
    echo "::notice::Check created issue for manual sync instructions"
```

### Standalone wiki-sync.yml

```yaml
name: Wiki Sync
on:
  push:
    tags:
      - 'v*.*.*'
  workflow_dispatch:
    inputs:
      version:
        description: 'Version tag to sync'
        required: true

jobs:
  sync:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          ref: ${{ github.event.inputs.version || github.ref }}
      
      - name: Sync Wiki
        run: .github/scripts/sync-wiki.sh ${{ github.event.inputs.version || github.ref_name }}
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

---

## File Organization in Wiki

### Before Sync (docs/ structure)
```
docs/
├── README.md
├── 00-전체설명/
│   └── README.md
├── 01-시작하기/
│   ├── README.md
│   └── 설치.md
├── 02-개발/
│   ├── README.md
│   └── 테스트.md
└── wiki/
    ├── WIKI_READY.md
    └── WIKI_SETUP.md
```

### After Sync (wiki structure)
```
wiki/
├── Home.md                    # from docs/README.md
├── 00-전체설명.md             # from docs/00-전체설명/README.md
├── 01-시작하기.md             # from docs/01-시작하기/README.md
├── 01-시작하기-설치.md        # from docs/01-시작하기/설치.md
├── 02-개발.md                 # from docs/02-개발/README.md
├── 02-개발-테스트.md          # from docs/02-개발/테스트.md
├── wiki-WIKI_READY.md         # from docs/wiki/WIKI_READY.md
└── wiki-WIKI_SETUP.md         # from docs/wiki/WIKI_SETUP.md
```

---

## Content Transformations

### Korean Text Handling
- UTF-8 encoding preserved
- No character transformations
- Korean directory names kept as-is

### Link Rewriting
Transform relative links to work in wiki:
```bash
rewrite_links() {
    local file="$1"
    
    # Convert relative doc links to wiki links
    # [text](../other-doc.md) → [text](other-doc)
    sed -i 's|\](\.\.\/\([^)]*\)\.md)|\](\1)|g' "$file"
    
    # Convert same-directory links
    # [text](./doc.md) → [text](doc)
    sed -i 's|\](\.\(/[^)]*\)\.md)|\](\1)|g' "$file"
}
```

### Image Handling
Images referenced in docs:
```markdown
![diagram](../images/architecture.png)
```

Options:
1. **Upload to wiki repository** (RECOMMENDED)
   - Copy images to wiki repo
   - Update image paths

2. **Reference main repository**
   - Use full GitHub URLs
   - Images stay in main repo

Implementation:
```bash
handle_images() {
    local file="$1"
    
    # Find image references
    grep -o '!\[.*\]([^)]*\.png)' "$file" | while read -r img; do
        local img_path=$(echo "$img" | sed 's/.*(\(.*\)).*/\1/')
        
        # Copy image to wiki
        cp "docs/$img_path" "${WIKI_DIR}/images/"
        
        # Update reference
        sed -i "s|$img_path|images/$(basename $img_path)|g" "$file"
    done
}
```

---

## Testing Strategy

### Local Testing
```bash
# Set up test environment
export GITHUB_TOKEN="test_token"
export GITHUB_REPOSITORY="user/repo"

# Test dry run
./sync-wiki.sh --dry-run v1.0.0

# Test with local wiki repo
git init test-wiki.wiki
export WIKI_REPO_URL="file://$(pwd)/test-wiki.wiki"
./sync-wiki.sh --wiki-repo "$WIKI_REPO_URL" v1.0.0
```

### Test Cases

**1. Normal Sync (Expected: Success)**
```bash
./sync-wiki.sh v1.0.0
# Expected: exit 0, wiki updated, no issues created
```

**2. Wiki Disabled (Expected: Non-blocking Failure)**
```bash
# Manually disable wiki in settings
./sync-wiki.sh v1.0.0
# Expected: exit 0, warning logged, no crash
```

**3. Push Conflict (Expected: Retry and Resolve)**
```bash
# Simulate conflict: manually edit wiki between retries
./sync-wiki.sh v1.0.0
# Expected: retry logic activates, eventually succeeds or creates issue
```

**4. Invalid Version (Expected: Fatal Error)**
```bash
./sync-wiki.sh invalid-version
# Expected: exit 1, error message about version format
```

---

## Performance Characteristics

### Typical Execution Time
- Small docs (<10 files): 10-20 seconds
- Medium docs (10-50 files): 30-60 seconds
- Large docs (>50 files): 60-120 seconds

### Timeout Configuration
- Overall script timeout: 10 minutes
- Git clone timeout: 2 minutes
- Git push timeout: 2 minutes per attempt

---

## Maintenance

### Updating Wiki Structure
When changing docs/ organization:
1. Update `convert_to_wiki_name()` function
2. Test with `--dry-run`
3. Document changes in this contract
4. Consider redirects for old wiki page names

### Adding New Content Types
When adding non-markdown content:
1. Add handler function (similar to `handle_images()`)
2. Test thoroughly with dry run
3. Update this contract with new behavior

### Troubleshooting Common Issues

**Issue**: Wiki pages show old content  
**Solution**: Check version footer, verify push succeeded

**Issue**: Images broken in wiki  
**Solution**: Verify image paths, check image handling logic

**Issue**: Korean characters corrupted  
**Solution**: Verify UTF-8 encoding, check git config

---

## Security Considerations

### Token Permissions
GITHUB_TOKEN must have:
- `contents: read` (checkout code)
- `pages: write` (update wiki)

### URL Sanitization
```bash
# Never log URLs with tokens
SAFE_URL="${WIKI_URL/@*//@***}"
echo "Cloning from: $SAFE_URL"
```

### Temporary Directory Cleanup
```bash
# Always clean up, even on errors
trap 'rm -rf "$WIKI_DIR"' EXIT INT TERM
```

---

## Version History

- **1.0.0** (2025-12-18): Initial contract specification
