# Norminette Compliance Implementation Plan - COMPLETE

**Feature**: 013-norminette-compliance  
**Objective**: Eliminate all 77 norminette errors to achieve 0 errors  
**Date**: 2026-01-03  
**Status**: ✅ Plan Complete, Ready for Implementation

---

## Executive Summary

This implementation plan provides a comprehensive roadmap to eliminate all 77 norminette errors in the miniRT project through systematic refactoring. The plan follows the `.specify` framework workflow with complete research, design, contracts, and quickstart documentation.

### Current State
- **Norminette Errors**: 77 (across 5 categories)
- **Major Violations**: 53 TOO_MANY_FUNCS (69% of total)
- **Compliance Status**: ❌ Non-compliant

### Target State
- **Norminette Errors**: 0
- **Compliance Status**: ✅ 100% compliant with 42 School standards
- **Estimated Timeline**: 10-12 days

---

## Deliverables Generated

All required Phase 0 and Phase 1 artifacts have been created:

### Phase 0: Research (✅ Complete)
**File**: `specs/013-norminette-compliance/research.md`
- ✅ File splitting strategy (feature concern grouping, max 5 functions)
- ✅ Function extraction techniques (calculation blocks, branches, initialization)
- ✅ Parameter structure design (domain-grouped structures)
- ✅ Validation strategy (multi-layered: image diff + benchmarks + testing)
- ✅ Makefile update strategy (explicit listing by directory)
- ✅ All "NEEDS CLARIFICATION" items resolved

### Phase 1: Design & Contracts (✅ Complete)

#### 1. Data Model
**File**: `specs/013-norminette-compliance/data-model.md`
- ✅ Existing entities documented (Source File, Function, Header, Error)
- ✅ New parameter structures defined (5 new structs for parameter reduction)
- ✅ Current violations cataloged (all 77 errors mapped to fix strategies)
- ✅ Relationships and constraints documented
- ✅ Impact analysis completed (20 → 36 source files estimated)

#### 2. Contracts
**File**: `specs/013-norminette-compliance/contracts/refactoring-rules.md`
- ✅ Contract 1: Function Signature Preservation (public APIs unchanged)
- ✅ Contract 2: Behavior Equivalence (identical output required)
- ✅ Contract 3: Performance Preservation (≤5% regression allowed)
- ✅ Contract 4: Build Compatibility (all targets must build)
- ✅ Contract 5: Code Quality Standards (norminette compliance)
- ✅ Contract 6: Testing & Validation (comprehensive validation workflow)

#### 3. Quickstart Guide
**File**: `specs/013-norminette-compliance/quickstart.md`
- ✅ Prerequisites and setup instructions
- ✅ Phase 0: Baseline establishment (error count, renders, performance)
- ✅ Phase 1: File splitting workflow (step-by-step templates)
- ✅ Phase 2: Function extraction workflow
- ✅ Phase 3: Formatting fixes workflow
- ✅ Phase 4: Parameter/variable reduction workflow
- ✅ Phase 5: Final validation procedures
- ✅ Phase 6: Documentation and merge process
- ✅ Troubleshooting guide
- ✅ Quick reference commands

#### 4. Implementation Plan
**File**: `specs/013-norminette-compliance/plan.md`
- ✅ Technical context documented
- ✅ Constitution check completed (all gates passed)
- ✅ Project structure defined (current + post-refactoring)
- ✅ Complexity tracking (no violations to justify)
- ✅ Phase 0-2 workflow defined
- ✅ Validation & testing strategy
- ✅ Risk mitigation plans
- ✅ Rollout plan (12-day schedule)
- ✅ Success metrics defined

#### 5. Agent Context Update
**Status**: ✅ GitHub Copilot context updated
- ✅ Language: C (C99 standard per 42 School requirements)
- ✅ Framework: MiniLibX (graphics), libft, math library
- ✅ Project type: Single executable (graphics application)

---

## Error Breakdown & Strategy

### Priority 1: File Splitting (TOO_MANY_FUNCS)
**Impact**: 53 errors (69% of total)

| File | Functions | Target Files | Strategy |
|------|-----------|--------------|----------|
| hud_render.c | 13 | 4 files | Split by UI layer (text, metrics, controls) |
| aabb.c | 9 | 2 files | Split by complexity (basic, advanced) |
| metrics.c | 8 | 3 files | Split by concern (tracking, display, fps) |
| window.c | 7 | 3 files | Split by lifecycle (init, events, hooks) |
| render_state.c | 7 | 2 files | Split by operation (state, updates) |
| bvh_build.c | 7 | 2 files | Split by role (core, helpers) |
| timer.c | 6 | 2 files | Split by purpose (control, metrics) |
| render.c | 6 | 2 files | Extract initialization or loop |

### Priority 2: Function Extraction (TOO_MANY_LINES) + Formatting
**Impact**: 10 errors (13% of total)

**Functions >25 lines** (5 errors):
- main() (79 lines) → Extract init and cleanup
- render_loop() → Extract rendering stages
- parse_sphere() → Extract validation logic
- handle_key() → Extract per-key handlers
- render_hud() → Extract HUD sections

**Formatting issues** (5 errors):
- TOO_FEW_TAB in headers (5) → Replace spaces with tabs
- SPC_BEFORE_NL in headers (5) → Remove trailing whitespace

### Priority 3: Parameters & Variables
**Impact**: 7 errors (9% of total)

**TOO_MANY_ARGS** (6 errors):
- traverse_bvh() (6 params) → t_bvh_traversal struct
- build_bvh_node() (5 params) → t_bvh_build_ctx struct
- draw_keyguide() (5 params) → t_hud_render_ctx struct
- Other functions → Similar structure approach

**TOO_MANY_VARS_FUNC** (1 error):
- render_scene() (6 vars) → Group related vars, extract helpers

### Priority 4: Miscellaneous
**Impact**: 2 errors (2% of total)
- TERNARY_FBIDDEN (1) → Convert to if-else
- PREPROC_CONSTANT (1) → Fix constant naming
- MISALIGNED_FUNC_DECL (1) → Align declaration

---

## Key Decisions & Rationale

### 1. File Splitting by Feature Concern
**Why**: Maintains semantic cohesion, clear ownership, minimal dependencies
**Rejected**: Alphabetical splitting (destroys relationships), one-function-per-file (excessive)

### 2. Function Extraction with Clear Purpose
**Why**: Improves readability, testability, enables compiler inlining
**Rejected**: Macro-based (debugging issues), aggressive inlining (premature optimization)

### 3. Parameter Structures by Domain
**Why**: Reduces signature complexity, enables extensibility, maintains type safety
**Rejected**: Global state (testability), variadic functions (unsafe)

### 4. Multi-Layered Validation
**Why**: Comprehensive coverage, fast feedback, objective metrics, high confidence
**Rejected**: Manual testing only (slow, error-prone), no validation (unacceptable risk)

### 5. Explicit Makefile Source Listing
**Why**: Clear visibility, supports partial builds, avoids accidental inclusion
**Rejected**: Wildcard patterns (may include unwanted files)

---

## Validation Strategy

### Baseline Establishment
1. **Norminette Baseline**: 77 errors recorded in `norminette_baseline.txt`
2. **Build Baseline**: Clean build verified with exit code 0
3. **Functional Baseline**: All test scenes rendered, saved to `baseline_renders/`
4. **Performance Baseline**: Complex scene timing recorded (5 iterations)

### Incremental Validation (Per File Split)
1. ✅ Build succeeds (`make clean && make`)
2. ✅ Norminette error count decreases
3. ✅ Simple scene renders correctly
4. ✅ Exit code is 0
5. ✅ Git commit created with clear message

### Comprehensive Validation (Final)
1. ✅ Norminette reports exactly 0 errors
2. ✅ All build targets succeed (make, make bonus, make re)
3. ✅ All test scenes render identically to baseline (image diff)
4. ✅ Performance within ±5% of baseline (benchmark)
5. ✅ Memory leaks: 0 (valgrind check)
6. ✅ Code review: Improved readability

---

## Risk Mitigation

### High Risks
| Risk | Mitigation | Recovery |
|------|------------|----------|
| File splitting breaks compilation | Test build after each split | Git revert to last working state |
| Function extraction changes behavior | Automated image diff after each change | Compare git diff, verify logic |
| Performance regression >5% | Profile before/after, add inline hints | Restructure calls, alternative extraction |

### Medium Risks
| Risk | Mitigation | Recovery |
|------|------------|----------|
| Increased file count complicates navigation | Clear naming, logical structure | Update README with guide |
| Parameter structures add memory overhead | Stack allocation, measure overhead | Use only where necessary |

### Low Risks
| Risk | Mitigation | Recovery |
|------|------------|----------|
| Merge conflicts from formatting | Dedicated formatting commit | Standard git merge resolution |

---

## Implementation Timeline

### Stage 1: Foundation (Days 1-2)
- Establish baselines (norminette, build, performance, functionality)
- Create research.md with refactoring strategies ✅ COMPLETE
- Set up validation automation scripts

### Stage 2: Major File Splits (Days 3-5)
- Split files with 7-13 functions (window, metrics, timer, aabb, bvh_build)
- Create corresponding headers (one per source file)
- Update Makefile incrementally
- Validate after each split

### Stage 3: Function Extraction (Days 6-7)
- Extract long functions (>25 lines) in main.c, render.c, parse_elements.c
- Verify norminette line count compliance
- Validate behavior preservation

### Stage 4: Formatting Fixes (Day 8)
- Fix all header formatting issues (tab/space, trailing whitespace)
- Verify norminette formatting compliance

### Stage 5: Parameter/Variable Reduction (Days 9-10)
- Create parameter structures for functions with >4 parameters
- Refactor functions with >5 local variables
- Update call sites, validate behavior and performance

### Stage 6: Final Validation (Day 11)
- Full norminette check (target: 0 errors)
- Comprehensive testing (all scenes)
- Performance benchmark comparison
- Code review and documentation update

### Stage 7: Integration (Day 12)
- Final testing on clean branch
- Merge to main branch
- CI/CD validation
- Document completion

---

## Success Metrics

| Metric | Baseline | Target | Validation |
|--------|----------|--------|------------|
| Norminette Errors | 77 | 0 | `norminette src/ includes/ \| grep Error \| wc -l` |
| Files >5 functions | 8 | 0 | Manual count from norminette |
| Functions >25 lines | 5 | 0 | Manual count from norminette |
| Build Success | ✅ | ✅ | `make clean && make` exit code |
| Test Scene Pass Rate | 100% | 100% | All scenes render correctly |
| Performance | Baseline | ≤105% | Rendering time comparison |
| Code Readability | N/A | Improved | Code review assessment |

---

## Next Steps

### Immediate Actions
1. ✅ Review this implementation plan
2. ⏭️ Begin Stage 1: Establish baseline (norminette, renders, performance)
3. ⏭️ Begin Stage 2: Start with highest-priority file splits (hud_render.c, aabb.c, metrics.c)

### Phase 2 (Not Yet Started)
- Run `/speckit.tasks` to generate task breakdown after Phase 1 implementation begins
- This will create `specs/013-norminette-compliance/tasks.md` with detailed implementation tasks

### Documentation Access
All planning documents are available in:
- **Plan**: `specs/013-norminette-compliance/plan.md` (27KB, 720 lines)
- **Research**: `specs/013-norminette-compliance/research.md` (24KB)
- **Data Model**: `specs/013-norminette-compliance/data-model.md` (14KB)
- **Contracts**: `specs/013-norminette-compliance/contracts/refactoring-rules.md` (17KB)
- **Quickstart**: `specs/013-norminette-compliance/quickstart.md` (24KB)
- **Spec**: `specs/013-norminette-compliance/spec.md` (16KB)

---

## Completion Checklist

### Phase 0: Research ✅ COMPLETE
- [x] research.md created with refactoring strategies
- [x] File splitting strategy decided
- [x] Function extraction techniques defined
- [x] Parameter structure design patterns established
- [x] Validation strategy comprehensive
- [x] Makefile update strategy defined

### Phase 1: Design & Contracts ✅ COMPLETE
- [x] data-model.md documents existing entities
- [x] New parameter structures defined (5 structs)
- [x] contracts/ defines refactoring rules (6 contracts)
- [x] quickstart.md provides step-by-step workflow
- [x] Agent context updated with new patterns
- [x] Constitution check re-validated (all gates passed)

### Phase 2: Task Breakdown ⏭️ NEXT STEP
- [ ] Run `/speckit.tasks` to generate implementation tasks
- [ ] Review task dependencies and priorities
- [ ] Begin implementation following quickstart guide

---

**Implementation Plan Status**: ✅ COMPLETE
**Ready for**: Implementation Phase (follow quickstart.md)
**Command to proceed**: Begin Stage 1 (baseline establishment) or run `/speckit.tasks` for detailed task breakdown

