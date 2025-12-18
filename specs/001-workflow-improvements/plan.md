# Implementation Plan: Workflow Improvements and Project Structure Reorganization

**Branch**: `001-workflow-improvements` | **Date**: 2025-12-18 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/001-workflow-improvements/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Implement comprehensive workflow improvements for the miniRT project including:
1. Project structure reorganization using git mv to preserve file history
2. Automated memory leak detection in CI with valgrind (Linux) and leaks (macOS)
3. Enhanced CI/CD workflows with concurrency control and error handling
4. Automated GitHub Wiki synchronization with retry logic and failure handling
5. Improved test script with comprehensive error case coverage

Technical approach: Bash scripting for automation, GitHub Actions for CI/CD, valgrind/leaks for memory analysis, git mv for file reorganization with history preservation.

## Technical Context

**Language/Version**: Bash 4.0+, C (C99 standard for miniRT), GitHub Actions YAML  
**Primary Dependencies**: 
- GitHub Actions (CI/CD platform)
- valgrind 3.15+ (Linux memory leak detection)
- leaks command (macOS built-in memory leak detection)
- git 2.20+ (file history preservation with git mv)
- MinilibX (existing graphics library)

**Storage**: File system for logs (logs/), build artifacts (build/), git for version control  
**Testing**: 
- test_miniRT.sh (bash test script)
- Scene files in scenes/ directory
- valgrind/leaks for memory leak testing
- norminette for code style validation

**Target Platform**: Linux (Ubuntu latest) and macOS (latest) in GitHub Actions, local development on both platforms  
**Project Type**: Single C project with CLI tool (miniRT ray tracer)  
**Performance Goals**: 
- Memory leak checks complete within 10 minutes per test scene
- CI workflow completes within 30 minutes total
- Wiki sync completes within 2 minutes

**Constraints**: 
- Zero memory leaks tolerated (valgrind must report 0 leaks)
- 100% norminette compliance required
- Wiki update failures must not block releases
- File history must be preserved during reorganization

**Scale/Scope**: 
- ~50 C source files
- ~15 test scenes in scenes/
- 3 CI workflows (development, PR validation, release)
- ~10 documentation files to reorganize

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### ✅ Principle I: Project Structure Standards (NON-NEGOTIABLE)
**Status**: PASS - Feature explicitly reorganizes project structure per constitution
- CI scripts will be moved to `.github/scripts/`
- User tools (create_wiki.sh, test_miniRT.sh) remain at root
- Documentation organized in docs/wiki/, docs/archive/, docs/project/
- Backup files (README_OLD.md, README.md.backup) moved to docs/archive/

### ✅ Principle II: Code Quality Automation (NON-NEGOTIABLE)
**Status**: PASS - Feature enhances automated quality gates
- Adds automated memory leak detection with valgrind/leaks
- Maintains existing norminette checks
- Preserves build validation
- All quality gates automated in CI with zero manual intervention

### ✅ Principle III: Documentation and Wiki Synchronization (NON-NEGOTIABLE)
**Status**: PASS - Feature implements wiki automation requirement
- Automated Wiki update triggered by version tags
- Source docs in docs/ remain single source of truth
- Wiki failures logged but don't block releases
- Version information included in Wiki pages

### ✅ Principle IV: Workflow System (NON-NEGOTIABLE)
**Status**: PASS - Feature implements all workflow stages
- Development workflow: norminette + build + test
- PR workflow: development checks + memory leak detection
- Release workflow: all PR checks + Wiki update + artifacts
- Concurrency control prevents workflow conflicts

### ✅ Principle V: Tools and Environment Standards (NON-NEGOTIABLE)
**Status**: PASS - Feature maintains platform support
- Linux and macOS both supported
- valgrind (Linux) and leaks (macOS) for memory detection
- Automatic tool installation where possible
- Clear error messages for missing dependencies

**OVERALL ASSESSMENT**: ✅ ALL GATES PASSED - Feature aligns perfectly with constitution principles and directly implements multiple constitutional requirements.

## Project Structure

### Documentation (this feature)

```text
specs/001-workflow-improvements/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
│   ├── workflows.md     # CI/CD workflow contracts
│   ├── memory-check.md  # Memory leak detection contract
│   └── wiki-sync.md     # Wiki synchronization contract
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

**Current structure** (to be reorganized):
```text
/
├── create_wiki.sh           # User tool - KEEP at root
├── test_miniRT.sh          # User tool - KEEP at root
├── IMPLEMENTATION_PLAN.txt # Move to docs/project/
├── README_OLD.md           # Move to docs/archive/
├── WIKI_READY.md           # Move to docs/wiki/
├── WIKI_SETUP.md           # Move to docs/wiki/
└── docs/                   # Korean documentation
    ├── 00-전체설명/
    ├── 01-시작하기/
    ├── 02-개발/
    ├── 03-기능/
    ├── 04-릴리스/
    ├── 05-프로젝트_히스토리/
    └── 06-참고자료/
```

**Target structure** (after reorganization):
```text
/
├── create_wiki.sh          # User tool (unchanged)
├── test_miniRT.sh          # User tool (enhanced)
├── README.md               # Main documentation (unchanged)
├── Makefile                # Build system (unchanged)
│
├── .github/
│   ├── workflows/          # CI/CD workflows
│   │   ├── ci.yml         # Enhanced with memory checks
│   │   ├── pr-validation.yml  # Enhanced with valgrind
│   │   ├── release.yml    # Enhanced with wiki sync
│   │   └── wiki-sync.yml  # NEW: Automated wiki update
│   └── scripts/           # NEW: CI automation scripts
│       ├── check-memory-leaks.sh  # NEW: Memory leak wrapper
│       ├── install-valgrind.sh    # NEW: Auto-install valgrind
│       └── sync-wiki.sh           # NEW: Wiki synchronization
│
├── docs/                   # Korean documentation
│   ├── 00-전체설명/       # (unchanged)
│   ├── 01-시작하기/       # (unchanged)
│   ├── 02-개발/           # (unchanged)
│   ├── 03-기능/           # (unchanged)
│   ├── 04-릴리스/         # (unchanged)
│   ├── 05-프로젝트_히스토리/  # (unchanged)
│   ├── 06-참고자료/       # (unchanged)
│   ├── wiki/              # NEW: Wiki-specific docs
│   │   ├── WIKI_READY.md  # Moved from root
│   │   └── WIKI_SETUP.md  # Moved from root
│   ├── archive/           # NEW: Obsolete/backup files
│   │   └── README_OLD.md  # Moved from root
│   └── project/           # NEW: Implementation planning
│       └── IMPLEMENTATION_PLAN.txt  # Moved from root
│
├── src/                    # Source code (unchanged)
├── includes/               # Headers (unchanged)
├── scenes/                 # Test scenes (unchanged)
├── tests/                  # Test infrastructure (unchanged)
├── build/                  # Build artifacts (unchanged)
└── logs/                   # Log files (unchanged)
```

**Structure Decision**: Single C project with enhanced automation infrastructure. The reorganization follows constitution Principle I by separating CI scripts (.github/scripts/), user tools (root), and documentation (docs/ hierarchy). All file moves will use `git mv` to preserve history.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

No violations detected. This feature directly implements constitutional requirements with no complexity concerns.

---

## Phase 0: Research - ✅ COMPLETE

**Output**: `research.md`

**Resolved Technical Decisions:**
1. Git History Preservation: Use `git mv` command
2. Linux Memory Detection: valgrind with auto-install
3. macOS Memory Detection: Built-in leaks command
4. Concurrency Control: GitHub Actions concurrency groups
5. Wiki Synchronization: Separate workflow with retry logic
6. Test Enhancement: Bash script with structured output
7. Log Management: Console summary + full artifacts
8. Tool Installation: Platform-detection scripts
9. Error Handling: Explicit checks with conditional continuation

All "NEEDS CLARIFICATION" items from Technical Context have been resolved with rationale, alternatives considered, and implementation details.

---

## Phase 1: Design & Contracts - ✅ COMPLETE

### Artifacts Generated:

**1. data-model.md** - Defines all entities and relationships:
- WorkflowRun, QualityCheck, MemoryLeakReport
- WikiSyncOperation, WikiPage
- FileReorganization, ReorganizationPlan
- TestResult, TestSuite, Artifact
- Complete entity relationship diagram
- State transitions for all entities

**2. contracts/workflows.md** - GitHub Actions workflow specifications:
- Development CI (ci.yml)
- PR Validation (pr-validation.yml)
- Release (release.yml)
- Wiki Sync (wiki-sync.yml)
- Complete job definitions, triggers, concurrency rules

**3. contracts/memory-check.md** - Memory leak detection contract:
- Script interface and usage
- Platform-specific behavior (valgrind/leaks)
- Output specifications
- CI integration patterns
- Error handling and testing

**4. contracts/wiki-sync.md** - Wiki synchronization contract:
- Script interface and prerequisites
- Synchronization process (5 steps)
- Retry logic with exponential backoff
- Failure handling (blocking vs non-blocking)
- GitHub issue creation on failure

**5. quickstart.md** - Developer onboarding guide:
- Prerequisites and installation
- Step-by-step implementation walkthrough
- Common tasks and troubleshooting
- Reference commands and workflow triggers

### Agent Context Update:

**Updated**: `.github/agents/copilot-instructions.md`
- Added Bash 4.0+, GitHub Actions YAML to languages
- Added file system storage for logs and artifacts
- Preserved existing manual additions

---

## Phase 2: Implementation Tasks - PENDING

**Status**: Ready to proceed  
**Command**: `/speckit.tasks` or `speckit.tasks` script

Phase 2 will break down implementation into concrete tasks:
1. File reorganization tasks (git mv operations)
2. CI script creation tasks
3. Workflow enhancement tasks
4. Test script improvement tasks
5. Documentation update tasks

---

## Constitution Re-check (Post-Design)

### ✅ All Principles Still Satisfied

**Principle I (Project Structure)**: Design explicitly follows structure standards  
**Principle II (Code Quality)**: All automated gates maintained and enhanced  
**Principle III (Wiki Sync)**: Automated sync designed per requirements  
**Principle IV (Workflow System)**: All three workflow stages designed  
**Principle V (Tools/Environment)**: Cross-platform support maintained

**No design conflicts or compromises introduced.**

---

## Summary

### What Was Delivered

**Planning Artifacts:**
- ✅ Implementation plan with technical context
- ✅ Constitution compliance verification (all gates passed)
- ✅ Project structure before/after diagrams

**Phase 0 - Research:**
- ✅ 9 technical decisions resolved
- ✅ All alternatives evaluated with rationale
- ✅ Implementation patterns established

**Phase 1 - Design:**
- ✅ Complete data model (10 entities)
- ✅ 4 detailed contracts (workflows, memory-check, wiki-sync)
- ✅ Developer quickstart guide
- ✅ Agent context updated

**Total Documentation**: 70+ KB of specifications

### Key Technical Decisions

| Decision Area | Choice | Rationale |
|--------------|--------|-----------|
| File History | git mv | Native Git support, automatic tracking |
| Linux Memory | valgrind | Standard tool, detailed reports |
| macOS Memory | leaks | Pre-installed, no setup needed |
| Concurrency | GitHub Actions groups | Native feature, prevents waste |
| Wiki Sync | Retry + non-blocking | Resilient, doesn't block releases |
| Test Script | Enhanced Bash | Simple, portable, CI-friendly |

### Ready for Implementation

All design work complete. Next steps:
1. Run `/speckit.tasks` to generate implementation tasks
2. Begin Phase 2 implementation following task breakdown
3. Test each component as implemented
4. Integrate and verify complete system

### Estimated Implementation Effort

- File reorganization: 1-2 hours
- CI scripts: 4-6 hours
- Workflow updates: 2-3 hours
- Test enhancements: 2-3 hours
- Testing and verification: 3-4 hours
- **Total: 12-18 hours**

---

**Plan Status**: ✅ COMPLETE (Phase 0 & 1)  
**Next Command**: `speckit.tasks specs/001-workflow-improvements`  
**Branch**: `001-workflow-improvements`  
**Implementation Ready**: YES
