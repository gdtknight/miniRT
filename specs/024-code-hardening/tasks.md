# Tasks: Code Hardening

**Input**: Design documents from `/specs/024-code-hardening/`
**Prerequisites**: plan.md (required), spec.md (required), research.md, data-model.md, quickstart.md

**Tests**: Not requested in spec. Manual testing via scene file regression.

**Organization**: Tasks grouped by user story (US1-US7) to enable independent implementation and testing.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2)
- Include exact file paths in descriptions

---

## Phase 1: Setup

**Purpose**: No new project initialization needed. Verify branch and build baseline.

- [X] T001 Verify branch 024-code-hardening is based on latest develop with 023-parsing-robustness merged, run `make re` to confirm clean build

---

## Phase 2: Foundational (Header & Data Model Updates)

**Purpose**: Struct and enum changes that MUST be complete before user story implementation

**Warning**: No user story work can begin until this phase is complete

- [X] T002 [P] Add `io_error` field to `t_line_reader` in `includes/parser.h` (after `line_too_long` field) and add `PARSE_ERR_IO` to `t_parse_result` enum (before `PARSE_ERR_COUNT`)
- [X] T003 [P] Add `width` and `height` fields to `t_progressive_state` in `includes/render_state.h` (after `tile_size` field)
- [X] T004 [P] Add `width` and `height` fields to `t_mlx_img` in `includes/mlx_context.h` (after `endian` field)
- [X] T005 [P] Add I/O error message `"I/O error while reading file"` to `msgs[]` array in `src/parser/parse_error_msg.c` (at index matching `PARSE_ERR_IO`)

**Checkpoint**: Headers and enums updated. All files should compile (`make re`).

---

## Phase 3: User Story 1 - Accurate BVH Rendering (Priority: P1)

**Goal**: BVH acceleration produces deterministic, correct intersections by initializing `temp_hit.distance`.

**Independent Test**: Render a 10+ object scene multiple times; verify identical output each time.

### Implementation for User Story 1

- [X] T006 [US1] Initialize `temp_hit.distance = hit->distance` before the loop in `bvh_leaf_intersect()` in `src/spatial/bvh_traverse.c` (insert after line 57, before `hit_anything = 0`)

**Checkpoint**: BVH rendering is deterministic. Test with `scenes/valid/*.rt`.

---

## Phase 4: User Story 2 - Axis-Aligned View Rendering (Priority: P1)

**Goal**: AABB slab test handles zero-direction ray components without div-by-zero or NaN.

**Independent Test**: Render scenes with camera direction (0,0,1), (1,0,0), (0,0,-1) and verify no artifacts.

### Implementation for User Story 2

- [X] T007 [US2] Add `#include <math.h>` to `src/spatial/aabb.c` for `fabs()` usage
- [X] T008 [US2] Extract a `static void safe_slab_axis(double origin, double dir, double box_min, double box_max, double *tmin, double *tmax)` helper in `src/spatial/aabb.c` that handles `fabs(dir) < 1e-8` case (origin outside slab: set `*tmin = 1e30, *tmax = -1e30` and return; origin inside: leave tmin/tmax unchanged and return; otherwise compute `inv_d = 1.0 / dir` and call `update_bounds`)
- [X] T009 [US2] Refactor `aabb_intersect()` in `src/spatial/aabb.c` to call `safe_slab_axis()` for each axis (x, y, z) instead of direct division

**Checkpoint**: Axis-aligned camera views render correctly. No NaN/INF artifacts.

---

## Phase 5: User Story 3 - Vertical Camera View (Priority: P2)

**Goal**: Camera direction parallel to world up (0,1,0) or (0,-1,0) renders correctly via alternative up vector.

**Independent Test**: Render with camera direction (0,1,0) and (0,-1,0); verify non-black output.

### Implementation for User Story 3

- [X] T010 [US3] In `init_camera_calc()` in `src/render/camera.c`, add gimbal lock detection: compute `fabs(vec3_dot(camera->direction, world_up))`, if > 0.999 switch `world_up` to `(t_vec3){0, 0, 1}` before computing `calc->right`

**Checkpoint**: Vertical camera views produce correct top-down/bottom-up renders.

---

## Phase 6: User Story 4 - Window Size Tile Rendering (Priority: P2)

**Goal**: Progressive tile calculation uses actual window dimensions instead of hardcoded 800.

**Independent Test**: Build with WINDOW_WIDTH != 800 (e.g., 1024, 640); verify all tiles cover the full window.

### Implementation for User Story 4

- [X] T011 [US4] Store `width` and `height` in `progressive_init()` in `src/render/render_progressive.c` (set `prog->width = width; prog->height = height`)
- [X] T012 [US4] Replace hardcoded `800` with `prog->width` in `progressive_next_tile()` in `src/render/render_progressive.c` (line 60: `tiles_x = (prog->width + prog->tile_size - 1) / prog->tile_size`)
- [X] T013 [US4] Also replace `WINDOW_WIDTH`/`WINDOW_HEIGHT` references with `prog->width`/`prog->height` in tile clamping (lines 67-70) in `src/render/render_progressive.c`

**Checkpoint**: Progressive rendering covers entire window at any resolution.

---

## Phase 7: User Story 5 - Correct Shadow Rendering (Priority: P2)

**Goal**: Shadow bias uses actual surface normal instead of hardcoded (0,1,0).

**Independent Test**: Render vertical wall (normal = (1,0,0)) and floor (normal = (0,1,0)); verify no shadow acne or peter-panning.

### Implementation for User Story 5

- [X] T014 [US5] Add `t_vec3 surface_normal` parameter to `calc_shadow_samples()` signature in `src/lighting/shadow_calc.c` and replace `normal = (t_vec3){0.0, 1.0, 0.0}` with the parameter
- [X] T015 [US5] Add `t_vec3 surface_normal` parameter to `calculate_shadow_factor()` signature in `src/lighting/shadow_calc.c` and pass it to `calc_shadow_samples()`
- [X] T016 [US5] Update `calculate_shadow_factor()` declaration in `includes/shadow.h` to include `t_vec3 surface_normal` parameter
- [X] T017 [US5] Update caller in `calc_lighting_factor()` in `src/lighting/lighting.c` to pass `hit->normal` as the `surface_normal` argument to `calculate_shadow_factor()`

**Checkpoint**: Shadows are surface-normal-aware. No acne on vertical walls.

---

## Phase 8: User Story 6 - I/O Error Reporting (Priority: P3)

**Goal**: `read()` errors are distinguished from EOF and reported to the user.

**Independent Test**: Verify code path: `read()` returning -1 sets `io_error = 1`, parsing loop detects and reports `PARSE_ERR_IO`.

### Implementation for User Story 6

- [X] T018 [US6] Initialize `reader->io_error = 0` in `line_reader_init()` in `src/parser/parser_utils.c` (after `reader->line_too_long = 0`)
- [X] T019 [US6] In `refill_buffer()` in `src/parser/parse_line_reader.c`, detect `reader->buf_len < 0` after `read()`, set `reader->io_error = 1` and `reader->buf_len = 0`
- [X] T020 [US6] In `process_lines()` in `src/parser/parser.c`, after the while loop, check `reader->io_error` and if set, report `PARSE_ERR_IO` via error context and return 0

**Checkpoint**: I/O errors produce clear "I/O error while reading file" message instead of silent success.

---

## Phase 9: User Story 7 - Defensive Input Handling (Priority: P3)

**Goal**: Integer overflow, capacity overflow, and pixel out-of-bounds are safely handled.

**Independent Test**: Parse file with `99999999999` integer; verify overflow error. Verify pixel bounds checks work.

### Implementation for User Story 7

- [X] T021 [P] [US7] Add overflow check in `parse_int_digits()` in `src/parser/parse_number.c`: add `#include <limits.h>`, change return type from `void` to `int`, before `*result = *result * 10 + digit` check `if (*result > (INT_MAX - digit) / 10) return (0)`, return `1` on success
- [X] T022 [P] [US7] Update `parse_int()` in `src/parser/parse_number.c` to check return value of `parse_int_digits()` and return `PARSE_ERR_NUMBER_FORMAT` on failure
- [X] T023 [P] [US7] Add `#include <limits.h>` and capacity overflow check `if (list->capacity > INT_MAX / 2) return (0)` before `new_capacity = list->capacity * 2` in `object_list_grow()` in `src/scene/object_list.c`
- [X] T024 [P] [US7] Add `width` and `height` fields storage in `mlx_img_init()` in `src/window/mlx_context.c` (set `img->width = width; img->height = height` after `mlx_get_data_addr`)
- [X] T025 [P] [US7] Add pixel bounds check `if (x < 0 || x >= img->width || y < 0 || y >= img->height) return;` in `mlx_img_put_pixel()` in `src/window/mlx_pixel.c` (before byte offset calculation)
- [X] T026 [P] [US7] Add pixel bounds check `if (x < 0 || x >= img->width || y < 0 || y >= img->height) return (0);` in `mlx_img_get_pixel()` in `src/window/mlx_pixel.c` (before byte offset calculation)

**Checkpoint**: Overflow and bounds errors handled gracefully. No crashes on malformed input.

---

## Phase 10: Polish & Cross-Cutting Concerns

**Purpose**: Build verification and regression testing

- [X] T027 Run `make re` and verify clean compilation with no warnings
- [X] T028 Run `norminette src/ includes/` and fix any Norm v4.1 violations introduced by changes
- [X] T029 Regression test: render all `scenes/valid/*.rt` files and verify no visual regressions
- [X] T030 Edge case test: render axis-aligned camera scenes per quickstart.md verification steps

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - verify baseline
- **Foundational (Phase 2)**: Depends on Phase 1 - BLOCKS all user stories
- **US1 (Phase 3)**: Depends on Phase 2 - independent of other stories
- **US2 (Phase 4)**: Depends on Phase 2 - independent of other stories
- **US3 (Phase 5)**: Depends on Phase 2 - independent of other stories
- **US4 (Phase 6)**: Depends on Phase 2 (T003) - independent of other stories
- **US5 (Phase 7)**: Depends on Phase 2 - independent of other stories
- **US6 (Phase 8)**: Depends on Phase 2 (T002, T005) - independent of other stories
- **US7 (Phase 9)**: Depends on Phase 2 (T004) - independent of other stories
- **Polish (Phase 10)**: Depends on all user stories being complete

### User Story Dependencies

- **US1 (P1)**: Independent - `bvh_traverse.c` only
- **US2 (P1)**: Independent - `aabb.c` only
- **US3 (P2)**: Independent - `camera.c` only
- **US4 (P2)**: Independent - `render_progressive.c` + `render_state.h`
- **US5 (P2)**: Independent - `shadow_calc.c` + `shadow.h` + `lighting.c`
- **US6 (P3)**: Independent - `parse_line_reader.c` + `parser.c` + `parser_utils.c`
- **US7 (P3)**: Independent - `parse_number.c` + `object_list.c` + `mlx_pixel.c` + `mlx_context.c`

### Parallel Opportunities

After Phase 2 completion, ALL user stories (US1-US7) can proceed in parallel since they modify different files with no cross-dependencies.

Within US7 (Phase 9), tasks T021-T026 are all marked [P] as they touch different files.

---

## Parallel Example: After Phase 2

```
# All of these can run simultaneously:
T006 [US1] - bvh_traverse.c
T007-T009 [US2] - aabb.c
T010 [US3] - camera.c
T011-T013 [US4] - render_progressive.c
T014-T017 [US5] - shadow_calc.c + lighting.c
T018-T020 [US6] - parse_line_reader.c + parser.c + parser_utils.c
T021-T026 [US7] - parse_number.c + object_list.c + mlx_pixel.c + mlx_context.c
```

---

## Implementation Strategy

### MVP First (US1 + US2 = P1 stories)

1. Complete Phase 1: Setup verification
2. Complete Phase 2: Header/enum updates
3. Complete Phase 3: BVH distance fix (US1)
4. Complete Phase 4: AABB div-by-zero fix (US2)
5. **STOP and VALIDATE**: Both P1 rendering correctness bugs fixed

### Incremental Delivery

1. Phase 1-2: Foundation ready
2. Phase 3-4: P1 bugs fixed (rendering correctness) - highest impact
3. Phase 5-7: P2 bugs fixed (specific conditions) - medium impact
4. Phase 8-9: P3 bugs fixed (defensive coding) - low impact
5. Phase 10: Polish and verify

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- All user stories are independently completable after Phase 2
- No test tasks generated (manual testing per spec)
- Commit after each phase or logical task group
- All changes must pass `norminette` (42 Norm v4.1)
