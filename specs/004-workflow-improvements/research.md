# Phase 0: Research & Technical Decisions

**Feature**: Workflow Improvements and Project Structure Reorganization  
**Date**: 2025-12-18  
**Status**: Complete

## Research Overview

This document resolves all NEEDS CLARIFICATION items from Technical Context and establishes technical decisions for implementing workflow improvements.

---

## 1. Git History Preservation During File Reorganization

### Decision
Use `git mv` command for all file relocations to preserve complete file history.

### Rationale
- `git mv` is Git's native command for moving files while maintaining history
- `git log --follow <filename>` automatically tracks files across moves
- Preserves blame information and commit history
- Atomic operation ensures consistency

### Implementation Details
```bash
# Example move preserving history
git mv IMPLEMENTATION_PLAN.txt docs/project/IMPLEMENTATION_PLAN.txt
git mv README_OLD.md docs/archive/README_OLD.md
git mv WIKI_READY.md docs/wiki/WIKI_READY.md
```

### Verification Method
```bash
# Verify history is preserved
git log --follow docs/project/IMPLEMENTATION_PLAN.txt
```

### Alternatives Considered
- **Manual move + git add**: Loses history connection, requires manual tracking
- **cp + git rm**: Creates new file, loses all historical context
- **Symlinks**: Adds complexity, doesn't truly reorganize structure

### Best Practices
- Create target directories before moving files
- One file per commit for clarity (or logical groups)
- Update all references to moved files in same commit
- Document moves in commit messages with old and new paths

---

## 2. Valgrind Memory Leak Detection on Linux

### Decision
Install valgrind automatically in CI using apt-get, with 10-minute timeout per test scene.

### Rationale
- valgrind is standard Linux memory debugging tool
- Available in Ubuntu package repositories (no compilation needed)
- Provides detailed leak reports with stack traces
- Zero-leak policy enforceable via exit codes

### Implementation Details
```bash
# Auto-installation in CI
if ! command -v valgrind &> /dev/null; then
    sudo apt-get update -qq
    sudo apt-get install -y valgrind
fi

# Execution with timeout
timeout 600 valgrind --leak-check=full --show-leak-kinds=all \
    --track-origins=yes --error-exitcode=1 \
    ./miniRT scenes/test_scene.rt > /dev/null
```

### Configuration Options
- `--leak-check=full`: Detailed leak information
- `--show-leak-kinds=all`: Show definitely/possibly/still reachable leaks
- `--track-origins=yes`: Track origin of uninitialized values
- `--error-exitcode=1`: Return non-zero on errors (CI integration)

### Performance Characteristics
- Slowdown: 10-20x compared to normal execution
- Memory overhead: 2-3x normal memory usage
- Timeout: 10 minutes per scene sufficient for miniRT

### Alternatives Considered
- **AddressSanitizer**: Requires recompilation, less detailed leak info
- **Memcheck**: Part of valgrind, same tool
- **Dr. Memory**: Windows-focused, less Linux support

### Best Practices
- Run valgrind on representative test scenes
- Capture first 1000 and last 500 lines of output
- Upload full logs as artifacts for debugging
- Suppress known library leaks (MinilibX if needed)

---

## 3. macOS Memory Leak Detection

### Decision
Use built-in `leaks` command on macOS with process ID tracking.

### Rationale
- Pre-installed on macOS (no installation needed)
- Native Apple tool, optimized for macOS
- Compatible with miniRT's graphics libraries
- Provides similar output to valgrind

### Implementation Details
```bash
# macOS leak detection
./miniRT scenes/test_scene.rt &
PID=$!
sleep 5  # Let program run
leaks $PID
kill $PID 2>/dev/null || true
```

### Alternative: MallocStackLogging
```bash
# More detailed tracking
export MallocStackLogging=1
./miniRT scenes/test_scene.rt
leaks miniRT
```

### Platform Detection
```bash
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Use valgrind
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # Use leaks
fi
```

### Alternatives Considered
- **valgrind on macOS**: Not officially supported on Apple Silicon
- **Instruments**: GUI tool, not CI-friendly
- **Address Sanitizer**: Requires recompilation flags

### Best Practices
- Use consistent process naming for leak tracking
- Handle process cleanup in all exit paths
- Combine with MallocStackLogging for detailed traces
- Test on both Intel and Apple Silicon

---

## 4. GitHub Actions Concurrency Control

### Decision
Use GitHub Actions concurrency groups with cancel-in-progress strategy for same-branch workflows.

### Rationale
- Native GitHub Actions feature (no external tools)
- Prevents resource waste from outdated workflow runs
- Configurable per workflow and branch
- Handles race conditions automatically

### Implementation Details
```yaml
# In workflow YAML
concurrency:
  group: ${{ github.workflow }}-${{ github.ref }}
  cancel-in-progress: true
```

### Behavior by Scenario
- **Push + PR simultaneously**: Different refs, run in parallel
- **Multiple pushes same branch**: Cancel older, run newest
- **Different branches**: Always run in parallel
- **Tag workflow**: Separate group, never cancelled

### Configuration Strategy
```yaml
# Development workflow (ci.yml)
concurrency:
  group: ci-${{ github.ref }}
  cancel-in-progress: true

# PR validation (pr-validation.yml)
concurrency:
  group: pr-${{ github.event.pull_request.number }}
  cancel-in-progress: true

# Release workflow (release.yml)
concurrency:
  group: release-${{ github.ref }}
  cancel-in-progress: false  # Never cancel releases
```

### Alternatives Considered
- **Manual workflow cancellation**: Requires manual intervention
- **Queue system**: Adds complexity, slower feedback
- **External orchestration**: Unnecessary for this use case

### Best Practices
- Use descriptive group names including workflow and ref
- Never cancel release/deployment workflows
- Document concurrency behavior in workflow comments
- Test with multiple simultaneous pushes

---

## 5. Wiki Synchronization Automation

### Decision
Create dedicated wiki-sync.yml workflow triggered by version tags with retry logic and failure isolation.

### Rationale
- Separates wiki concerns from release workflow
- Retry mechanism handles transient network issues
- Failure doesn't block release completion
- Automatic issue creation for failed syncs

### Implementation Details
```yaml
# Trigger on version tags
on:
  push:
    tags:
      - 'v*.*.*'

# Retry with exponential backoff
- name: Sync Wiki
  uses: actions/github-script@v7
  with:
    retries: 3
    retry-on: error
```

### Retry Strategy
- **Attempt 1**: Immediate execution
- **Attempt 2**: 30 second delay
- **Attempt 3**: 90 second delay
- **After 3 failures**: Log error, create issue, continue

### Failure Handling
```bash
# Pseudo-code for error handling
if wiki_sync_fails; then
    log_to_artifacts "Wiki sync failed: $error"
    create_github_issue "Wiki sync failed for $tag"
    exit 0  # Don't fail the release
fi
```

### Prerequisites Check
```bash
# Verify prerequisites before attempting sync
check_github_token() {
    if [ -z "$GITHUB_TOKEN" ]; then
        echo "ERROR: GITHUB_TOKEN not set"
        exit 1
    fi
}

check_wiki_enabled() {
    # Use GitHub API to verify wiki is enabled
    gh api repos/$REPO/wiki || {
        echo "WARNING: Wiki not enabled, skipping sync"
        exit 0
    }
}
```

### Alternatives Considered
- **Inline in release workflow**: Failure would block releases
- **Manual synchronization**: Prone to forgotten updates
- **Third-party action**: Additional dependency, less control

### Best Practices
- Always check prerequisites before attempting sync
- Log all failures to artifacts for debugging
- Create GitHub issues automatically for failed syncs
- Include version information in all wiki pages
- Test with mock failures to verify error handling

---

## 6. Test Script Enhancement Strategy

### Decision
Enhance test_miniRT.sh with structured error cases, clear pass/fail indicators, and CI-compatible exit codes.

### Rationale
- Bash script is simple, portable, no dependencies
- Can test error conditions that valgrind cannot
- Clear output improves debugging experience
- Exit codes enable CI integration

### Implementation Structure
```bash
#!/bin/bash

# Test counter and results
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Test execution function
run_test() {
    local test_name="$1"
    local test_file="$2"
    local expected_result="$3"  # "pass" or "fail"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    if ./miniRT "$test_file" > /dev/null 2>&1; then
        actual="pass"
    else
        actual="fail"
    fi
    
    if [ "$actual" == "$expected_result" ]; then
        echo "✓ PASS: $test_name"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "✗ FAIL: $test_name (expected $expected_result, got $actual)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

# Error test cases
run_test "Missing ambient light" "scenes/error/no_ambient.rt" "fail"
run_test "Invalid camera position" "scenes/error/bad_camera.rt" "fail"
run_test "Negative sphere radius" "scenes/error/negative_radius.rt" "fail"

# Summary and exit
echo ""
echo "Test Summary: $PASSED_TESTS/$TOTAL_TESTS passed"
[ $FAILED_TESTS -eq 0 ] && exit 0 || exit 1
```

### Test Categories
1. **Parse Errors**: Invalid syntax, wrong types
2. **Semantic Errors**: Invalid values (negative radius)
3. **Missing Required**: No ambient, camera, or light
4. **Edge Cases**: Empty file, huge values, UTF-8 issues

### Output Format
```
Running miniRT Test Suite
========================

✓ PASS: Valid scene with sphere
✓ PASS: Missing ambient light (error expected)
✗ FAIL: Invalid camera position (expected fail, got pass)
✓ PASS: Multiple objects

Test Summary: 3/4 passed
FAILED: 1 test(s) failed
```

### Alternatives Considered
- **Python test framework**: Adds dependency, overkill for simple tests
- **C-based tests**: Requires compilation, less accessible
- **Manual testing**: Not reproducible, no CI integration

### Best Practices
- One scene file per test case
- Clear test names describing what is tested
- Structured output with pass/fail indicators
- Summary statistics at end
- Non-zero exit code on any failure

---

## 7. Log Management for CI Workflows

### Decision
Capture first 1000 and last 500 lines of valgrind output to console, upload full logs as artifacts.

### Rationale
- Balances immediate visibility with storage efficiency
- Key information (leaks) typically at start or end
- Full logs available for deep debugging
- Prevents CI log overflow (10MB limit)

### Implementation
```bash
# Capture and display relevant portions
valgrind --leak-check=full ... ./miniRT scene.rt 2>&1 | tee valgrind_full.log

# Display summary in console
head -n 1000 valgrind_full.log
echo "... (middle section truncated, see artifacts) ..."
tail -n 500 valgrind_full.log

# Upload full log
# (GitHub Actions artifact upload step)
```

### Artifact Organization
```
artifacts/
├── valgrind-logs/
│   ├── scene1-valgrind.log
│   ├── scene2-valgrind.log
│   └── summary.txt
└── test-results/
    └── test-summary.txt
```

### Retention Policy
- Artifacts retained for 90 days (GitHub default)
- Can be adjusted per repository settings
- Critical failures: Download and archive manually

### Alternatives Considered
- **Full logs to console**: Exceeds GitHub log limits
- **Summary only**: Insufficient for debugging
- **External log storage**: Adds complexity and cost

### Best Practices
- Always upload full logs as artifacts
- Include timestamps in log filenames
- Compress large log files before upload
- Link to artifacts in PR comments for failures

---

## 8. Platform-Specific Tool Installation

### Decision
Create installation scripts that detect platform and install appropriate tools automatically.

### Rationale
- Ensures consistent environment across local and CI
- Reduces setup friction for contributors
- Clear error messages if installation fails
- Testable and maintainable

### Implementation: install-valgrind.sh
```bash
#!/bin/bash
set -e

install_valgrind_linux() {
    echo "Installing valgrind on Linux..."
    sudo apt-get update -qq
    sudo apt-get install -y valgrind
    valgrind --version
}

install_valgrind_macos() {
    echo "macOS detected: using built-in 'leaks' command"
    if ! command -v leaks &> /dev/null; then
        echo "ERROR: leaks command not found. Install Xcode Command Line Tools:"
        echo "  xcode-select --install"
        exit 1
    fi
}

install_valgrind_unsupported() {
    echo "ERROR: Unsupported platform: $OSTYPE"
    echo "Supported platforms: Linux (valgrind), macOS (leaks)"
    exit 1
}

# Main installation logic
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    install_valgrind_linux
elif [[ "$OSTYPE" == "darwin"* ]]; then
    install_valgrind_macos
else
    install_valgrind_unsupported
fi

echo "Memory leak detection tools ready!"
```

### Error Messages
```
✓ Success: valgrind 3.18.1 installed
✗ Failure: Unable to install valgrind
  → Check internet connection
  → Verify sudo permissions
  → Try manual installation: apt-get install valgrind
```

### Alternatives Considered
- **Docker containers**: Heavyweight, slower startup
- **Pre-built images**: Less flexible, harder to debug
- **Manual installation docs**: Prone to being outdated

### Best Practices
- Detect platform before attempting installation
- Provide clear instructions for manual installation
- Verify installation success before proceeding
- Support both CI and local development environments

---

## 9. GitHub Actions Workflow Error Handling

### Decision
Implement explicit error handling with `set -e`, status checks, and conditional continuation.

### Rationale
- Prevents silent failures in CI
- Enables graceful degradation for non-critical steps
- Clear failure attribution for debugging
- Follows fail-fast principle for critical errors

### Implementation Patterns

**Critical Step (must succeed):**
```yaml
- name: Build miniRT
  run: |
    set -e  # Exit on any error
    make
```

**Non-Critical Step (can fail):**
```yaml
- name: Update Wiki
  continue-on-error: true
  run: |
    ./sync-wiki.sh || {
      echo "::warning::Wiki sync failed, continuing anyway"
      exit 0
    }
```

**Conditional Step (run only if previous succeeded):**
```yaml
- name: Deploy artifacts
  if: success()
  run: |
    ./deploy.sh
```

**Always Run (cleanup):**
```yaml
- name: Cleanup
  if: always()
  run: |
    rm -rf temp/
```

### Error Message Best Practices
```bash
# Good: Clear, actionable error message
echo "::error::Memory leaks detected in scene: $scene"
echo "::error::Leaked bytes: $bytes at $location"
echo "::error::Run locally: valgrind ./miniRT $scene"

# Bad: Vague error message
echo "Error occurred"
```

### Alternatives Considered
- **Ignore all errors**: Dangerous, hides problems
- **Fail everything**: Too strict, blocks progress
- **Manual checks**: Not scalable, error-prone

### Best Practices
- Use `set -e` for critical scripts
- Add `|| true` for steps that may legitimately fail
- Use GitHub Actions annotations (::error::, ::warning::)
- Provide context and remediation steps in error messages
- Test error paths (intentional failures)

---

## Summary of Technical Decisions

| Area | Decision | Key Benefit |
|------|----------|-------------|
| File History | git mv command | Preserves complete history |
| Linux Memory Check | valgrind with auto-install | Standard tool, detailed reports |
| macOS Memory Check | Built-in leaks command | No installation needed |
| Concurrency | GitHub Actions concurrency groups | Prevents resource waste |
| Wiki Sync | Separate workflow with retry | Failure doesn't block releases |
| Test Enhancement | Bash with structured output | Simple, portable, CI-friendly |
| Log Management | Console summary + artifacts | Balance visibility and storage |
| Tool Installation | Platform-detection script | Automatic, clear errors |
| Error Handling | Explicit checks, conditional steps | Fail-fast with graceful degradation |

---

## Next Steps (Phase 1)

With all technical decisions resolved, Phase 1 will:
1. Generate data-model.md defining workflow entities and states
2. Create contracts/ for CI workflow specifications
3. Generate quickstart.md for developer onboarding
4. Update agent context with new technologies and patterns
