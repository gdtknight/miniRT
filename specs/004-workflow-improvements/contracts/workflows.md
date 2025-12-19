# Contract: GitHub Actions Workflows

**Version**: 1.0.0  
**Date**: 2025-12-18  
**Status**: Specification

## Overview

This contract defines the behavior, inputs, outputs, and guarantees for all GitHub Actions workflows in the miniRT project.

---

## Workflow: Development CI (ci.yml)

### Purpose
Validate code quality on development branches before PR creation.

### Triggers
```yaml
on:
  push:
    branches:
      - develop
      - 'feature/**'
      - 'bugfix/**'
    paths-ignore:
      - '**.md'
      - 'docs/**'
```

### Concurrency
```yaml
concurrency:
  group: ci-${{ github.ref }}
  cancel-in-progress: true
```

### Jobs

#### 1. norminette-check
**Platform**: ubuntu-latest  
**Timeout**: 2 minutes

**Steps**:
1. Checkout code
2. Install norminette
3. Run norminette on src/ and includes/
4. Exit with code 1 if any violations

**Success Criteria**:
- norminette reports 0 errors
- norminette reports 0 warnings

**Failure Behavior**:
- Fails workflow
- Displays specific files and lines with violations

#### 2. build
**Platform**: ubuntu-latest, macos-latest (matrix)  
**Timeout**: 5 minutes  
**Depends On**: norminette-check

**Steps**:
1. Checkout code
2. Install dependencies (MinilibX)
3. Run `make`
4. Verify miniRT binary exists
5. Run `make clean` (verify no errors)

**Success Criteria**:
- `make` completes with exit code 0
- miniRT executable created
- No compilation warnings (with -Wall -Wextra -Werror)

**Failure Behavior**:
- Fails workflow
- Displays compilation errors

#### 3. test
**Platform**: ubuntu-latest  
**Timeout**: 5 minutes  
**Depends On**: build

**Steps**:
1. Use miniRT binary from build job
2. Run test_miniRT.sh
3. Capture test results

**Success Criteria**:
- test_miniRT.sh exits with code 0
- All expected tests pass

**Failure Behavior**:
- Fails workflow
- Displays failed test cases

### Outputs
- Workflow status (success/failure)
- Console logs for each job

### Guarantees
- Only latest push to a branch will complete (older runs cancelled)
- All jobs run on each platform
- No artifacts uploaded (development stage only)

---

## Workflow: PR Validation (pr-validation.yml)

### Purpose
Comprehensive validation before merging code, including memory leak detection.

### Triggers
```yaml
on:
  pull_request:
    types: [opened, synchronize, reopened]
    paths-ignore:
      - '**.md'
      - 'docs/**'
```

### Concurrency
```yaml
concurrency:
  group: pr-${{ github.event.pull_request.number }}
  cancel-in-progress: true
```

### Jobs

#### 1. norminette-check
**Same as Development CI**

#### 2. build
**Same as Development CI**

#### 3. test
**Same as Development CI**

#### 4. memory-leak-check
**Platform**: ubuntu-latest, macos-latest (matrix)  
**Timeout**: 30 minutes  
**Depends On**: build

**Steps**:
1. Install memory checking tools
   - Linux: Install valgrind via apt-get
   - macOS: Verify leaks command available
2. For each scene in scenes/test/*.rt:
   - Run miniRT with valgrind/leaks
   - Capture output
   - Check for leaks
3. Display summary of leaks (if any)
4. Upload full logs as artifacts

**Success Criteria**:
- valgrind reports: "All heap blocks were freed -- no leaks are possible"
- leaks reports: "0 leaks for 0 total leaked bytes"
- Exit code 0 from memory check tool

**Failure Behavior**:
- Fails workflow
- Displays leak location, bytes leaked, scene file
- Provides command to reproduce locally
- Uploads full valgrind/leaks log as artifact

**Timeout Behavior**:
- If single scene exceeds 10 minutes, kill and report timeout
- Mark scene as failed
- Continue with remaining scenes

### Outputs
- Workflow status (success/failure)
- Console logs for each job
- Artifacts:
  - valgrind-logs.tar.gz (all valgrind outputs)
  - memory-leak-summary.txt (summary of leaks found)

### Guarantees
- Memory check runs on all test scenes
- Full logs always uploaded, even on timeout
- PR cannot be merged with memory leaks
- Only latest commit to PR will complete (older runs cancelled)

---

## Workflow: Release (release.yml)

### Purpose
Create production release with full validation, artifacts, and wiki update.

### Triggers
```yaml
on:
  push:
    tags:
      - 'v*.*.*'
```

### Concurrency
```yaml
concurrency:
  group: release-${{ github.ref }}
  cancel-in-progress: false  # Never cancel releases
```

### Jobs

#### 1-4. All PR Validation Jobs
**Same as PR Validation workflow**  
All quality checks must pass.

#### 5. build-artifacts
**Platform**: ubuntu-latest, macos-latest (matrix)  
**Timeout**: 10 minutes  
**Depends On**: memory-leak-check (all platforms)

**Steps**:
1. Checkout code
2. Build miniRT with release flags
3. Strip debugging symbols
4. Create release archive
   - miniRT-{version}-{platform}.tar.gz
   - Includes: binary, README.md, scenes/
5. Upload as artifact

**Success Criteria**:
- Build completes successfully
- Archive created and under 50MB
- Binary is executable

**Failure Behavior**:
- Fails workflow
- Displays build errors
- No release created

#### 6. sync-wiki
**Platform**: ubuntu-latest  
**Timeout**: 10 minutes  
**Depends On**: build-artifacts  
**Continue On Error**: true

**Steps**:
1. Checkout wiki repository
2. Extract version from tag
3. For each doc in docs/**/*.md:
   - Convert to wiki format
   - Add version footer
   - Commit to wiki repo
4. Push to wiki repository
5. On failure:
   - Log error to artifacts
   - Create GitHub issue with error details
   - Continue (don't fail release)

**Success Criteria**:
- All docs synced to wiki
- Wiki repository updated

**Failure Behavior**:
- Logs warning
- Creates GitHub issue
- **Does not fail release workflow**

**Retry Logic**:
- Attempt 1: Immediate
- Attempt 2: 30 second delay
- Attempt 3: 90 second delay
- After 3 failures: Log and continue

#### 7. create-release
**Platform**: ubuntu-latest  
**Timeout**: 5 minutes  
**Depends On**: sync-wiki (always runs regardless of wiki status)

**Steps**:
1. Download all build artifacts
2. Create GitHub release
   - Title: "miniRT {version}"
   - Body: Auto-generated changelog
   - Assets: All platform binaries
3. Mark as latest release

**Success Criteria**:
- GitHub release created
- All artifacts attached
- Release is visible

**Failure Behavior**:
- Fails workflow
- Logs error
- Manual release creation required

### Outputs
- Workflow status (success/failure)
- GitHub release with binaries
- Updated wiki (if sync succeeded)
- Artifacts:
  - miniRT-{version}-linux.tar.gz
  - miniRT-{version}-macos.tar.gz
  - wiki-sync-log.txt
  - All logs from quality checks

### Guarantees
- All quality checks pass before release creation
- Wiki sync failure does not block release
- Release always created if quality checks pass
- Artifacts retained for 90 days

---

## Workflow: Wiki Sync (wiki-sync.yml)

### Purpose
Automated wiki updates triggered by release tags. Can also be manually triggered to repair failed syncs.

### Triggers
```yaml
on:
  push:
    tags:
      - 'v*.*.*'
  workflow_dispatch:
    inputs:
      version:
        description: 'Version tag to sync (e.g., v1.0.0)'
        required: true
```

### Concurrency
```yaml
concurrency:
  group: wiki-sync-${{ github.event.inputs.version || github.ref }}
  cancel-in-progress: false
```

### Jobs

#### 1. sync
**Platform**: ubuntu-latest  
**Timeout**: 10 minutes

**Steps**:
1. Check prerequisites:
   - GITHUB_TOKEN exists
   - Wiki enabled in repository settings
   - Version tag exists
2. Checkout wiki repository
   - URL: https://github.com/{owner}/{repo}.wiki.git
3. Checkout main repository at specified tag
4. For each markdown file in docs/:
   - Parse frontmatter
   - Add version information
   - Copy to wiki repository with correct naming
5. Commit changes:
   - Message: "Update wiki for version {version}"
   - Author: GitHub Actions bot
6. Push to wiki repository with retry logic
7. On success: Log summary
8. On failure: Create GitHub issue

**Success Criteria**:
- All docs copied to wiki
- Wiki repository push succeeds
- No conflicts

**Failure Behavior**:
- Retry up to 3 times with exponential backoff
- Create issue: "Wiki sync failed for {version}"
  - Title: "🚨 Wiki Sync Failure: {version}"
  - Labels: documentation, bug, automated
  - Body: Error details and manual sync instructions
- Exit with status 0 (don't fail if called from release workflow)

**Manual Sync Instructions** (in issue):
```bash
# Clone wiki repo
git clone https://github.com/{owner}/{repo}.wiki.git

# Copy docs manually
cp -r /path/to/docs/* wiki/

# Commit and push
cd wiki
git add .
git commit -m "Manual sync for {version}"
git push
```

### Outputs
- Workflow status (success/warning)
- Updated wiki (if successful)
- GitHub issue (if failed)
- Artifact: wiki-sync-detailed-log.txt

### Guarantees
- Always creates issue on failure
- Never fails release workflow (when called from release.yml)
- Can be re-run manually with workflow_dispatch
- Idempotent (safe to run multiple times)

---

## Cross-Workflow Contracts

### Environment Variables
All workflows have access to:
- `GITHUB_TOKEN`: Authentication for API and wiki access
- `GITHUB_REPOSITORY`: Repository owner/name
- `GITHUB_SHA`: Commit SHA being tested
- `GITHUB_REF`: Git reference (branch, tag, or PR)

### Artifact Naming Convention
- `{job-name}-{platform}-{timestamp}`
- Example: `memory-leak-check-ubuntu-20231218-153045`

### Log Format
All scripts must output in this format:
```
::group::Step Name
... output ...
::endgroup::

::error::Error message
::warning::Warning message
::notice::Notice message
```

### Exit Codes
- `0`: Success
- `1`: Generic failure
- `2`: Configuration error
- `124`: Timeout (from timeout command)

### Platform Matrix
```yaml
strategy:
  matrix:
    os: [ubuntu-latest, macos-latest]
  fail-fast: false  # Continue testing other platforms on failure
```

---

## Error Scenarios

### Scenario: Memory leak detected
**Workflow**: pr-validation.yml  
**Job**: memory-leak-check

**Behavior**:
1. Display leak details in console
2. Upload full valgrind log as artifact
3. Fail job with exit code 1
4. Add comment to PR with leak summary and artifact link

### Scenario: Build failure on macOS only
**Workflow**: ci.yml  
**Job**: build

**Behavior**:
1. Linux job continues and completes
2. macOS job fails
3. Overall workflow marked as failed
4. Both platform results visible in checks

### Scenario: Wiki sync timeout
**Workflow**: release.yml  
**Job**: sync-wiki

**Behavior**:
1. Workflow waits up to 10 minutes
2. After timeout, mark job as failed
3. Create GitHub issue
4. Continue with create-release job
5. Overall workflow succeeds (wiki sync is non-blocking)

### Scenario: Concurrent pushes to same branch
**Workflow**: ci.yml

**Behavior**:
1. First push starts workflow run #1
2. Second push starts workflow run #2
3. GitHub Actions cancels run #1
4. Only run #2 completes
5. Resources freed from run #1

---

## Testing Strategy

### Workflow Testing
- **Manual trigger**: Use workflow_dispatch for testing
- **Test branches**: Create test/* branches for workflow development
- **Mock failures**: Intentionally introduce errors to verify failure paths

### Local Testing
Scripts should be runnable locally:
```bash
# Test norminette check
bash .github/scripts/check-norminette.sh

# Test memory leak check
bash .github/scripts/check-memory-leaks.sh scenes/test/sphere.rt

# Test wiki sync (dry-run)
bash .github/scripts/sync-wiki.sh --dry-run v1.0.0
```

---

## Maintenance

### Adding New Checks
1. Create script in .github/scripts/
2. Add job to appropriate workflow
3. Update this contract document
4. Test on test branch before merging

### Modifying Timeouts
- Update both workflow YAML and this contract
- Consider impact on overall workflow duration
- Test with worst-case scenarios

### Deprecating Workflows
- Mark as deprecated in contract
- Update README with migration path
- Keep workflow for 2 releases before removing

---

## Version History

- **1.0.0** (2025-12-18): Initial contract specification
