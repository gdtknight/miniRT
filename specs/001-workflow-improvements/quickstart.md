# Quickstart: Workflow Improvements

**Feature**: Workflow Improvements and Project Structure Reorganization  
**Audience**: Developers implementing or using the improved workflows  
**Time to Complete**: 30-45 minutes setup + testing

---

## Prerequisites

### Required Tools
- Git 2.20 or newer
- GNU Make
- C compiler (gcc or clang)
- GitHub account with repository access

### Platform-Specific Requirements

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install -y git make gcc valgrind
```

**macOS:**
```bash
# Install Xcode Command Line Tools (includes git, make, clang, leaks)
xcode-select --install

# Verify leaks command available
which leaks
```

### GitHub CLI (Optional but Recommended)
```bash
# Installation
# Linux
sudo apt-get install gh

# macOS
brew install gh

# Authenticate
gh auth login
```

---

## Quick Start (5 minutes)

### 1. Clone and Setup

```bash
# Clone repository
git clone https://github.com/YOUR_USERNAME/miniRT.git
cd miniRT

# Switch to workflow improvements branch
git checkout 001-workflow-improvements

# Build the project
make
```

### 2. Run Local Quality Checks

```bash
# Check code style
norminette src includes

# Build
make re

# Run tests
./test_miniRT.sh

# Check for memory leaks (one scene)
bash .github/scripts/check-memory-leaks.sh scenes/test/sphere.rt
```

### 3. Verify Success
If all commands succeed with exit code 0, your environment is ready!

---

## Detailed Walkthrough

### Step 1: Understanding the New Structure

**Before reorganization:**
```
miniRT/
├── create_wiki.sh               # User tool
├── test_miniRT.sh              # User tool
├── IMPLEMENTATION_PLAN.txt     # Planning doc
├── README_OLD.md               # Backup file
├── WIKI_READY.md               # Wiki doc
└── docs/                       # Korean docs
```

**After reorganization:**
```
miniRT/
├── create_wiki.sh              # User tool (unchanged)
├── test_miniRT.sh              # User tool (enhanced)
├── .github/
│   ├── workflows/              # CI/CD workflows
│   └── scripts/                # CI automation scripts
├── docs/
│   ├── wiki/                   # Wiki docs (moved)
│   ├── archive/                # Old/backup files (moved)
│   └── project/                # Planning docs (moved)
└── src/                        # Source code (unchanged)
```

**Key principles:**
- User-facing tools stay at root (easy to find)
- CI scripts in `.github/scripts/` (organization)
- Documentation organized by purpose (clarity)
- All moves preserve git history

---

### Step 2: File Reorganization Process

**If implementing the reorganization:**

```bash
# Create new directory structure
mkdir -p .github/scripts
mkdir -p docs/wiki
mkdir -p docs/archive
mkdir -p docs/project

# Move files using git mv (preserves history)
git mv IMPLEMENTATION_PLAN.txt docs/project/
git mv README_OLD.md docs/archive/
git mv WIKI_READY.md docs/wiki/
git mv WIKI_SETUP.md docs/wiki/

# Verify history is preserved
git log --follow docs/project/IMPLEMENTATION_PLAN.txt

# Commit changes
git commit -m "docs: reorganize project structure per constitution

- Move planning docs to docs/project/
- Move obsolete files to docs/archive/
- Move wiki docs to docs/wiki/
- Preserve git history with git mv"
```

**Verification:**
```bash
# Each file should show full history
git log --follow -- docs/project/IMPLEMENTATION_PLAN.txt
git log --follow -- docs/archive/README_OLD.md
git log --follow -- docs/wiki/WIKI_READY.md
```

---

### Step 3: Setting Up CI Scripts

**Create memory leak check script:**

```bash
# Create the script
cat > .github/scripts/check-memory-leaks.sh << 'EOF'
#!/bin/bash
set -e

# Platform detection and tool selection
# (See contracts/memory-check.md for full implementation)

SCENE_FILE="$1"

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Install valgrind if needed
    if ! command -v valgrind &> /dev/null; then
        sudo apt-get update -qq
        sudo apt-get install -y valgrind
    fi
    
    # Run valgrind
    valgrind --leak-check=full --error-exitcode=1 ./miniRT "$SCENE_FILE"
    
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # Use leaks on macOS
    ./miniRT "$SCENE_FILE" &
    PID=$!
    sleep 2
    leaks $PID
    kill $PID 2>/dev/null || true
fi
EOF

# Make executable
chmod +x .github/scripts/check-memory-leaks.sh

# Test locally
.github/scripts/check-memory-leaks.sh scenes/test/sphere.rt
```

**Create wiki sync script:**

```bash
# Create the script
cat > .github/scripts/sync-wiki.sh << 'EOF'
#!/bin/bash
set -e

# Wiki synchronization logic
# (See contracts/wiki-sync.md for full implementation)

VERSION="$1"

# Check prerequisites
if [ -z "$GITHUB_TOKEN" ]; then
    echo "ERROR: GITHUB_TOKEN not set"
    exit 1
fi

# Clone wiki, sync docs, push
# ...
EOF

chmod +x .github/scripts/sync-wiki.sh
```

---

### Step 4: Configuring GitHub Actions Workflows

**Update ci.yml for development checks:**

```yaml
# .github/workflows/ci.yml
name: Development CI

on:
  push:
    branches: [develop, 'feature/**']

concurrency:
  group: ci-${{ github.ref }}
  cancel-in-progress: true

jobs:
  quality-checks:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest]
    
    steps:
      - uses: actions/checkout@v4
      
      - name: Install norminette
        run: pip install norminette
      
      - name: Check code style
        run: norminette src includes
      
      - name: Build
        run: make
      
      - name: Run tests
        run: ./test_miniRT.sh
```

**Update pr-validation.yml with memory checks:**

```yaml
# .github/workflows/pr-validation.yml
name: PR Validation

on:
  pull_request:
    types: [opened, synchronize]

jobs:
  # ... norminette, build, test jobs ...
  
  memory-leak-check:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest]
    needs: build
    
    steps:
      - uses: actions/checkout@v4
      - name: Build miniRT
        run: make
      
      - name: Check memory leaks
        run: |
          for scene in scenes/test/*.rt; do
            echo "Testing: $scene"
            .github/scripts/check-memory-leaks.sh "$scene"
          done
      
      - name: Upload logs on failure
        if: failure()
        uses: actions/upload-artifact@v3
        with:
          name: memory-leak-logs-${{ matrix.os }}
          path: logs/
```

**Create wiki-sync workflow:**

```yaml
# .github/workflows/wiki-sync.yml
name: Wiki Sync

on:
  push:
    tags: ['v*.*.*']
  workflow_dispatch:
    inputs:
      version:
        description: 'Version tag'
        required: true

jobs:
  sync:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          ref: ${{ github.event.inputs.version || github.ref }}
      
      - name: Sync Wiki
        continue-on-error: true
        run: .github/scripts/sync-wiki.sh ${{ github.event.inputs.version || github.ref_name }}
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

---

### Step 5: Enhancing test_miniRT.sh

**Add structured output and error cases:**

```bash
#!/bin/bash

# Test counters
TOTAL=0
PASSED=0
FAILED=0

# Test execution function
run_test() {
    local name="$1"
    local scene="$2"
    local expected="$3"  # "pass" or "fail"
    
    TOTAL=$((TOTAL + 1))
    
    if timeout 5 ./miniRT "$scene" > /dev/null 2>&1; then
        actual="pass"
    else
        actual="fail"
    fi
    
    if [ "$actual" == "$expected" ]; then
        echo "✓ PASS: $name"
        PASSED=$((PASSED + 1))
    else
        echo "✗ FAIL: $name (expected $expected, got $actual)"
        FAILED=$((FAILED + 1))
    fi
}

echo "miniRT Test Suite"
echo "================="

# Valid scenes (should pass)
run_test "Simple sphere" "scenes/test/sphere.rt" "pass"
run_test "Multiple objects" "scenes/test/multi.rt" "pass"

# Error cases (should fail gracefully)
run_test "Missing ambient" "scenes/error/no_ambient.rt" "fail"
run_test "Invalid camera" "scenes/error/bad_camera.rt" "fail"

# Summary
echo ""
echo "Results: $PASSED/$TOTAL passed"
[ $FAILED -eq 0 ] && exit 0 || exit 1
```

---

### Step 6: Testing Workflows Locally

**Test memory leak detection:**
```bash
# Good scene (should pass)
.github/scripts/check-memory-leaks.sh scenes/test/sphere.rt
echo "Exit code: $?"  # Should be 0

# Create test scene with intentional leak (for testing)
# After fixing all leaks, this step is for verification only
```

**Test wiki sync (dry run):**
```bash
export GITHUB_TOKEN="dummy_token_for_testing"
export GITHUB_REPOSITORY="your-username/miniRT"

.github/scripts/sync-wiki.sh --dry-run v1.0.0
# Should show what would be synced without actually pushing
```

**Test complete workflow locally:**
```bash
# Run all quality checks
norminette src includes && \
make re && \
./test_miniRT.sh && \
.github/scripts/check-memory-leaks.sh scenes/test/sphere.rt

echo "All checks passed! ✓"
```

---

### Step 7: First Pull Request with New Workflows

**Create feature branch:**
```bash
git checkout -b test-workflows
```

**Make a small change:**
```bash
# Add a comment or update documentation
echo "// Test workflows" >> src/main.c

git add src/main.c
git commit -m "test: verify workflow improvements"
git push origin test-workflows
```

**Create PR and watch workflows:**
1. Go to GitHub repository
2. Create Pull Request from `test-workflows` to `main`
3. Watch CI checks run:
   - Norminette check
   - Build on Linux and macOS
   - Tests
   - Memory leak detection
4. Verify all checks pass
5. Check artifacts if memory leak check fails

---

### Step 8: Creating a Release with Wiki Sync

**Tag a release:**
```bash
# Make sure you're on main branch with all changes
git checkout main
git pull origin main

# Create annotated tag
git tag -a v1.0.0 -m "Release v1.0.0: Workflow improvements"

# Push tag
git push origin v1.0.0
```

**Monitor release workflow:**
1. Go to Actions tab on GitHub
2. Find "Release" workflow run
3. Verify stages:
   - Quality checks (norminette, build, test, memory)
   - Build artifacts
   - Wiki sync (may fail gracefully)
   - Create release
4. Check releases page for published artifacts

**If wiki sync fails:**
- Workflow continues (non-blocking)
- GitHub issue automatically created
- Follow manual sync instructions in issue

---

## Common Tasks

### Running Memory Leak Check on All Scenes

```bash
#!/bin/bash
# check-all-scenes.sh

for scene in scenes/**/*.rt; do
    echo "Checking: $scene"
    if .github/scripts/check-memory-leaks.sh "$scene"; then
        echo "✓ No leaks"
    else
        echo "✗ Leaks detected!"
        exit 1
    fi
done

echo "All scenes passed!"
```

### Manually Syncing Wiki

```bash
# If automatic sync fails, run manually
gh auth login  # Authenticate if needed

.github/scripts/sync-wiki.sh v1.0.0
```

### Checking Workflow Status

```bash
# Using GitHub CLI
gh run list --workflow=ci.yml
gh run list --workflow=pr-validation.yml
gh run view <run-id>
```

### Downloading Artifacts from Failed Runs

```bash
# List artifacts
gh run view <run-id> --log-failed

# Download specific artifact
gh run download <run-id> -n memory-leak-logs
```

---

## Troubleshooting

### Issue: Norminette not found
**Solution:**
```bash
pip install norminette
# Or
pip3 install norminette
```

### Issue: Valgrind not installed (Linux)
**Solution:**
```bash
sudo apt-get update
sudo apt-get install -y valgrind
```

### Issue: Memory leak check times out
**Possible causes:**
- Infinite loop in code
- Very complex scene
- Performance issue

**Solution:**
```bash
# Increase timeout (default 600s)
timeout 900 .github/scripts/check-memory-leaks.sh scene.rt

# Profile the scene
time ./miniRT scene.rt
```

### Issue: Wiki sync fails with authentication error
**Solution:**
```bash
# Verify token has correct permissions
gh auth status

# Regenerate token with wiki write permissions
gh auth login --scopes repo,wiki
```

### Issue: Workflow cancelled unexpectedly
**Cause:** Concurrency control cancelled older run

**Solution:** This is expected behavior. The latest push always takes precedence.

### Issue: Git history not preserved after reorganization
**Cause:** Used `mv` instead of `git mv`

**Solution:**
```bash
# Undo the move
git reset --hard HEAD~1

# Redo with git mv
git mv old/path new/path
git commit -m "Move file preserving history"

# Verify
git log --follow new/path
```

---

## Next Steps

After completing this quickstart:

1. **Review Contracts**: Read detailed specifications in `contracts/`
   - `workflows.md`: GitHub Actions workflow details
   - `memory-check.md`: Memory leak detection contract
   - `wiki-sync.md`: Wiki synchronization contract

2. **Review Data Model**: Understand entities in `data-model.md`

3. **Implement Tasks**: Proceed to Phase 2 with `/speckit.tasks` command

4. **Test Thoroughly**: Create test PRs to verify all workflows

5. **Document Issues**: Create GitHub issues for any problems encountered

---

## Reference Commands

### Essential Commands
```bash
# Quality checks (local)
norminette src includes
make re
./test_miniRT.sh
.github/scripts/check-memory-leaks.sh scenes/test/sphere.rt

# Git operations
git mv old/path new/path
git log --follow new/path
git tag -a v1.0.0 -m "Release message"
git push origin v1.0.0

# GitHub CLI
gh pr create
gh pr checks
gh run list
gh run view <run-id>
gh run download <run-id>
```

### Workflow Triggers
```bash
# Trigger development CI
git push origin feature-branch

# Trigger PR validation
gh pr create --base main --head feature-branch

# Trigger release workflow
git tag v1.0.0 && git push origin v1.0.0

# Manually trigger wiki sync
gh workflow run wiki-sync.yml -f version=v1.0.0
```

---

## Support

- **Documentation**: See `contracts/` directory for detailed specifications
- **GitHub Issues**: Report problems or ask questions
- **Workflow Logs**: Check Actions tab for detailed execution logs
- **Constitution**: Refer to `.specify/memory/constitution.md` for principles

---

**Estimated Time to Productivity:**
- Basic setup: 10 minutes
- Full implementation: 2-3 hours
- Testing and verification: 1 hour
- Total: ~4 hours

Good luck with the workflow improvements! 🚀
