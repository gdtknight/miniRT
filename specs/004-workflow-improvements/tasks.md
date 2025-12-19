# Tasks: Workflow Improvements and Project Structure Reorganization

**Input**: Design documents from `/specs/001-workflow-improvements/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

**Tests**: Not requested in specification - focusing on implementation and validation tasks

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

- Single C project at repository root
- CI scripts in `.github/scripts/`
- Workflows in `.github/workflows/`
- Documentation in `docs/` with subdirectories
- User tools remain at project root

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Create directory structure and prepare for reorganization

**Estimated Time**: 15-30 minutes

- [ ] T001 Create `.github/scripts/` directory for CI automation scripts
- [ ] T002 [P] Create `docs/wiki/` directory for wiki documentation
- [ ] T003 [P] Create `docs/archive/` directory for obsolete files
- [ ] T004 [P] Create `docs/project/` directory for implementation planning docs

**Success Criteria**: All target directories exist and are tracked in git

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before user stories can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

**Estimated Time**: 1-2 hours

- [ ] T005 Create `.github/scripts/check-memory-leaks.sh` memory leak detection wrapper script
- [ ] T006 [P] Create `.github/scripts/install-valgrind.sh` platform-specific installation script
- [ ] T007 [P] Create `.github/scripts/sync-wiki.sh` wiki synchronization script
- [ ] T008 Make all scripts in `.github/scripts/` executable (chmod +x)

**Success Criteria**: All foundational scripts exist and are executable

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Project Structure Organization (Priority: P1) 🎯 MVP

**Goal**: Reorganize project structure using git mv to preserve file history, with CI scripts in `.github/scripts/`, documentation organized by purpose, and user tools remaining at root

**Independent Test**: Verify all files moved to correct locations and `git log --follow` shows complete history for each moved file

**Estimated Time**: 30-45 minutes

### Implementation for User Story 1

- [ ] T009 [US1] Move `IMPLEMENTATION_PLAN.txt` to `docs/project/IMPLEMENTATION_PLAN.txt` using git mv
- [ ] T010 [US1] Move `README_OLD.md` to `docs/archive/README_OLD.md` using git mv
- [ ] T011 [US1] Move `README.md.backup` to `docs/archive/README.md.backup` using git mv (if exists)
- [ ] T012 [US1] Move `WIKI_READY.md` to `docs/wiki/WIKI_READY.md` using git mv
- [ ] T013 [US1] Move `WIKI_SETUP.md` to `docs/wiki/WIKI_SETUP.md` using git mv
- [ ] T014 [US1] Verify git history preserved with `git log --follow` for all moved files
- [ ] T015 [US1] Update any internal references to moved files in documentation and scripts
- [ ] T016 [US1] Commit reorganization changes with descriptive message documenting all moves

**Validation Steps**:
```bash
# Verify history preservation
git log --follow docs/project/IMPLEMENTATION_PLAN.txt
git log --follow docs/archive/README_OLD.md
git log --follow docs/wiki/WIKI_READY.md
git log --follow docs/wiki/WIKI_SETUP.md

# Verify root directory is clean
ls -la | grep -E '\.(md|txt)$' | wc -l  # Should be minimal (README.md, LICENSE, etc.)
```

**Success Criteria**: 
- All planning docs in `docs/project/`
- All obsolete files in `docs/archive/`
- All wiki docs in `docs/wiki/`
- Root contains <10 files (excluding directories)
- 100% git history preserved for all moved files

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently

---

## Phase 4: User Story 2 - Automated Memory Leak Detection (Priority: P2)

**Goal**: Implement automatic memory leak detection using valgrind (Linux) and leaks (macOS) that runs on all PRs, fails CI on any memory leaks, and provides detailed error messages with leak location

**Independent Test**: Create test PR with intentional memory leak and verify CI fails with clear error message showing leak location and test scene

**Estimated Time**: 2-3 hours

### Implementation for User Story 2

- [ ] T017 [US2] Implement platform detection logic in `.github/scripts/check-memory-leaks.sh`
- [ ] T018 [US2] Add valgrind execution with full leak checking in `.github/scripts/check-memory-leaks.sh`
- [ ] T019 [US2] Add macOS leaks command execution with process tracking in `.github/scripts/check-memory-leaks.sh`
- [ ] T020 [US2] Implement output parsing and error reporting in `.github/scripts/check-memory-leaks.sh`
- [ ] T021 [US2] Add timeout handling (10 minutes per scene) in `.github/scripts/check-memory-leaks.sh`
- [ ] T022 [US2] Implement Linux valgrind auto-installation in `.github/scripts/install-valgrind.sh`
- [ ] T023 [US2] Implement macOS prerequisites check in `.github/scripts/install-valgrind.sh`
- [ ] T024 [US2] Add error handling for unsupported platforms in `.github/scripts/install-valgrind.sh`
- [ ] T025 [US2] Test memory leak detection script locally on valid scene (should pass)
- [ ] T026 [US2] Create test scene with intentional leak for validation (temporary, for testing only)

**Validation Steps**:
```bash
# Test on Linux (if available)
.github/scripts/install-valgrind.sh
.github/scripts/check-memory-leaks.sh scenes/test/sphere.rt

# Test on macOS (if available)
.github/scripts/check-memory-leaks.sh scenes/test/sphere.rt

# Should exit with code 0 for leak-free code
echo $?
```

**Success Criteria**:
- Memory leak detection executes on all test scenes
- Zero tolerance: CI fails if any leaks detected
- Error messages include leak location within 3 lines of context
- Completes within 10 minutes per scene
- Works on both Linux (valgrind) and macOS (leaks)

**Checkpoint**: At this point, User Stories 1 AND 2 should both work independently

---

## Phase 5: User Story 3 - Unified Quality Check Workflow (Priority: P3)

**Goal**: Create comprehensive quality check workflows (Development, PR Validation, Release) that run automatically with norminette, build, tests, and memory leak checks, providing consistent quality gates with clear pass/fail indicators

**Independent Test**: Trigger workflows at each stage (push to dev, create PR, create tag) and verify appropriate checks run with clear results

**Estimated Time**: 2-3 hours

### Implementation for User Story 3

- [ ] T027 [US3] Update `.github/workflows/ci.yml` with concurrency control for development branches
- [ ] T028 [US3] Add norminette, build, and test jobs to `.github/workflows/ci.yml`
- [ ] T029 [US3] Add matrix strategy for Linux and macOS in `.github/workflows/ci.yml`
- [ ] T030 [US3] Update `.github/workflows/pr-validation.yml` with PR-specific triggers
- [ ] T031 [US3] Add memory leak check job to `.github/workflows/pr-validation.yml`
- [ ] T032 [US3] Add concurrency control for PR validation in `.github/workflows/pr-validation.yml`
- [ ] T033 [US3] Add artifact upload for memory leak logs on failure in `.github/workflows/pr-validation.yml`
- [ ] T034 [US3] Update `.github/workflows/release.yml` with version tag triggers
- [ ] T035 [US3] Add all quality checks to `.github/workflows/release.yml`
- [ ] T036 [US3] Add build artifact generation to `.github/workflows/release.yml`
- [ ] T037 [US3] Configure concurrency groups with cancel-in-progress for dev and PR workflows
- [ ] T038 [US3] Test development workflow by pushing to feature branch
- [ ] T039 [US3] Test PR validation workflow by creating test pull request

**Validation Steps**:
```bash
# Test development workflow
git checkout -b test-ci-workflow
echo "# Test" >> README.md
git add README.md
git commit -m "test: verify CI workflow"
git push origin test-ci-workflow

# Monitor workflow on GitHub Actions tab
# Verify: norminette, build, test all execute

# Test PR workflow
gh pr create --title "Test PR Validation" --body "Testing workflows"
# Verify: All dev checks + memory leak detection execute
```

**Success Criteria**:
- Development workflow runs on push to dev branches (norminette + build + test)
- PR validation runs all dev checks plus memory leak detection
- Release workflow runs all checks plus artifact generation
- All workflows complete within 30 minutes for clean code
- Clear pass/fail indicators in GitHub UI
- Concurrency control prevents duplicate runs

**Checkpoint**: All user stories should now be independently functional

---

## Phase 6: User Story 4 - Automated Wiki Updates (Priority: P4)

**Goal**: Implement automatic GitHub Wiki synchronization triggered by version tags, with retry logic for failures, non-blocking failure handling, and automatic issue creation on persistent failures

**Independent Test**: Create version tag and verify Wiki updates with documentation from docs/ directory, including version information, without blocking release if Wiki fails

**Estimated Time**: 2-3 hours

### Implementation for User Story 4

- [ ] T040 [US4] Implement wiki repository clone logic in `.github/scripts/sync-wiki.sh`
- [ ] T041 [US4] Add Korean documentation file discovery in `.github/scripts/sync-wiki.sh`
- [ ] T042 [US4] Implement markdown conversion with version metadata in `.github/scripts/sync-wiki.sh`
- [ ] T043 [US4] Add wiki commit and push logic in `.github/scripts/sync-wiki.sh`
- [ ] T044 [US4] Implement retry logic with exponential backoff (0s, 30s, 90s) in `.github/scripts/sync-wiki.sh`
- [ ] T045 [US4] Add error logging and GitHub issue creation on failure in `.github/scripts/sync-wiki.sh`
- [ ] T046 [US4] Create `.github/workflows/wiki-sync.yml` workflow file
- [ ] T047 [US4] Add version tag triggers to `.github/workflows/wiki-sync.yml`
- [ ] T048 [US4] Add manual workflow_dispatch trigger to `.github/workflows/wiki-sync.yml`
- [ ] T049 [US4] Configure continue-on-error for wiki sync job in `.github/workflows/wiki-sync.yml`
- [ ] T050 [US4] Add wiki sync job to `.github/workflows/release.yml` as non-blocking step
- [ ] T051 [US4] Test wiki sync script locally with dry-run mode
- [ ] T052 [US4] Test wiki sync workflow by creating test tag (if wiki enabled)

**Validation Steps**:
```bash
# Dry run test
export GITHUB_TOKEN="dummy_token"
export GITHUB_REPOSITORY="test/test"
.github/scripts/sync-wiki.sh --dry-run v1.0.0

# Test with actual tag (if wiki enabled)
git tag -a v0.0.1-test -m "Test wiki sync"
git push origin v0.0.1-test
# Monitor wiki-sync workflow on GitHub Actions
# Verify wiki updates or graceful failure
```

**Success Criteria**:
- Wiki automatically updates within 2 minutes of tag creation
- Documentation synced from docs/ directory with version information
- Retry up to 3 times with exponential backoff
- Failure logged but does not block release (100% releases succeed)
- GitHub issue created automatically on persistent wiki sync failure

**Checkpoint**: At this point, all core workflow improvements are functional

---

## Phase 7: User Story 5 - Enhanced Error Testing (Priority: P5)

**Goal**: Enhance test_miniRT.sh with comprehensive error case testing (parsing errors, semantic errors, missing required elements, edge cases) with clear pass/fail reporting and CI-compatible exit codes

**Independent Test**: Run improved test script with various invalid scene files and verify each test reports clear success/failure status with overall test statistics

**Estimated Time**: 2-3 hours

### Implementation for User Story 5

- [ ] T053 [US5] Add test counter variables (TOTAL, PASSED, FAILED) to `test_miniRT.sh`
- [ ] T054 [US5] Create `run_test()` function with expected result parameter in `test_miniRT.sh`
- [ ] T055 [US5] Implement pass/fail indicator logic (✓/✗) in `test_miniRT.sh`
- [ ] T056 [US5] Add test summary output at end of `test_miniRT.sh`
- [ ] T057 [US5] Set exit code based on test results (0 if all pass, 1 otherwise) in `test_miniRT.sh`
- [ ] T058 [US5] Create `scenes/error/` directory for error test cases
- [ ] T059 [P] [US5] Create error test scene for missing ambient light in `scenes/error/no_ambient.rt`
- [ ] T060 [P] [US5] Create error test scene for invalid camera in `scenes/error/bad_camera.rt`
- [ ] T061 [P] [US5] Create error test scene for negative sphere radius in `scenes/error/negative_radius.rt`
- [ ] T062 [P] [US5] Create error test scene for invalid light position in `scenes/error/invalid_light.rt`
- [ ] T063 [P] [US5] Create error test scene for malformed syntax in `scenes/error/syntax_error.rt`
- [ ] T064 [US5] Add test cases to `test_miniRT.sh` for all error scenarios (15+ distinct cases)
- [ ] T065 [US5] Add timeout handling (5 seconds per test) in `test_miniRT.sh`
- [ ] T066 [US5] Test script locally with valid and invalid scenes
- [ ] T067 [US5] Verify script works in CI environment with proper exit codes

**Validation Steps**:
```bash
# Run enhanced test script
./test_miniRT.sh

# Expected output format:
# miniRT Test Suite
# =================
# ✓ PASS: Valid sphere scene
# ✓ PASS: Missing ambient light (error expected)
# ✗ FAIL: Invalid test case
#
# Test Summary: 14/15 passed

# Check exit code
echo $?  # Should be 0 if all tests pass, 1 otherwise
```

**Success Criteria**:
- Test script covers 15+ distinct error scenarios
- Clear pass/fail indicators for each test case
- Summary statistics (X/Y passed) at end
- Non-zero exit code on any failure (CI integration)
- Executes successfully in CI environment
- Each test completes within 5 seconds

**Checkpoint**: All user stories are complete and independently testable

---

## Phase 8: Polish & Cross-Cutting Concerns

**Purpose**: Integration, documentation, and final validation

**Estimated Time**: 1-2 hours

- [ ] T068 [P] Update `README.md` with workflow information and badge links
- [ ] T069 [P] Update `docs/02-개발/` with new CI workflow documentation
- [ ] T070 [P] Add quickstart instructions to `docs/01-시작하기/` for new developers
- [ ] T071 Verify all workflows work together in complete end-to-end test
- [ ] T072 Create example pull request demonstrating all workflow stages
- [ ] T073 Test complete release process (tag → workflows → artifacts → wiki)
- [ ] T074 Document troubleshooting steps in `docs/02-개발/troubleshooting.md`
- [ ] T075 [P] Add workflow architecture diagram to `docs/06-참고자료/`
- [ ] T076 Run complete validation per `quickstart.md`
- [ ] T077 Clean up any temporary test files or branches

**Final Validation Checklist**:
```bash
# 1. Project structure clean
ls -la | grep -E '\.(md|txt)$' | wc -l  # <10 files

# 2. All scripts executable
ls -l .github/scripts/*.sh  # All should have +x

# 3. Git history preserved
git log --follow docs/project/IMPLEMENTATION_PLAN.txt  # Shows full history

# 4. Workflows configured
ls .github/workflows/*.yml  # ci.yml, pr-validation.yml, release.yml, wiki-sync.yml

# 5. Memory leak detection works
.github/scripts/check-memory-leaks.sh scenes/test/sphere.rt

# 6. Test script enhanced
./test_miniRT.sh | grep "Test Summary"

# 7. All quality checks pass
norminette src includes && make re && ./test_miniRT.sh
```

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately (~15-30 min)
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories (~1-2 hours)
- **User Stories (Phase 3-7)**: All depend on Foundational phase completion
  - Can proceed in parallel if team has capacity
  - Or sequentially in priority order: US1 → US2 → US3 → US4 → US5
- **Polish (Phase 8)**: Depends on all user stories being complete (~1-2 hours)

### User Story Dependencies

- **US1 (Project Structure)**: Can start after Foundational - No dependencies on other stories (~30-45 min)
- **US2 (Memory Leak Detection)**: Can start after Foundational - Independent of US1 (~2-3 hours)
- **US3 (Quality Check Workflows)**: Can start after Foundational - May reference US2 scripts but independently testable (~2-3 hours)
- **US4 (Wiki Sync)**: Can start after Foundational - Independent of other stories (~2-3 hours)
- **US5 (Error Testing)**: Can start after Foundational - Independent of other stories (~2-3 hours)

### Within Each User Story

- US1: Sequential file moves, each verified before next
- US2: Scripts in parallel, then integration testing
- US3: Workflows can be updated in parallel, then tested sequentially
- US4: Script implementation then workflow integration
- US5: Test cases can be created in parallel, script updated sequentially

### Parallel Opportunities

**Phase 1 (Setup)**: T002, T003, T004 can run in parallel (different directories)

**Phase 2 (Foundational)**: T006, T007 can run in parallel (different scripts)

**Phase 5 (US3)**: Multiple workflow files can be edited in parallel

**Phase 7 (US5)**: T059-T063 can run in parallel (different test scene files)

**Phase 8 (Polish)**: T068, T069, T070, T075 can run in parallel (different documentation files)

**Between User Stories**: After Phase 2 completes, all user stories (US1-US5) can be worked on in parallel by different team members

---

## Parallel Example: User Story 5 (Error Testing)

```bash
# Launch all error scene creation in parallel:
# Terminal 1
cat > scenes/error/no_ambient.rt << EOF
C 0,0,0 0,0,1 90
L -40,50,0 0.7 255,255,255
sp 0,0,20 20 255,0,0
EOF

# Terminal 2
cat > scenes/error/bad_camera.rt << EOF
A 0.2 255,255,255
C invalid_position 0,0,1 90
L -40,50,0 0.7 255,255,255
EOF

# Terminal 3
cat > scenes/error/negative_radius.rt << EOF
A 0.2 255,255,255
C 0,0,0 0,0,1 90
sp 0,0,20 -20 255,0,0
EOF

# All can be created simultaneously
```

---

## Implementation Strategy

### MVP First (User Story 1 Only) - Recommended Start

1. Complete Phase 1: Setup (~15-30 min)
2. Complete Phase 2: Foundational (~1-2 hours)
3. Complete Phase 3: User Story 1 (~30-45 min)
4. **STOP and VALIDATE**: Verify project structure is clean and organized
5. **Total MVP Time**: ~2.5-4 hours

**MVP Deliverable**: Clean, organized project structure following constitution standards

### Incremental Delivery (Recommended Full Implementation)

1. **Foundation** (Phases 1-2): Setup + Scripts → ~2-3 hours
2. **US1** (Phase 3): Project structure → Test independently → ~30-45 min
3. **US2** (Phase 4): Memory leak detection → Test independently → ~2-3 hours
4. **US3** (Phase 5): CI workflows → Test independently → ~2-3 hours
5. **US4** (Phase 6): Wiki sync → Test independently → ~2-3 hours
6. **US5** (Phase 7): Error testing → Test independently → ~2-3 hours
7. **Polish** (Phase 8): Integration & documentation → ~1-2 hours

**Total Time (Sequential)**: ~12-18 hours

### Parallel Team Strategy

With 3 developers after Foundational phase completion:

1. **Team completes Phases 1-2 together**: ~2-3 hours
2. **Developer A**: US1 (Project Structure) → ~30-45 min
3. **Developer B**: US2 (Memory Leak) + US3 (Workflows) → ~4-6 hours
4. **Developer C**: US4 (Wiki) + US5 (Testing) → ~4-6 hours
5. **Team completes Phase 8 together**: ~1-2 hours

**Total Time (Parallel)**: ~8-12 hours

---

## Estimated Time Summary

| Phase | Description | Sequential Time | Parallel Time |
|-------|-------------|-----------------|---------------|
| Phase 1 | Setup | 15-30 min | 15-30 min |
| Phase 2 | Foundational | 1-2 hours | 1-2 hours |
| Phase 3 | US1: Project Structure | 30-45 min | 30-45 min |
| Phase 4 | US2: Memory Leak Detection | 2-3 hours | 2-3 hours |
| Phase 5 | US3: Quality Workflows | 2-3 hours | 2-3 hours |
| Phase 6 | US4: Wiki Sync | 2-3 hours | 2-3 hours |
| Phase 7 | US5: Error Testing | 2-3 hours | 2-3 hours |
| Phase 8 | Polish & Integration | 1-2 hours | 1-2 hours |
| **Total** | **Complete Implementation** | **12-18 hours** | **8-12 hours** |

**MVP Only (US1)**: 2.5-4 hours
**MVP + Critical Features (US1-US3)**: 6-10 hours

---

## Notes

- **[P] tasks**: Different files, can run in parallel
- **[Story] label**: Maps task to specific user story for traceability
- **Time estimates**: Include implementation, local testing, and basic debugging
- **Success criteria**: Measurable outcomes for each user story
- **Independent testing**: Each story can be validated without others
- **Commit strategy**: Commit after completing each user story phase
- **Rollback safety**: Each user story completion is a safe rollback point

---

## Success Validation

After completing all tasks, verify:

1. ✅ **US1**: Project structure clean, <10 files in root, git history preserved
2. ✅ **US2**: Memory leak detection works on both Linux and macOS, CI fails on leaks
3. ✅ **US3**: All three workflows (dev, PR, release) execute correctly with clear results
4. ✅ **US4**: Wiki sync works or fails gracefully without blocking releases
5. ✅ **US5**: Test script covers 15+ error cases with clear pass/fail indicators
6. ✅ **Integration**: Complete end-to-end flow from commit → PR → release works smoothly
7. ✅ **Documentation**: All changes documented in README and Korean docs
8. ✅ **Constitution**: All principles (I-V) satisfied per plan.md verification

---

**Generated**: 2025-12-18  
**Branch**: `001-workflow-improvements`  
**Ready for Implementation**: YES ✅
