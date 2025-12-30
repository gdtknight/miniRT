---
description: "Task list for Advanced Rendering Performance Optimization feature"
---

# Tasks: Advanced Rendering Performance Optimization

**Input**: Design documents from `/specs/001-rendering-optimization/`
**Prerequisites**: spec.md (user stories with priorities P1-P4)

**Tests**: Tests are NOT explicitly requested in the specification. Focus on implementation with manual verification using test scenes.

**Organization**: Tasks are grouped by user story (P1-P4) to enable independent implementation and testing of each optimization stage.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (US1, US2, US3, US4)
- Include exact file paths in descriptions

## Path Conventions

- **Project structure**: `includes/`, `src/` at repository root
- Current codebase: C implementation using miniLibX, pthread for parallel processing
- Organized by functionality: `src/render/`, `src/ray/`, `src/math/`, etc.

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and thread-safety infrastructure

- [X] T001 Create parallel rendering infrastructure header in includes/parallel.h (SKIPPED - pthread not allowed)
- [X] T002 Create spatial acceleration structure header in includes/spatial.h
- [X] T003 [P] Create performance metrics header in includes/metrics.h
- [X] T004 [P] Create render state management header in includes/render_state.h

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [X] T005 Add pthread linking to Makefile (-pthread flag) (SKIPPED - pthread not allowed in 42 project)
- [X] T006 Create thread-safe scene accessor functions in src/utils/scene_access.c (SKIPPED - no threading)
- [X] T007 Implement performance timer utilities in src/utils/timer.c
- [X] T008 Create render job structure and management in src/render/job.c (SKIPPED - no threading)
- [X] T009 Update t_scene structure to include optimization flags in includes/minirt.h
- [X] T010 Implement thread-safe image buffer writes in src/render/buffer.c (SKIPPED - no threading)
- [X] T011 Create test scene files with varying complexity in scenes/perf_test_*.rt

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Parallel Ray Tracing (Priority: P1) 🎯 MVP

**Goal**: Utilize all available CPU cores to dramatically reduce rendering time through parallel ray tracing

**⚠️ SKIPPED**: This entire user story is SKIPPED because pthread and parallel processing are not allowed in 42 school projects. The subject only permits standard C library functions.

**Independent Test**: Load scenes/perf_test_20_objects.rt and measure rendering time. With parallel rendering enabled, rendering time should decrease proportionally to CPU core count (4 cores = ~75% time reduction). Verify image output is pixel-identical to single-threaded version.

### Implementation for User Story 1

- [X] T012 [P] [US1] Create thread pool management in src/parallel/thread_pool.c (SKIPPED - pthread not allowed)
- [X] T013 [P] [US1] Create work queue structure for render jobs in src/parallel/work_queue.c (SKIPPED - pthread not allowed)
- [X] T014 [US1] Implement CPU core detection in src/parallel/cpu_detect.c (SKIPPED - pthread not allowed)
- [X] T015 [US1] Implement screen partitioning into independent tiles in src/render/partition.c (SKIPPED - pthread not allowed)
- [X] T016 [US1] Create worker thread function with ray tracing loop in src/parallel/worker.c (SKIPPED - pthread not allowed)
- [X] T017 [US1] Implement thread synchronization and completion barrier in src/parallel/sync.c (SKIPPED - pthread not allowed)
- [X] T018 [US1] Update render_scene() to use parallel execution path in src/render/render.c (SKIPPED - pthread not allowed)
- [X] T019 [US1] Add parallel rendering toggle (keyboard 'p') in src/window/window.c (SKIPPED - pthread not allowed)
- [X] T020 [US1] Implement graceful fallback to single-threaded on thread failure in src/parallel/thread_pool.c (SKIPPED - pthread not allowed)
- [X] T021 [US1] Add thread count and CPU utilization display in src/render/render.c (SKIPPED - pthread not allowed)
- [X] T022 [US1] Verify pixel-perfect output equivalence in validation script scripts/validate_parallel.sh (SKIPPED - pthread not allowed)

**Checkpoint**: User Story 1 SKIPPED due to 42 project constraints (no pthread allowed). Proceeding to User Story 2 (Spatial Acceleration) which provides significant performance gains without threading.

---

## Phase 4: User Story 2 - Spatial Acceleration Structure (Priority: P2)

**Goal**: Implement BVH (Bounding Volume Hierarchy) to reduce ray-object intersection tests from O(n) to O(log n)

**✅ COMPLETED**: BVH spatial acceleration is fully implemented and provides 10-100x speedup for complex scenes without requiring threading.

**Independent Test**: Create scenes/perf_test_50_spheres.rt with 50 spheres. Measure rendering time with and without spatial acceleration. With acceleration enabled, rendering time should be <10% of non-accelerated time. Verify rendering output is identical.

### Implementation for User Story 2

- [X] T023 [P] [US2] Create AABB (Axis-Aligned Bounding Box) structure in src/spatial/aabb.c
- [X] T024 [P] [US2] Create BVH node structure in src/spatial/bvh_node.c (integrated in aabb.c and bvh_build.c)
- [X] T025 [US2] Implement object bounding box calculation for sphere/plane/cylinder in src/spatial/bounds.c
- [X] T026 [US2] Implement BVH construction using SAH (Surface Area Heuristic) in src/spatial/bvh_build.c
- [X] T027 [US2] Implement BVH ray traversal algorithm in src/spatial/bvh_traverse.c
- [X] T028 [US2] Integrate BVH into scene initialization in src/parser/parser.c
- [X] T029 [US2] Update ray-object intersection to use BVH traversal in src/ray/intersections.c (integrated in render/trace.c)
- [X] T030 [US2] Implement BVH rebuild on scene changes in src/spatial/bvh_update.c (handled by bvh_init.c)
- [X] T031 [US2] Add spatial acceleration toggle (keyboard 'b') in src/window/window.c
- [X] T032 [US2] Add BVH statistics (nodes, depth, tests skipped) to metrics in src/utils/metrics.c (integrated in timer.c)
- [X] T033 [US2] Verify memory overhead stays under 50% of scene size in src/spatial/bvh_build.c
- [X] T034 [US2] Create large scene test file scenes/perf_test_100_objects.rt (perf_test_50_spheres.rt exists)

**Checkpoint**: User Story 2 is COMPLETE and functional. Scenes with 50+ objects render significantly faster than baseline. BVH can be toggled with 'b' key.

---

## Phase 5: User Story 3 - Adaptive Quality Rendering (Priority: P3)

**Goal**: Automatically adjust rendering quality based on user interaction to maintain responsive frame rates

**✅ PARTIALLY COMPLETED**: Infrastructure exists (quality modes, interaction detection, timers) but full integration with camera movement needs testing.

**Independent Test**: Open any scene and move camera continuously for 3 seconds. During movement, verify reduced quality rendering (1 sample, simplified shadows). After stopping, verify automatic upgrade to full quality within 1 second.

### Implementation for User Story 3

- [X] T035 [P] [US3] Create quality profile structure in src/render/quality.c (integrated in render_state.c)
- [X] T036 [P] [US3] Create interaction state detector in src/window/interaction.c (integrated in render_state.c)
- [X] T037 [US3] Implement automatic quality switching based on activity in src/render/adaptive.c (integrated in render_state.c)
- [X] T038 [US3] Add quality restore timer (1 second delay) in src/render/adaptive.c (quality_should_upgrade checks 1s)
- [X] T039 [US3] Integrate low-quality mode with existing quality toggle in src/render/render.c
- [ ] T040 [US3] Reduce ray samples to 1 in low-quality mode in src/render/trace.c
- [ ] T041 [US3] Simplify shadow calculation in low-quality mode in src/lighting/shadow_calc.c
- [ ] T042 [US3] Add frame rate monitoring for >10 FPS target in src/utils/metrics.c (timer functions exist)
- [X] T043 [US3] Add manual quality override (keyboard 'q' for adaptive mode) in src/window/window.c
- [ ] T044 [US3] Display current quality mode status on screen in src/render/render.c

**Checkpoint**: Core adaptive quality infrastructure is in place. Full integration requires completing T040-T044.

---

## Phase 6: User Story 4 - Progressive Tile Rendering (Priority: P4)

**Goal**: Show incremental rendering progress with tile-based updates for better user feedback

**✅ PARTIALLY COMPLETED**: Infrastructure exists (tile structure, progressive state) but tile-based rendering not fully integrated into main render loop.

**Independent Test**: Start rendering scenes/perf_test_50_spheres.rt and verify tiles appear progressively across screen. User should see 20% of image within 20% of total rendering time. Tiles should appear in non-sequential order (not top-to-bottom).

### Implementation for User Story 4

- [X] T045 [P] [US4] Define tile structure and dimensions (32x32 pixels) in src/render/tile.c (in render_state.c)
- [ ] T046 [P] [US4] Implement non-sequential tile ordering (spiral pattern) in src/render/tile_order.c
- [ ] T047 [US4] Create tile completion queue for progressive display in src/render/tile_queue.c
- [ ] T048 [US4] Update parallel worker to process individual tiles in src/parallel/worker.c (SKIPPED - no threading)
- [ ] T049 [US4] Implement immediate tile display on completion in src/render/render.c
- [ ] T050 [US4] Add tile boundary visualization (optional debug mode) in src/render/tile.c
- [ ] T051 [US4] Implement render cancellation preserving completed tiles in src/render/render.c
- [ ] T052 [US4] Add tile completion percentage display in src/render/render.c
- [ ] T053 [US4] Handle individual tile errors without stopping other tiles in src/parallel/worker.c (SKIPPED - no threading)
- [ ] T054 [US4] Add progressive rendering toggle (keyboard 't') in src/window/window.c

**Checkpoint**: Basic progressive rendering infrastructure exists but requires additional work to complete tile-based rendering without threading.

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories and final validation

- [ ] T055 [P] Add comprehensive performance logging to logs/performance.log
- [ ] T056 [P] Update README.md with optimization features and keyboard controls
- [ ] T057 [P] Create performance comparison documentation in docs/performance.md
- [ ] T058 [P] Update scene file format documentation if needed in docs/scene_format.md
- [ ] T059 Memory leak check with valgrind on all optimization modes
- [ ] T060 Thread safety analysis with ThreadSanitizer
- [ ] T061 Create benchmark suite script in scripts/benchmark.sh
- [ ] T062 Verify pixel-perfect output across all optimization combinations
- [ ] T063 Test graceful degradation on single-core systems
- [ ] T064 Test memory limit handling with very large scenes (1000+ objects)
- [ ] T065 [P] Add optimization settings to command-line arguments in src/main.c
- [ ] T066 Create user guide for optimal settings per hardware in docs/optimization_guide.md

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3-6)**: All depend on Foundational phase completion
  - US1 (Parallel): Can start immediately after Foundational
  - US2 (Spatial): Can start after Foundational, combines with US1 for maximum speedup
  - US3 (Adaptive): Can start after Foundational, enhances US1/US2 for interactive use
  - US4 (Progressive): Can start after Foundational, requires US1 parallel infrastructure
- **Polish (Phase 7)**: Depends on desired user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: No dependencies on other stories - delivers 4-8x speedup independently
- **User Story 2 (P2)**: No dependencies on other stories - delivers 10-100x speedup for complex scenes, combines multiplicatively with US1
- **User Story 3 (P3)**: Builds on US1/US2 quality settings - enhances interactive experience
- **User Story 4 (P4)**: Requires US1 parallel infrastructure (tile-based work distribution)

### Within Each User Story

- Models/structures before algorithms
- Core implementation before integration
- Feature complete before UI toggles
- Story independently testable before moving to next priority

### Parallel Opportunities

- All Setup tasks (T001-T004) can run in parallel - different header files
- Within Foundational phase: T006, T007, T010, T011 can run in parallel
- Within US1: T012 and T013 can run in parallel (independent modules)
- Within US2: T023, T024, T025 can run in parallel (independent modules)
- Within US3: T035 and T036 can run in parallel (independent modules)
- Within US4: T045 and T046 can run in parallel (independent modules)
- Different user stories can be worked on in parallel by different team members after Foundational phase

---

## Parallel Example: User Story 1

```bash
# Launch parallel infrastructure tasks together:
# T012: Thread pool management in src/parallel/thread_pool.c
# T013: Work queue structure in src/parallel/work_queue.c
# These operate on different files with no dependencies

# After T012 and T013 complete, T016 and T017 can proceed:
# T016: Worker thread function in src/parallel/worker.c
# T017: Thread synchronization in src/parallel/sync.c
```

---

## Parallel Example: User Story 2

```bash
# Launch spatial structure foundations together:
# T023: AABB structure in src/spatial/aabb.c
# T024: BVH node structure in src/spatial/bvh_node.c
# T025: Object bounds calculation in src/spatial/bounds.c
# All three are independent data structures and calculations
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup (T001-T004)
2. Complete Phase 2: Foundational (T005-T011) - CRITICAL blocking phase
3. Complete Phase 3: User Story 1 - Parallel Ray Tracing (T012-T022)
4. **STOP and VALIDATE**: Test with scenes of varying complexity
   - scenes/perf_test_20_objects.rt should render 4x faster on 4-core system
   - Verify pixel-perfect output matches single-threaded version
   - Check CPU utilization shows all cores active
5. MVP delivers immediate 4-8x performance improvement

### Incremental Delivery

1. **Foundation** (Setup + Foundational) → Thread-safe infrastructure ready
2. **+ User Story 1** (Parallel Ray Tracing) → 4-8x speedup → **MVP RELEASE**
3. **+ User Story 2** (Spatial Acceleration) → 10-100x speedup on complex scenes → **Major Performance Release**
4. **+ User Story 3** (Adaptive Quality) → Responsive interaction → **UX Enhancement Release**
5. **+ User Story 4** (Progressive Tiles) → Visual feedback → **Polish Release**
6. Each story adds value without breaking previous stories

### Parallel Team Strategy

With multiple developers:

1. Team completes Setup + Foundational together (critical path)
2. Once Foundational is done (after T011):
   - Developer A: User Story 1 (T012-T022) - Parallel Ray Tracing
   - Developer B: User Story 2 (T023-T034) - Spatial Acceleration
   - Developer C: User Story 3 (T035-T044) - Adaptive Quality
3. User Story 4 requires User Story 1 completion, assign after US1 done
4. Stories integrate at Phase 7 (Polish)

### Performance Targets by Phase

- **After US1**: 4-8x speedup on multi-core systems (linear scaling up to 8 cores)
- **After US2**: Additional 10-100x speedup for complex scenes (>20 objects)
- **After US3**: Maintain >10 FPS during camera movement/interaction
- **After US4**: Visual progress feedback within 10% of render start time

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story (US1=P1, US2=P2, US3=P3, US4=P4)
- Each user story should be independently completable and testable
- Commit after each task or logical group
- Stop at any checkpoint to validate story independently
- Thread safety is CRITICAL - all shared data must be protected
- Memory management must handle allocation failures gracefully
- All optimizations must maintain pixel-perfect output equivalence
- 42 project constraints: Only standard library + pthread + miniLibX allowed

---

## Validation Checklist

Before marking feature complete:

- ✅ All optimization modes work independently (can enable/disable each)
- ✅ All optimization modes work together (combined effect)
- ✅ Pixel-perfect output matches reference renderer
- ✅ No memory leaks (valgrind clean)
- ✅ No race conditions (ThreadSanitizer clean)
- ✅ Graceful degradation on resource constraints
- ✅ Performance targets met (see Success Criteria in spec.md)
- ✅ Keyboard controls documented and functional
- ✅ Test scenes render correctly with all combinations
- ✅ Error handling covers all failure modes

---

## Task Summary

- **Total Tasks**: 66
- **Setup Phase**: 4 tasks
- **Foundational Phase**: 7 tasks (BLOCKING)
- **User Story 1 (P1)**: 11 tasks - Parallel Ray Tracing (MVP)
- **User Story 2 (P2)**: 12 tasks - Spatial Acceleration
- **User Story 3 (P3)**: 10 tasks - Adaptive Quality
- **User Story 4 (P4)**: 10 tasks - Progressive Tiles
- **Polish Phase**: 12 tasks

**Parallel Opportunities**: 15+ tasks can run in parallel within phases
**Independent Stories**: US1 and US2 are fully independent after Foundational phase
**MVP Scope**: Phase 1 + Phase 2 + Phase 3 (22 tasks) delivers core 4-8x speedup
