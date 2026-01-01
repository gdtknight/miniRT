---

description: "Task list for Code Refactoring for Calculation Optimization"
---

# Tasks: Code Refactoring for Calculation Optimization

**Input**: Design documents from `/specs/011-code-refactoring/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

**Tests**: Tests are NOT explicitly requested in the feature specification. This is a refactoring task focused on performance optimization and code maintainability. Validation will be done through existing test infrastructure and manual scene rendering.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each optimization pattern.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

- **Single project**: `src/`, `includes/`, `tests/`, `scenes/` at repository root
- All paths relative to `/Users/yoshin/dev/working/miniRT/`

---

## Phase 1: Setup (Baseline Validation)

**Purpose**: Establish baseline performance metrics and ensure clean starting state

- [X] T001 Create feature branch 011-code-refactoring and push to remote
- [X] T002 Run full build with make clean && make and document baseline compilation
- [X] T003 Run norminette on all target files and confirm zero violations at baseline
- [ ] T004 Run valgrind on test scenes and confirm zero memory leaks at baseline
- [ ] T005 Render all test scenes in scenes/ directory and save as reference images
- [ ] T006 Measure baseline FPS performance on scenes/benchmark.rt (or similar complex scene)
- [ ] T007 Document baseline metrics: LOC count, build time, FPS, memory usage

---

## Phase 2: Foundational (Structure Updates)

**Purpose**: Core structure changes that MUST be complete before ANY optimization can be implemented

**⚠️ CRITICAL**: No optimization work can begin until this phase is complete

- [ ] T008 Add radius field (double) to t_sphere structure in includes/objects.h
- [ ] T009 Add radius_squared field (double) to t_sphere structure in includes/objects.h
- [ ] T010 Add radius field (double) to t_cylinder structure in includes/objects.h
- [ ] T011 Add radius_squared field (double) to t_cylinder structure in includes/objects.h
- [ ] T012 Add half_height field (double) to t_cylinder structure in includes/objects.h
- [ ] T013 Run make clean && make to verify structure changes compile cleanly
- [ ] T014 Run norminette on includes/objects.h to verify coding standard compliance

**Checkpoint**: Structure definitions complete - optimization implementations can now begin in parallel

---

## Phase 3: User Story 1 - Eliminate Redundant Vector Calculations (Priority: P1) 🎯 MVP

**Goal**: Cache repeated vector dot products in cylinder intersection to eliminate redundant calculations in hot path

**Independent Test**: Run cylinder test scenes and verify identical visual output with improved FPS (5-15% target)

### Implementation for User Story 1

- [ ] T015 [US1] Update parse_sphere() in src/parser/parse_objects.c to compute sphere->radius = diameter / 2.0 after parsing
- [ ] T016 [US1] Update parse_sphere() in src/parser/parse_objects.c to compute sphere->radius_squared = radius * radius
- [ ] T017 [US1] Update parse_cylinder() in src/parser/parse_objects.c to compute cylinder->radius = diameter / 2.0 after parsing
- [ ] T018 [US1] Update parse_cylinder() in src/parser/parse_objects.c to compute cylinder->radius_squared = radius * radius
- [ ] T019 [US1] Update parse_cylinder() in src/parser/parse_objects.c to compute cylinder->half_height = height / 2.0
- [ ] T020 [US1] Update intersect_sphere() in src/ray/intersections.c to use sphere->radius instead of diameter / 2.0
- [ ] T021 [US1] Update intersect_sphere() in src/ray/intersections.c to use sphere->radius_squared in quadratic formula
- [ ] T022 [US1] Add local variable dir_dot_axis in calculate_cylinder_intersection() in src/ray/intersect_cylinder.c
- [ ] T023 [US1] Add local variable oc_dot_axis in calculate_cylinder_intersection() in src/ray/intersect_cylinder.c
- [ ] T024 [US1] Add local variable dir_dot_dir in calculate_cylinder_intersection() in src/ray/intersect_cylinder.c
- [ ] T025 [US1] Add local variable oc_dot_oc in calculate_cylinder_intersection() in src/ray/intersect_cylinder.c
- [ ] T026 [US1] Replace duplicate vec3_dot(ray->direction, cyl->axis) calls with cached dir_dot_axis in src/ray/intersect_cylinder.c
- [ ] T027 [US1] Replace duplicate vec3_dot(oc, cyl->axis) calls with cached oc_dot_axis in src/ray/intersect_cylinder.c
- [ ] T028 [US1] Update cylinder body intersection to use cylinder->radius_squared in src/ray/intersect_cylinder.c
- [ ] T029 [US1] Update cylinder cap intersection to use cylinder->half_height in src/ray/intersect_cylinder.c
- [ ] T030 [US1] Update shadow ray calculations in src/lighting/shadow_calc.c to use cached radius values if applicable

### Validation for User Story 1

- [ ] T031 [US1] Run make clean && make and verify zero compilation errors or warnings
- [ ] T032 [US1] Run norminette on all modified files (src/parser/parse_objects.c, src/ray/intersections.c, src/ray/intersect_cylinder.c, src/lighting/shadow_calc.c)
- [ ] T033 [US1] Render all test scenes and compare pixel-perfect against reference images
- [ ] T034 [US1] Run valgrind on test scenes and verify zero memory leaks
- [ ] T035 [US1] Measure FPS on benchmark scene and verify 5-15% improvement
- [ ] T036 [US1] Git commit with message "US1: Eliminate redundant vector calculations with caching"

**Checkpoint**: At this point, User Story 1 should be fully functional with measurable performance improvement and identical visual output

---

## Phase 4: User Story 2 - Abstract Repetitive Intersection Loop Pattern (Priority: P2)

**Goal**: Create generic intersection iterator to eliminate code duplication across three intersection checking functions

**Independent Test**: Run all test scenes with mixed object types and verify identical rendering with reduced code volume

### Implementation for User Story 2

- [ ] T037 [US2] Add t_intersect_fn typedef in includes/ray.h with signature: int (*)(t_ray *ray, void *object, t_hit *hit)
- [ ] T038 [US2] Implement check_intersections_generic() function in src/render/trace.c per contract specification
- [ ] T039 [US2] Update sphere intersection call site to use check_intersections_generic() with intersect_sphere function pointer
- [ ] T040 [US2] Update plane intersection call site to use check_intersections_generic() with intersect_plane function pointer
- [ ] T041 [US2] Update cylinder intersection call site to use check_intersections_generic() with intersect_cylinder function pointer
- [ ] T042 [US2] Delete deprecated check_sphere_intersections() function from src/render/trace.c
- [ ] T043 [US2] Delete deprecated check_plane_intersections() function from src/render/trace.c
- [ ] T044 [US2] Delete deprecated check_cylinder_intersections() function from src/render/trace.c

### Validation for User Story 2

- [ ] T045 [US2] Run make clean && make and verify zero compilation errors or warnings
- [ ] T046 [US2] Run norminette on modified files (includes/ray.h, src/render/trace.c)
- [ ] T047 [US2] Render all test scenes with mixed objects and verify pixel-perfect output
- [ ] T048 [US2] Run valgrind on test scenes and verify zero memory leaks
- [ ] T049 [US2] Count lines of code removed (target: ~25 lines from trace.c)
- [ ] T050 [US2] Git commit with message "US2: Abstract intersection loop pattern with generic iterator"

**Checkpoint**: At this point, User Stories 1 AND 2 should both work, with reduced code duplication and maintained performance

---

## Phase 5: User Story 3 - Cache Geometry Calculations (Priority: P2)

**Goal**: Validate that precomputed geometry values (from US1) are correctly used throughout codebase, ensuring no redundant calculations remain

**Independent Test**: Search codebase for any remaining diameter/2.0 calculations and verify all use cached values

**Note**: Core implementation already completed in US1 (precomputed values added and computed during parsing). This phase focuses on validation and cleanup.

### Implementation for User Story 3

- [ ] T051 [US3] Search codebase for remaining "diameter / 2.0" or "diameter/2.0" calculations using grep
- [ ] T052 [US3] Search codebase for remaining "height / 2.0" or "height/2.0" calculations using grep
- [ ] T053 [US3] Search codebase for remaining "radius * radius" calculations that should use radius_squared
- [ ] T054 [US3] Update any missed calculations to use precomputed values if found
- [ ] T055 [US3] Add code comments documenting cached values in includes/objects.h structure definitions
- [ ] T056 [US3] Add code comments in parse_objects.c explaining why values are precomputed

### Validation for User Story 3

- [ ] T057 [US3] Run make clean && make and verify clean compilation
- [ ] T058 [US3] Run norminette on modified files
- [ ] T059 [US3] Render all test scenes and verify pixel-perfect output
- [ ] T060 [US3] Run valgrind and verify zero memory leaks
- [ ] T061 [US3] Verify FPS improvement is maintained from US1 baseline
- [ ] T062 [US3] Git commit with message "US3: Validate and document cached geometry calculations"

**Checkpoint**: All geometry caching is validated and documented

---

## Phase 6: User Story 4 - Consolidate Similar Calculation Functions (Priority: P3)

**Goal**: Extract screen-to-NDC coordinate conversion into helper function to eliminate duplication

**Independent Test**: Run rendering with helper function and verify identical pixel output

### Implementation for User Story 4

- [ ] T063 [US4] Add static inline screen_to_ndc() function at top of src/render/render.c after includes
- [ ] T064 [US4] Implement screen_to_ndc() with parameters: int x, int y, double *u, double *v
- [ ] T065 [US4] Replace first occurrence of screen coordinate conversion in src/render/render.c with screen_to_ndc() call
- [ ] T066 [US4] Replace second occurrence of screen coordinate conversion in src/render/render.c with screen_to_ndc() call
- [ ] T067 [US4] Search for any other coordinate conversion patterns using grep and update if found

### Validation for User Story 4

- [ ] T068 [US4] Run make clean && make and verify clean compilation
- [ ] T069 [US4] Run norminette on src/render/render.c
- [ ] T070 [US4] Render test scenes and verify pixel-perfect output
- [ ] T071 [US4] Run valgrind and verify zero memory leaks
- [ ] T072 [US4] Git commit with message "US4: Extract screen-to-NDC conversion into helper function"

**Checkpoint**: All user stories should now be independently functional with consolidated code patterns

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories and final validation

- [ ] T073 [P] Review all code comments and ensure cached calculations are documented
- [ ] T074 [P] Run norminette on all modified files one final time
- [ ] T075 Run full build with make clean && make re
- [ ] T076 Run valgrind on all test scenes in scenes/ directory
- [ ] T077 Render all test scenes and perform final pixel-perfect comparison
- [ ] T078 Measure final FPS on benchmark scene and calculate improvement percentage
- [ ] T079 Count final lines of code and calculate reduction from baseline
- [ ] T080 Calculate code duplication reduction percentage (count duplicate code blocks)
- [ ] T081 Run quickstart.md validation steps per guide
- [ ] T082 Update Korean documentation in docs/specs/011-code-refactoring/ if needed
- [ ] T083 Create pull request with summary of performance improvements and metrics
- [ ] T084 Add PR description with before/after FPS comparison and LOC reduction

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3+)**: All depend on Foundational phase completion
  - **User Story 1 (Phase 3)**: Can start after Foundational - Most critical optimization
  - **User Story 2 (Phase 4)**: Depends on US1 completion for testing but independent implementation
  - **User Story 3 (Phase 5)**: Validates US1 implementation - depends on US1
  - **User Story 4 (Phase 6)**: Independent of US1-3, can run after Foundational
- **Polish (Phase 7)**: Depends on all user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories - **HIGHEST PRIORITY**
- **User Story 2 (P2)**: Can start after Foundational (Phase 2) - Independent but benefits from US1 testing
- **User Story 3 (P2)**: Depends on User Story 1 (validates precomputed values are used correctly)
- **User Story 4 (P3)**: Can start after Foundational (Phase 2) - Independent of US1-3

### Within Each User Story

- User Story 1: Parsing updates BEFORE intersection updates (models before usage)
- User Story 2: Generic function implementation BEFORE call site updates BEFORE old function deletion
- User Story 3: Search tasks BEFORE updates BEFORE documentation
- User Story 4: Function creation BEFORE call site replacements
- Validation tasks MUST come after all implementation tasks within each story

### Parallel Opportunities

- **Phase 1 Setup**: Tasks T002-T007 marked [P] can run in parallel (build, norminette, valgrind, rendering, benchmarks)
- **Phase 2 Foundational**: Tasks T008-T012 can be done together (structure field additions)
- **User Story 1**: Tasks T015-T019 can run in parallel (parsing updates for sphere and cylinder)
- **User Story 1**: Tasks T022-T025 can run in parallel (local variable declarations)
- **User Story 2**: Tasks T039-T041 can run in parallel (update call sites for different object types)
- **User Story 2**: Tasks T042-T044 can run in parallel (delete deprecated functions)
- **User Story 3**: Tasks T051-T053 can run in parallel (grep searches)
- **User Story 4**: Tasks T065-T066 can run in parallel (replace coordinate conversions)
- **Phase 7 Polish**: Tasks T073-T074 can run in parallel (comments and norminette)
- **Note**: Different user stories CANNOT run fully in parallel due to dependencies (US3 needs US1, testing overlaps)

---

## Parallel Example: User Story 1 - Parsing Updates

```bash
# Launch all parsing updates together:
Task T015: "Update parse_sphere() to compute radius"
Task T016: "Update parse_sphere() to compute radius_squared"
Task T017: "Update parse_cylinder() to compute radius"
Task T018: "Update parse_cylinder() to compute radius_squared"
Task T019: "Update parse_cylinder() to compute half_height"

# Launch all local variable declarations together:
Task T022: "Add dir_dot_axis variable"
Task T023: "Add oc_dot_axis variable"
Task T024: "Add dir_dot_dir variable"
Task T025: "Add oc_dot_oc variable"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup (baseline validation)
2. Complete Phase 2: Foundational (structure updates)
3. Complete Phase 3: User Story 1 (vector calculation caching)
4. **STOP and VALIDATE**: Test US1 independently
   - Verify identical visual output
   - Measure FPS improvement (target: 5-15%)
   - Confirm zero memory leaks
5. Deploy/demo if ready (shows immediate performance value)

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready (structure changes in place)
2. Add User Story 1 → Test independently → **First performance win (5-15% FPS boost)**
3. Add User Story 2 → Test independently → **Code maintainability win (30% less duplication)**
4. Add User Story 3 → Test independently → **Validation and documentation complete**
5. Add User Story 4 → Test independently → **Final code consolidation**
6. Each story adds value without breaking previous optimizations

### Sequential Strategy (Recommended for Solo Developer)

Since this is a refactoring task with interdependencies:

1. **Week 1**: Setup + Foundational + User Story 1
   - Most critical performance optimization
   - Measurable FPS improvement
   - Complete validation cycle
2. **Week 2**: User Story 2 + User Story 3
   - Code structure improvements
   - Validation of US1 work
3. **Week 3**: User Story 4 + Polish
   - Final consolidations
   - Complete testing and documentation

---

## Success Criteria Validation

From spec.md, verify these outcomes after implementation:

- **SC-001**: ✅ 10% reduction in redundant calculations
  - **Measure**: Count eliminated vec3_dot() calls in cylinder intersection (3 eliminated per test)
  - **Measure**: Count eliminated division operations (all diameter/2 and height/2 precomputed)
  
- **SC-002**: ✅ 30% reduction in duplicate code patterns
  - **Measure**: Count lines removed from trace.c (~25 lines from 3 functions → 1 generic function)
  - **Target**: ~55% reduction in intersection checking code
  
- **SC-003**: ✅ Identical visual output (pixel-perfect)
  - **Validate**: Compare all test scene renders against reference images
  - **Tool**: Image comparison utility or manual visual inspection
  
- **SC-004**: ✅ 200+ fewer lines of code
  - **Measure**: Run `wc -l` on all modified files before and after
  - **Expected**: ~25 lines from trace.c + improved code structure
  
- **SC-005**: ✅ 5-15% rendering performance improvement
  - **Measure**: FPS comparison on benchmark scene before/after
  - **Document**: Baseline FPS vs. final FPS percentage
  
- **SC-006**: ✅ All existing tests pass
  - **Validate**: All test scenes render without errors
  - **Validate**: Valgrind shows zero leaks
  
- **SC-007**: ✅ Improved complexity metrics
  - **Measure**: Cyclomatic complexity reduction (3 functions → 1 generic)
  - **Measure**: Code duplication percentage reduction

---

## Notes

- **[P] tasks** = different files or independent operations, no dependencies
- **[Story] label** maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Commit after each user story completion (T036, T050, T062, T072)
- Stop at any checkpoint to validate story independently
- **Critical**: Maintain pixel-perfect rendering output throughout all changes
- **Critical**: Zero memory leaks required (42 School constraint)
- **Critical**: Norminette compliance required at all stages
- **No tests written**: This is pure refactoring - validation through existing test infrastructure
- Estimated total time: 6-9 hours for complete implementation and testing

---

## Total Task Count

- **Phase 1 (Setup)**: 7 tasks
- **Phase 2 (Foundational)**: 7 tasks
- **Phase 3 (User Story 1)**: 22 tasks (16 implementation + 6 validation)
- **Phase 4 (User Story 2)**: 14 tasks (8 implementation + 6 validation)
- **Phase 5 (User Story 3)**: 12 tasks (6 implementation + 6 validation)
- **Phase 6 (User Story 4)**: 10 tasks (5 implementation + 5 validation)
- **Phase 7 (Polish)**: 12 tasks

**Total: 84 tasks**

### Task Count by User Story

- **User Story 1** (Eliminate Redundant Calculations): 22 tasks - **Most critical, highest impact**
- **User Story 2** (Abstract Loop Pattern): 14 tasks - **Code quality focus**
- **User Story 3** (Cache Geometry): 12 tasks - **Validation and documentation**
- **User Story 4** (Consolidate Functions): 10 tasks - **Final consolidation**

### Parallel Opportunities Identified

- **5 parallel groups** in Phase 1 (baseline measurements)
- **5 parallel groups** in Phase 2 (structure updates)
- **3 parallel groups** in User Story 1 (parsing, variables, validation)
- **2 parallel groups** in User Story 2 (call sites, deletions)
- **2 parallel groups** in User Story 3 (searches, validations)
- **1 parallel group** in User Story 4 (replacements)
- **2 parallel groups** in Phase 7 (polish tasks)

**Total: ~20 parallel opportunities** for faster execution if team capacity allows

### Independent Test Criteria for Each Story

- **User Story 1**: Render cylinder scenes → FPS improves 5-15% → Output identical
- **User Story 2**: Render mixed object scenes → All objects work → Code reduced 30%
- **User Story 3**: Grep for duplicates → None found → Documentation complete
- **User Story 4**: Render any scene → Coordinates correct → Conversion consolidated

### Suggested MVP Scope

**MVP = User Story 1 ONLY** (Phase 1 + Phase 2 + Phase 3)
- **Deliverable**: Most critical performance optimization (vector calculation caching)
- **Measurable**: 5-15% FPS improvement
- **Timeline**: Can be completed in 1 week
- **Value**: Immediate performance benefit with minimal risk
- **Testing**: Independent validation with existing scenes

After MVP success, incrementally add US2 → US3 → US4 for full feature completion.
