# Tasks: BVH Performance Verification and Debugging Tools

**Feature Branch**: `008-bvh-performance-diagnostics`  
**Input**: Design documents from `/specs/008-bvh-performance-diagnostics/`  
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

**Tests**: Tests are NOT included in this feature (no tests requested in specification)

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `- [ ] [ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (US1, US2, US3, US4)
- All paths are absolute from repository root: `/Users/yoshin/dev/working/miniRT/`

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and metric infrastructure activation

- [X] T001 Review existing metrics.h structure at includes/metrics.h to understand t_metrics fields
- [X] T002 Verify timer functions (timer_start, timer_elapsed_us, timer_elapsed_ms) are declared in includes/metrics.h
- [X] T003 [P] Verify BVH toggle mechanism ('B' key) is functional in src/window/window.c
- [X] T004 [P] Verify detail toggle mechanism ('I' key) exists in src/window/window.c for show_info flag

**Checkpoint**: ✅ Existing infrastructure validated - ready for metrics implementation

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core metrics collection API that ALL user stories depend on

**⚠️ CRITICAL**: No user story work can begin until metrics collection and timer functions are implemented

- [X] T005 Implement timer_start() in src/render/metrics.c using gettimeofday()
- [X] T006 Implement timer_elapsed_us() in src/render/metrics.c returning microseconds
- [X] T007 [P] Implement timer_elapsed_ms() in src/render/metrics.c returning milliseconds
- [X] T008 Implement metrics_init() in src/render/metrics.c to zero all counters and frame buffer
- [X] T009 Implement metrics_start_frame() in src/render/metrics.c to reset per-frame counters and start timer
- [X] T010 Implement metrics_end_frame() in src/render/metrics.c to calculate frame time and update FPS rolling average
- [X] T011 [P] Implement calculate_fps() helper in src/render/metrics_calc.c using 60-frame rolling average
- [X] T012 [P] Implement calculate_bvh_efficiency() helper in src/render/metrics_calc.c using formula (1 - actual/naive) × 100
- [X] T013 [P] Implement calculate_avg_tests_per_ray() helper in src/render/metrics_calc.c
- [X] T014 Implement inline metrics_add_ray() in src/render/metrics.c (simple counter increment)
- [X] T015 [P] Implement inline metrics_add_intersect_test() in src/render/metrics.c
- [X] T016 [P] Implement inline metrics_add_bvh_node_visit() in src/render/metrics.c
- [X] T017 [P] Implement inline metrics_add_bvh_skip() in src/render/metrics.c
- [X] T018 [P] Implement inline metrics_add_bvh_box_test() in src/render/metrics.c
- [X] T019 Add metrics_init() call to render state initialization in src/render/render_state.c
- [X] T020 Add metrics_start_frame() call at beginning of render loop in src/render/render.c
- [X] T021 Add metrics_end_frame() call at end of render loop in src/render/render.c

**Checkpoint**: ✅ Metrics collection API complete - all user stories can now instrument their features

---

## Phase 3: User Story 1 - Real-time Performance Monitoring (Priority: P1) 🎯 MVP

**Goal**: Developers can see immediate FPS, frame time, and BVH status in HUD to confirm BVH is working

**Independent Test**: Launch miniRT with any scene, observe FPS and frame time in HUD, press 'B' key to toggle BVH and verify metrics update in real-time

### Implementation for User Story 1

- [X] T022 [P] [US1] Implement hud_format_fps() in src/hud/hud_format.c to format double as "XX.X" string
- [X] T023 [P] [US1] Implement hud_format_time_ms() in src/hud/hud_format.c to format microseconds as "XX.Xms"
- [X] T024 [P] [US1] Implement hud_format_bvh_status() in src/hud/hud_format.c to format enabled flag as "ON"/"OFF"
- [X] T025 [US1] Implement render_perf_header() in src/hud/hud_performance.c to display "=== Performance ==="
- [X] T026 [US1] Implement render_perf_basic() in src/hud/hud_performance.c to display FPS, frame time, BVH status
- [X] T027 [US1] Implement hud_render_performance() main function in src/hud/hud_performance.c calling header and basic renderers
- [X] T028 [US1] Add hud_render_performance() declaration to includes/hud.h with documentation
- [X] T029 [US1] Integrate hud_render_performance() call into hud_render_content() in src/hud/hud_render.c after objects section
- [X] T030 [US1] Add metrics_add_ray() call in trace_ray() function in src/render/trace.c for each ray traced
- [X] T031 [US1] Verify HUD displays FPS, frame time, and BVH status when application runs

**Checkpoint**: ✅ User Story 1 complete - Basic performance monitoring working, FPS and frame time visible, BVH toggle shows status change

---

## Phase 4: User Story 2 - BVH Effectiveness Analysis (Priority: P2)

**Goal**: Developers can view detailed BVH statistics (tests, nodes visited/skipped, efficiency) to validate spatial culling

**Independent Test**: Press 'I' key in detailed mode, verify ray-box tests, ray-object tests, nodes visited/skipped, avg tests/ray, and efficiency percentage are displayed

### Implementation for User Story 2

- [ ] T032 [P] [US2] Implement hud_format_counter() in src/hud/hud_format.c to format large numbers with comma separators
- [ ] T033 [P] [US2] Implement hud_format_percent() in src/hud/hud_format.c to format percentage as "XX.X%"
- [ ] T034 [US2] Implement render_bvh_counters() in src/hud/hud_performance.c to display ray-box, ray-obj, nodes visited/skipped
- [ ] T035 [US2] Implement render_bvh_metrics() in src/hud/hud_performance.c to display avg tests/ray and efficiency
- [ ] T036 [US2] Implement render_perf_detailed() in src/hud/hud_performance.c calling counters and metrics renderers
- [ ] T037 [US2] Update hud_render_performance() in src/hud/hud_performance.c to call render_perf_detailed() when show_info flag is enabled
- [ ] T038 [US2] Add metrics_add_bvh_node_visit() call in bvh_node_intersect() in src/spatial/bvh_traverse.c for each node visited
- [ ] T039 [US2] Add metrics_add_bvh_skip() call in bvh_node_intersect() in src/spatial/bvh_traverse.c when AABB test fails
- [ ] T040 [US2] Add metrics_add_bvh_box_test() call in aabb_intersect() in src/spatial/bvh_intersect.c for each ray-box test
- [ ] T041 [US2] Add metrics_add_intersect_test() calls in all object intersection functions (sphere, plane, cylinder, cone) in src/objects/
- [ ] T042 [US2] Verify detailed BVH statistics display when 'I' key is pressed and BVH is enabled

**Checkpoint**: User Story 2 complete - Detailed BVH statistics visible, efficiency calculation working, demonstrates 10x+ test reduction

---

## Phase 5: User Story 3 - BVH Structure Validation (Priority: P3)

**Goal**: Developers can inspect BVH tree quality (nodes, depth, objects/leaf) in HUD and terminal

**Independent Test**: Launch application and check terminal for BVH build statistics, press 'I' key and verify tree stats appear in HUD

### Implementation for User Story 3

- [ ] T043 [P] [US3] Add total_nodes field tracking to t_bvh structure in includes/spatial.h
- [ ] T044 [P] [US3] Add max_depth field tracking to t_bvh structure in includes/spatial.h
- [ ] T045 [P] [US3] Add leaf_count and leaf_object_sum fields to t_bvh structure in includes/spatial.h
- [ ] T046 [P] [US3] Add avg_leaf_objects and build_time_us fields to t_bvh structure in includes/spatial.h
- [ ] T047 [US3] Increment total_nodes counter in bvh_build_recursive() in src/spatial/bvh_init.c for each node created
- [ ] T048 [US3] Update max_depth tracking in bvh_build_recursive() in src/spatial/bvh_init.c during recursion
- [ ] T049 [US3] Accumulate leaf_count and leaf_object_sum in bvh_build_recursive() in src/spatial/bvh_init.c for leaf nodes
- [ ] T050 [US3] Calculate avg_leaf_objects after tree construction in bvh_build() in src/spatial/bvh_init.c
- [ ] T051 [US3] Measure and store build_time_us using timer functions in bvh_build() in src/spatial/bvh_init.c
- [ ] T052 [US3] Output BVH build statistics to terminal using printf() in bvh_build() in src/spatial/bvh_init.c
- [ ] T053 [US3] Implement render_bvh_tree_stats() in src/hud/hud_performance.c to display tree nodes, depth, avg objects/leaf
- [ ] T054 [US3] Integrate render_bvh_tree_stats() call in render_perf_detailed() in src/hud/hud_performance.c
- [ ] T055 [US3] Verify terminal outputs BVH statistics on scene load and tree stats appear in HUD detail mode

**Checkpoint**: User Story 3 complete - BVH tree quality metrics visible in both terminal and HUD, validates tree structure

---

## Phase 6: User Story 4 - Performance Anomaly Detection (Priority: P4)

**Goal**: System warns developers when BVH causes performance degradation instead of improvement

**Independent Test**: Run application with simple scene (3-5 objects) where BVH overhead exceeds benefits, verify terminal displays warning

### Implementation for User Story 4

- [ ] T056 [US4] Implement metrics_log_render() in src/render/metrics.c to output per-frame statistics to terminal (optional debugging)
- [ ] T057 [US4] Implement BVH efficiency detection logic in src/render/metrics.c to compare BVH vs naive rendering
- [ ] T058 [US4] Add warning output to terminal in src/render/metrics.c when BVH efficiency ratio > 1.2x for simple scenes
- [ ] T059 [US4] Integrate efficiency check call in render loop in src/render/render.c after metrics_end_frame()
- [ ] T060 [US4] Test with simple scene (3-5 objects) and verify warning appears when BVH is slower than naive approach

**Checkpoint**: User Story 4 complete - Performance anomaly detection working, warns users about BVH misuse in simple scenes

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: Code quality, optimization, and validation

- [ ] T061 [P] Run norminette on all modified C files to ensure zero violations
- [ ] T062 [P] Run valgrind on application to verify zero memory leaks from metrics collection
- [ ] T063 [P] Verify compilation succeeds on both macOS and Linux platforms
- [ ] T064 Measure metrics collection overhead with test scenes, confirm <5% frame time increase
- [ ] T065 Test simple scene (5 objects): verify BVH overhead warning appears
- [ ] T066 Test complex scene (50+ objects): verify 10x+ speedup with BVH enabled vs disabled
- [ ] T067 [P] Add Doxygen-style comments to all new functions in metrics.c and metrics_calc.c
- [ ] T068 [P] Add Doxygen-style comments to all new functions in hud_performance.c and hud_format.c
- [ ] T069 [P] Add 42 Header format to all new C files (metrics.c, metrics_calc.c, hud_performance.c)
- [ ] T070 Validate quickstart.md instructions by following test scenarios step-by-step
- [ ] T071 Update README.md with performance diagnostics features and key bindings if needed
- [ ] T072 Final integration test: Launch with various scenes, toggle BVH/detail modes, verify all metrics update correctly

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately (infrastructure validation)
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories (metrics API must exist)
- **User Stories (Phase 3-6)**: All depend on Foundational phase completion
  - US1 (P1): Basic performance display - First user story to implement (MVP)
  - US2 (P2): Detailed BVH stats - Depends on US1 display infrastructure
  - US3 (P3): Tree structure stats - Independent of US1/US2, can start after Foundational
  - US4 (P4): Anomaly detection - Depends on metrics collection from US1/US2
- **Polish (Phase 7)**: Depends on all user stories being complete

### User Story Dependencies

- **User Story 1 (P1 - MVP)**: Can start after Foundational (Phase 2) - No dependencies on other stories
  - Delivers: Real-time FPS, frame time, BVH status display
  - MVP stopping point: This is sufficient to validate BVH is working

- **User Story 2 (P2)**: Can start after Foundational (Phase 2) - No dependency on US1 but uses same display infrastructure
  - Delivers: Detailed intersection test statistics and efficiency metrics
  - Extends US1 with detailed mode toggle

- **User Story 3 (P3)**: Can start after Foundational (Phase 2) - Independent of US1/US2
  - Delivers: BVH tree quality metrics (terminal + HUD)
  - Can be implemented in parallel with US2 by different developer

- **User Story 4 (P4)**: Depends on US1 and US2 metrics being collected
  - Delivers: Proactive performance warnings
  - Final enhancement, lowest priority

### Within Each User Story

- **US1**: Format functions → Render functions → Integration → Instrumentation
- **US2**: Format functions → Detailed render functions → BVH instrumentation
- **US3**: Data structure changes → Statistics collection → Display functions
- **US4**: Detection logic → Warning output → Integration

### Parallel Opportunities Within User Stories

**User Story 1 (T022-T031)**:
- Parallel: T022, T023, T024 (all format functions - different files)
- Sequential: T025 → T026 → T027 (render functions in same file)
- Parallel: T028 (header), T030 (trace.c) can happen with T027
- Final: T029, T031 (integration and verification)

**User Story 2 (T032-T042)**:
- Parallel: T032, T033 (format functions - different files)
- Sequential: T034 → T035 → T036 → T037 (render functions in same file)
- Parallel: T038, T039, T040, T041 (all instrumentation - different files)
- Final: T042 (verification)

**User Story 3 (T043-T055)**:
- Parallel: T043, T044, T045, T046 (all header modifications - can be batched)
- Sequential: T047 → T048 → T049 → T050 → T051 → T052 (build instrumentation in same file)
- Parallel: T053 can start while T047-T052 are in progress (different file)
- Final: T054 → T055 (integration and verification)

**User Story 4 (T056-T060)**:
- Sequential: All tasks in same area (efficiency detection)

---

## Parallel Example: User Story 1 (MVP)

```bash
# Step 1: Launch all format functions in parallel (different files)
Task T022: "Implement hud_format_fps() in src/hud/hud_format.c"
Task T023: "Implement hud_format_time_ms() in src/hud/hud_format.c"
Task T024: "Implement hud_format_bvh_status() in src/hud/hud_format.c"

# Step 2: Build performance rendering (sequential, same file)
Task T025: "Implement render_perf_header() in src/hud/hud_performance.c"
Task T026: "Implement render_perf_basic() in src/hud/hud_performance.c"
Task T027: "Implement hud_render_performance() in src/hud/hud_performance.c"

# Step 3: Integration (parallel where possible)
Task T028: "Add declaration to includes/hud.h"
Task T030: "Add metrics_add_ray() to src/render/trace.c"
# (Can happen while T027 is in progress)

# Step 4: Final integration
Task T029: "Integrate into hud_render_content()"
Task T031: "Verify display works"
```

---

## Parallel Example: User Story 2 (Detailed Stats)

```bash
# Step 1: Format functions (parallel)
Task T032: "Implement hud_format_counter() in src/hud/hud_format.c"
Task T033: "Implement hud_format_percent() in src/hud/hud_format.c"

# Step 2: While format functions complete, start instrumentation (parallel, different files)
Task T038: "Add metrics_add_bvh_node_visit() in src/spatial/bvh_traverse.c"
Task T039: "Add metrics_add_bvh_skip() in src/spatial/bvh_traverse.c"
Task T040: "Add metrics_add_bvh_box_test() in src/spatial/bvh_intersect.c"
Task T041: "Add metrics_add_intersect_test() in src/objects/*.c"

# Step 3: Display functions (sequential, same file)
Task T034: "Implement render_bvh_counters()"
Task T035: "Implement render_bvh_metrics()"
Task T036: "Implement render_perf_detailed()"
Task T037: "Update hud_render_performance()"

# Step 4: Verification
Task T042: "Verify detailed stats display"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup (validate existing infrastructure)
2. Complete Phase 2: Foundational (metrics API - CRITICAL)
3. Complete Phase 3: User Story 1 (basic performance display)
4. **STOP and VALIDATE**: 
   - Launch miniRT with test scene
   - Verify FPS and frame time display in HUD
   - Press 'B' key and observe BVH status change
   - Compare FPS with BVH ON vs OFF (should see 10x+ difference on complex scenes)
5. **MVP COMPLETE**: Basic diagnostics working, BVH effectiveness validated

### Incremental Delivery

1. **Foundation** (Setup + Foundational) → Metrics collection API ready
2. **MVP** (+ User Story 1) → Basic performance monitoring working → Can validate BVH works
3. **Enhanced** (+ User Story 2) → Detailed statistics showing test reduction → Can diagnose efficiency
4. **Complete** (+ User Story 3) → Tree quality metrics → Can optimize BVH construction
5. **Polish** (+ User Story 4) → Anomaly warnings → Can avoid BVH misuse

Each story adds diagnostic depth without breaking previous functionality.

### Parallel Team Strategy

With multiple developers (after Foundational phase):

1. **Team completes Phase 1 + Phase 2 together** (foundation must be solid)
2. Once Foundational complete:
   - **Developer A**: User Story 1 (MVP display) - T022-T031
   - **Developer B**: User Story 3 (Tree stats) - T043-T055 (independent)
   - **Developer C**: User Story 2 (Detailed stats) - T032-T042 (after US1 display ready)
3. User Story 4 completes last (needs metrics from US1/US2)

---

## Task Count Summary

- **Phase 1 (Setup)**: 4 tasks
- **Phase 2 (Foundational)**: 17 tasks ⚠️ BLOCKING
- **Phase 3 (US1 - MVP)**: 10 tasks 🎯
- **Phase 4 (US2)**: 11 tasks
- **Phase 5 (US3)**: 13 tasks
- **Phase 6 (US4)**: 5 tasks
- **Phase 7 (Polish)**: 12 tasks

**Total**: 72 tasks

**MVP Scope** (Phases 1-3): 31 tasks delivers basic performance monitoring

**Full Feature** (All phases): 72 tasks delivers complete BVH diagnostics system

---

## Parallel Opportunities Summary

- **Phase 1**: 2 parallel tasks (T003, T004)
- **Phase 2**: 11 parallel tasks (T007, T011-T013, T015-T018)
- **Phase 3 (US1)**: 3 parallel tasks (T022-T024, T028+T030 during T027)
- **Phase 4 (US2)**: 6 parallel tasks (T032-T033, T038-T041)
- **Phase 5 (US3)**: 5 parallel tasks (T043-T046, T053 during T047-T052)
- **Phase 6 (US4)**: 0 parallel tasks (sequential detection logic)
- **Phase 7**: 5 parallel tasks (T061-T063, T067-T069)

**Total parallel opportunities**: 32 tasks can run concurrently with others

---

## Notes

- ✅ All tasks follow checklist format: `- [ ] [ID] [P?] [Story?] Description with file path`
- ✅ Tasks organized by user story for independent implementation
- ✅ Each user story delivers independently testable functionality
- ✅ MVP clearly identified (User Story 1 only)
- ✅ Parallel opportunities marked with [P] flag
- ✅ File paths specified for every implementation task
- ✅ Tests intentionally omitted (not requested in specification)
- ✅ 42 School norminette compliance tasks included in polish phase
- ✅ Performance validation tasks included (<5% overhead, 10x+ speedup)

**Ready for execution**: Tasks can be fed to LLM agents or assigned to developers following dependency order.
