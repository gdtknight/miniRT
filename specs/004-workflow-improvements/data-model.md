# Phase 1: Data Model

**Feature**: Workflow Improvements and Project Structure Reorganization  
**Date**: 2025-12-18  
**Status**: Complete

## Overview

This document defines the data structures, entities, states, and relationships for the workflow improvement system.

---

## Core Entities

### 1. WorkflowRun

Represents a single execution of a CI/CD workflow.

**Fields:**
- `workflow_name`: string - Name of the workflow (ci, pr-validation, release)
- `trigger_event`: string - Event that triggered the run (push, pull_request, tag)
- `ref`: string - Git reference (branch, PR number, tag)
- `status`: WorkflowStatus - Current status of the workflow run
- `started_at`: timestamp - When the workflow started
- `completed_at`: timestamp - When the workflow completed (nullable)
- `checks`: array<QualityCheck> - List of quality checks performed
- `artifacts`: array<Artifact> - Generated artifacts (logs, binaries)

**States (WorkflowStatus):**
- `queued`: Workflow is waiting to start
- `in_progress`: Workflow is currently running
- `success`: All checks passed
- `failure`: One or more checks failed
- `cancelled`: Workflow was cancelled (concurrency control)

**State Transitions:**
```
queued → in_progress → success
                     → failure
                     → cancelled
```

**Relationships:**
- One WorkflowRun has many QualityCheck
- One WorkflowRun produces many Artifact
- WorkflowRun belongs to one Repository

---

### 2. QualityCheck

Represents a single quality gate check (norminette, build, test, memory leak).

**Fields:**
- `check_type`: CheckType - Type of quality check
- `status`: CheckStatus - Pass/fail status
- `started_at`: timestamp - When check started
- `completed_at`: timestamp - When check completed
- `output`: string - Console output (may be truncated)
- `error_message`: string - Detailed error if failed (nullable)
- `exit_code`: integer - Process exit code

**Check Types (CheckType):**
- `norminette`: Code style validation
- `build`: Compilation check
- `test`: Unit/integration tests
- `memory_leak`: valgrind/leaks analysis

**States (CheckStatus):**
- `pending`: Check not yet started
- `running`: Check in progress
- `passed`: Check succeeded
- `failed`: Check failed
- `skipped`: Check not applicable (platform-specific)

**Validation Rules:**
- Exit code 0 = passed
- Exit code non-zero = failed
- Memory leak check: Must report 0 leaks to pass
- Norminette: Must report 0 errors to pass

**State Transitions:**
```
pending → running → passed
                 → failed
                 → skipped
```

**Relationships:**
- QualityCheck belongs to one WorkflowRun
- One QualityCheck may produce one MemoryLeakReport (for memory checks)

---

### 3. MemoryLeakReport

Represents results of memory leak detection (valgrind or leaks).

**Fields:**
- `tool`: string - Tool used (valgrind or leaks)
- `platform`: string - OS platform (linux or macos)
- `scene_file`: string - Test scene that was analyzed
- `total_leaks`: integer - Total number of leaks detected
- `leaked_bytes`: integer - Total bytes leaked
- `leak_locations`: array<LeakLocation> - Where leaks occurred
- `full_log_path`: string - Path to full valgrind/leaks log
- `execution_time`: integer - Seconds taken to run check

**Leak Location:**
- `file`: string - Source file containing leak
- `line`: integer - Line number in source file
- `function`: string - Function where leak occurred
- `bytes`: integer - Bytes leaked at this location
- `stack_trace`: array<string> - Call stack leading to leak

**Validation Rules:**
- `total_leaks` must be 0 to pass
- `leaked_bytes` must be 0 to pass
- Timeout: 600 seconds (10 minutes) per scene

**Relationships:**
- MemoryLeakReport belongs to one QualityCheck
- One MemoryLeakReport analyzes one SceneFile

---

### 4. WikiSyncOperation

Represents an attempt to synchronize documentation to GitHub Wiki.

**Fields:**
- `version_tag`: string - Release version (e.g., v1.0.0)
- `status`: SyncStatus - Current status
- `attempt_number`: integer - Retry attempt (1-3)
- `started_at`: timestamp - When sync started
- `completed_at`: timestamp - When sync completed
- `pages_synced`: array<WikiPage> - Pages that were updated
- `error_message`: string - Error if sync failed (nullable)
- `github_issue_number`: integer - Auto-created issue if failed (nullable)

**States (SyncStatus):**
- `pending`: Sync not yet started
- `in_progress`: Sync is running
- `success`: All pages synced successfully
- `retrying`: Previous attempt failed, retrying
- `failed`: All retry attempts exhausted

**State Transitions:**
```
pending → in_progress → success
                     → retrying → in_progress → success
                                              → failed
                     → failed
```

**Validation Rules:**
- Maximum 3 retry attempts
- Exponential backoff: 0s, 30s, 90s
- Failure does not block release workflow

**Relationships:**
- WikiSyncOperation belongs to one WorkflowRun
- One WikiSyncOperation updates many WikiPage

---

### 5. WikiPage

Represents a single page in the GitHub Wiki.

**Fields:**
- `source_path`: string - Path to source markdown in docs/
- `wiki_path`: string - Path in wiki repository
- `title`: string - Page title (Korean)
- `content`: string - Markdown content
- `version`: string - Version tag this page belongs to
- `last_updated`: timestamp - When page was last synced

**Validation Rules:**
- Source must exist in docs/ directory
- Content must be valid markdown
- File size < 1MB per page
- Title must be non-empty

**Relationships:**
- WikiPage belongs to one WikiSyncOperation
- WikiPage corresponds to one source file in docs/

---

### 6. FileReorganization

Represents a file move operation during project restructuring.

**Fields:**
- `old_path`: string - Original file location
- `new_path`: string - New file location
- `file_type`: FileType - Category of file being moved
- `git_command`: string - Exact git command used (git mv)
- `commit_sha`: string - Commit hash of the move
- `history_preserved`: boolean - Whether git history was preserved

**File Types (FileType):**
- `ci_script`: CI automation script
- `wiki_doc`: Wiki documentation
- `archive`: Obsolete/backup file
- `planning`: Implementation planning document

**Validation Rules:**
- Old path must exist before move
- New path must not exist before move
- Parent directory of new path must exist
- Must use `git mv` command (not manual mv)

**Relationships:**
- FileReorganization belongs to one ReorganizationPlan
- Many FileReorganization operations form one complete reorganization

---

### 7. ReorganizationPlan

Represents the complete project structure reorganization.

**Fields:**
- `plan_id`: string - Unique identifier for this reorganization
- `status`: PlanStatus - Current status of reorganization
- `file_moves`: array<FileReorganization> - All file moves in plan
- `started_at`: timestamp - When reorganization started
- `completed_at`: timestamp - When reorganization completed
- `verification_passed`: boolean - Whether history verification passed

**States (PlanStatus):**
- `planned`: Reorganization defined but not started
- `in_progress`: Files being moved
- `verifying`: Checking history preservation
- `completed`: All moves successful and verified
- `failed`: One or more moves failed

**State Transitions:**
```
planned → in_progress → verifying → completed
                     → failed
```

**Validation Rules:**
- All file moves must preserve git history
- No file should be moved more than once
- Verification: `git log --follow` works for all moved files

**Relationships:**
- One ReorganizationPlan contains many FileReorganization
- ReorganizationPlan is executed once per project

---

### 8. TestResult

Represents the outcome of running test_miniRT.sh.

**Fields:**
- `test_name`: string - Descriptive name of test case
- `scene_file`: string - Scene file being tested
- `expected_result`: TestOutcome - Expected behavior (pass or fail)
- `actual_result`: TestOutcome - Actual behavior observed
- `status`: boolean - Whether test passed (expected == actual)
- `execution_time`: integer - Milliseconds taken to run test
- `error_output`: string - Error message if any (nullable)

**Test Outcome (TestOutcome):**
- `pass`: Program executed successfully
- `fail`: Program exited with error (expected for error test cases)

**Validation Rules:**
- Test passes if `expected_result == actual_result`
- Timeout: 5 seconds per test case
- Scene file must exist

**Relationships:**
- TestResult belongs to one TestSuite
- One TestResult tests one SceneFile

---

### 9. TestSuite

Represents a complete run of the test script.

**Fields:**
- `total_tests`: integer - Number of test cases executed
- `passed_tests`: integer - Number that passed
- `failed_tests`: integer - Number that failed
- `skipped_tests`: integer - Number that were skipped
- `results`: array<TestResult> - Individual test results
- `started_at`: timestamp - When test suite started
- `completed_at`: timestamp - When test suite completed
- `exit_code`: integer - Overall exit code (0 = all passed)

**Validation Rules:**
- `total_tests == passed_tests + failed_tests + skipped_tests`
- Exit code is 0 if and only if `failed_tests == 0`

**Relationships:**
- TestSuite contains many TestResult
- TestSuite belongs to one WorkflowRun

---

### 10. Artifact

Represents a file generated during workflow execution.

**Fields:**
- `artifact_type`: ArtifactType - Type of artifact
- `file_path`: string - Path to artifact file
- `file_size`: integer - Size in bytes
- `created_at`: timestamp - When artifact was created
- `retention_days`: integer - Days to retain (default 90)
- `download_url`: string - GitHub artifact download URL (nullable)

**Artifact Types (ArtifactType):**
- `valgrind_log`: Full valgrind output
- `test_results`: Test suite results
- `build_binary`: Compiled miniRT executable
- `wiki_sync_log`: Wiki synchronization log

**Validation Rules:**
- File size < 100MB per artifact
- Retention days between 1 and 400

**Relationships:**
- Artifact belongs to one WorkflowRun
- Artifacts are uploaded to GitHub Actions artifact storage

---

## Entity Relationship Diagram

```
Repository
    └── WorkflowRun (many)
            ├── QualityCheck (many)
            │       └── MemoryLeakReport (optional, for memory checks)
            ├── Artifact (many)
            ├── WikiSyncOperation (optional, for release workflows)
            │       └── WikiPage (many)
            └── TestSuite (optional, for workflows with tests)
                    └── TestResult (many)

ReorganizationPlan
    └── FileReorganization (many)
```

---

## Workflow Type Requirements

### Development Workflow (ci.yml)
**Triggers**: Push to development branches  
**Required Checks**:
- norminette (QualityCheck)
- build (QualityCheck)
- test (TestSuite)

### PR Validation Workflow (pr-validation.yml)
**Triggers**: Pull request opened/updated  
**Required Checks**:
- norminette (QualityCheck)
- build (QualityCheck)
- test (TestSuite)
- memory_leak (QualityCheck with MemoryLeakReport)

### Release Workflow (release.yml)
**Triggers**: Version tag created (v*.*.*)  
**Required Checks**:
- norminette (QualityCheck)
- build (QualityCheck)
- test (TestSuite)
- memory_leak (QualityCheck with MemoryLeakReport)
- wiki_sync (WikiSyncOperation)

**Additional Outputs**:
- Build artifacts (Artifact)

---

## State Management

### Concurrency Handling

**Concurrency Groups**:
```
ci-{ref}           # Development workflow
pr-{pr_number}     # PR validation workflow
release-{ref}      # Release workflow
```

**Cancellation Policy**:
- Development: Cancel older runs on new push
- PR Validation: Cancel older runs on new commit
- Release: Never cancel (cancel-in-progress: false)

**State Resolution**:
- When workflow is cancelled: Status → `cancelled`
- Older runs are stopped before new run starts
- Artifacts from cancelled runs are not retained

---

## Data Persistence

### GitHub Actions Native Storage
- WorkflowRun: GitHub Actions run metadata
- QualityCheck: Job and step results
- Artifact: GitHub Actions artifact storage (90 days)

### Git Repository Storage
- FileReorganization: Git history and commits
- ReorganizationPlan: Documentation in spec files

### GitHub Wiki Storage
- WikiPage: Wiki repository (separate git repo)
- WikiSyncOperation: Workflow logs and artifacts

### File System Storage (CI runners)
- MemoryLeakReport: Temporary logs (uploaded to artifacts)
- TestResult: Temporary output (uploaded to artifacts)

---

## Data Flows

### 1. PR Validation Flow
```
Developer pushes code
    → WorkflowRun created (pr-validation)
    → QualityCheck: norminette
    → QualityCheck: build
    → QualityCheck: test (TestSuite)
    → QualityCheck: memory_leak (MemoryLeakReport)
    → All checks pass? → WorkflowRun status = success
    → Any check fails? → WorkflowRun status = failure
```

### 2. Release Flow
```
Maintainer creates tag
    → WorkflowRun created (release)
    → All PR validation checks run
    → WikiSyncOperation attempts sync
        → Success? → Continue
        → Failure? → Retry up to 3 times
            → Still fails? → Create GitHub issue, continue anyway
    → Generate build artifacts (Artifact)
    → WorkflowRun status = success (even if wiki sync failed)
```

### 3. File Reorganization Flow
```
ReorganizationPlan created
    → For each file to move:
        → FileReorganization created
        → Execute git mv command
        → Verify history preserved
        → FileReorganization marked complete
    → All moves complete?
        → ReorganizationPlan status = verifying
        → Run verification tests
        → ReorganizationPlan status = completed
```

---

## Performance Considerations

### Timeouts
- **norminette**: 2 minutes
- **build**: 5 minutes
- **test**: 5 minutes
- **memory_leak**: 10 minutes per scene, 30 minutes total
- **wiki_sync**: 2 minutes per attempt

### Parallelization
- Multiple scene files can be tested in parallel (memory leak)
- Different workflows run in parallel (different refs)
- Quality checks within a workflow run sequentially

### Resource Limits
- Maximum 20 concurrent workflow runs per repository
- Maximum 10GB artifact storage per repository
- Maximum 6 hours per workflow run (GitHub Actions limit)

---

## Next Steps (Contracts)

With data model defined, Phase 1 continues with:
1. contracts/workflows.md - GitHub Actions workflow specifications
2. contracts/memory-check.md - Memory leak detection contract
3. contracts/wiki-sync.md - Wiki synchronization contract
