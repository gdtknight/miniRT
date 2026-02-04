# Tasks: Dead Code Removal & Legacy Cleanup

**Input**: Design documents from `/specs/025-dead-code-removal/`
**Prerequisites**: plan.md, spec.md, data-model.md, contracts/deletion-manifest.md, research.md, quickstart.md

**Tests**: Not included (manual build + norminette + scene regression per spec)

**Organization**: Tasks follow spec phases (FR-001→FR-007). Each phase is independently verifiable via `make re`.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Maps to plan.md user stories (US1-US16)
- Exact file paths included in all descriptions

---

## Phase 1: Immediate Dead Code Deletion (FR-001, FR-002, FR-003, FR-004)

**Goal**: Delete all files, types, and declarations with zero external references. No pre-work needed.

**Independent Test**: `make re` succeeds with zero warnings after all Phase 1 tasks complete.

### File Deletions

- [x] T001 [P] [US1] Delete 4 unused header files: `includes/overlay.h`, `includes/format_object_id.h`, `includes/render_quality.h`, `includes/render_progressive.h`
- [x] T002 [P] [US2] Delete `includes/render_state.h` entirely (all types and declarations unused per FR-002, research R1)
- [x] T003 [P] [US2] Delete 5 orphaned source files: `src/spatial/aabb_shapes.c`, `src/render/render_state.c`, `src/render/render_quality.c`, `src/render/render_progressive.c`, `src/utils/format_object_id.c`

### Header Partial Cleanup

- [x] T004 [P] [US3] Remove legacy `t_sphere`, `t_plane`, `t_cylinder` typedefs from `includes/objects.h` (lines 74-104, including comments)
- [x] T005 [P] [US4] Remove dead `parse_vector`, `parse_color` declarations (with doxygen comments) from `includes/parser.h` (lines 189-203)
- [x] T006 [P] [US4] Remove 7 unimplemented selection helper declarations + 1 duplicate `render_scene_to_buffer` declaration from `includes/window_internal.h` (lines 129-139)
- [x] T007 [P] [US4] Remove `keyguide_render_background` declaration from `includes/keyguide.h`

### Source Partial Cleanup

- [x] T008 [P] [US5] Delete `keyguide_render_background` function (including docstring) from `src/keyguide/keyguide_render.c` (lines 25-52)

### Makefile & Build Verification

- [x] T009 [US6] Remove 5 deleted .c entries from Makefile SRCS: `aabb_shapes.c` (line ~105), `render_state.c` (line ~98), `render_quality.c` (line ~99), `render_progressive.c` (line ~100), `format_object_id.c` (line ~116)
- [x] T010 [US6] Phase 1 build verification: `make re` succeeds + `norminette` passes on modified files

**Checkpoint**: 10 files deleted, 4 headers cleaned, 1 function removed. `make re` succeeds.

---

## Phase 2: Dependency Separation (FR-005)

**Goal**: Move active `in_range` function to its new home, then delete emptied files and orphaned dependencies.

**Independent Test**: `make re` succeeds. `in_range` still callable from existing parser code.

**Depends on**: Phase 1 complete

### Function Migration

- [x] T011 [US7] Move `in_range()` function (4 lines) from `src/parser/parse_validation.c:26-29` to top of `src/parser/parse_validation_strict.c` (after includes). No header change needed (`parser.h:212` already declares it).

### File Deletions

- [x] T012 [US8] Delete `src/parser/parse_validation.c` and remove its entry from Makefile SRCS (line ~77)
- [x] T013 [P] [US9] Delete `src/utils/ft_atof.c`, remove `ft_atof` declaration from `includes/minirt.h` (line ~127), and remove Makefile SRCS entry (line ~117)

### Build Verification

- [x] T014 Phase 2 build verification: `make re` succeeds + `norminette` passes on `src/parser/parse_validation_strict.c` and `includes/minirt.h`

**Checkpoint**: 2 more files deleted, `in_range` safely relocated. `make re` succeeds.

---

## Phase 3: Wrapper & Error Path Substitution (FR-006, FR-007)

**Goal**: Replace legacy wrapper calls with direct calls, then delete emptied wrapper code.

**Independent Test**: `make re` succeeds. Error output identical for all 18 invalid scene files.

**Depends on**: Phase 1 complete (Phase 2 not required)

### cleanup_all Replacement (FR-006)

- [x] T015 [P] [US10] In `src/window/window_lifecycle.c`, replace `cleanup_all(render->scene, render)` with: save `render->scene` to local `t_scene *scene` variable, call `render_destroy(render)`, then call `scene_destroy(scene)`. Critical: must save scene pointer before render_destroy frees render.

- [x] T016 [US11] Delete `src/utils/cleanup.c`, remove `cleanup_all` declaration from `includes/minirt.h` (lines ~123-124), and remove Makefile SRCS entry (line ~114). Depends on T015.

### print_error Replacement (FR-007)

- [x] T017 [P] [US12] In `src/parser/parser.c`, replace all 7 `print_error()` calls with `error_print()` using comma operator pattern `(error_print(CODE), 0)` to preserve return value 0. Mapping: validate_scene 4 calls → `ERR_PARSE_MISSING`; parse_scene → `ERR_FILE_EXT`, `ERR_FILE_OPEN`, `ERR_MALLOC`.

- [x] T018 [US13] Delete `print_error` function (lines ~83-100, including docstring) from `src/utils/error.c` and remove `print_error` declaration from `includes/error.h` (line ~43). Depends on T017.

### Build Verification

- [x] T019 Phase 3 build verification: `make re` succeeds + `norminette` passes on modified files (`window_lifecycle.c`, `parser.c`, `error.c`, `minirt.h`, `error.h`)

**Checkpoint**: 1 more file deleted, 2 legacy APIs eliminated. `make re` succeeds.

---

## Phase 4: Final Verification

**Goal**: Full regression verification across build, style, and runtime behavior.

**Depends on**: Phases 1, 2, 3 all complete

- [x] T020 [US14] Full clean build: `make re` succeeds with zero warnings
- [x] T021 [P] [US14] Style check: `norminette src/ includes/` passes with no errors
- [x] T022 [US15] Valid scene rendering: build succeeds (graphical verification requires manual test)
- [x] T023 [US16] Invalid scene regression: 17/18 produce error output; 1 (invalid_fov_out_of_range.rt, FOV=180) passes parsing — pre-existing behavior, not a regression

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1** (Immediate Deletions): No dependencies - start immediately
- **Phase 2** (Dependency Separation): Depends on Phase 1 completion
- **Phase 3** (Wrapper Substitution): Depends on Phase 1 completion (independent of Phase 2)
- **Phase 4** (Final Verification): Depends on Phases 1, 2, 3 all complete

### Within-Phase Dependencies

**Phase 1**: T001-T008 all parallelizable [P]. T009 (Makefile) after T001-T003. T010 (build) last.

**Phase 2**: T011 (move in_range) → T012 (delete parse_validation.c). T013 parallelizable with T012 [P]. T014 (build) last.

**Phase 3**: T015 → T016 (cleanup_all pair). T017 → T018 (print_error pair). The two pairs are independent and parallelizable. T019 (build) last.

**Phase 4**: T020, T021 parallelizable [P]. T022, T023 after build confirmed.

### Cross-Phase Parallelism

Phase 2 and Phase 3 can execute in parallel after Phase 1 completes, with one constraint: both T013 (Phase 2) and T016 (Phase 3) modify `includes/minirt.h` — if run in parallel, coordinate edits to different sections of the same file.

### Parallel Opportunities

```
Phase 1: T001 ─┐
         T002 ─┤
         T003 ─┤ (all parallel)
         T004 ─┤
         T005 ─┤
         T006 ─┤
         T007 ─┤
         T008 ─┘→ T009 → T010

Phase 2: T011 → T012 ─┐→ T014
                T013 ─┘

Phase 3: T015 → T016 ─┐→ T019
         T017 → T018 ─┘

Phase 4: T020 ─┐→ T022 → T023
         T021 ─┘
```

---

## Implementation Strategy

### MVP First (Phase 1 Only)

1. Complete Phase 1: Remove all zero-reference dead code
2. **STOP and VALIDATE**: `make re` + `norminette`
3. This alone removes 10 files and cleans 4 headers — largest impact

### Incremental Delivery

1. Phase 1 → 10 files deleted, 4 headers cleaned → `make re` ✓
2. Phase 2 → 2 more files deleted, `in_range` relocated → `make re` ✓
3. Phase 3 → 1 more file deleted, 2 legacy APIs removed → `make re` ✓
4. Phase 4 → Full regression verified → ready to merge

### Totals

- **Files deleted**: 13 (5 headers + 8 sources)
- **Declarations/functions removed**: 22
- **Legacy wrappers replaced**: 3 call sites (cleanup_all × 1, print_error × 7 → 8 total edits)
- **Makefile SRCS lines removed**: 8
- **Total tasks**: 23

---

## Notes

- [P] tasks = different files, no dependencies on incomplete tasks
- [US#] maps to plan.md user stories for traceability
- Commit after each phase checkpoint (4 commits total)
- `print_error` returns 0, `error_print` returns 1 — comma operator `(error_print(code), 0)` preserves return semantics
- `render->scene` must be saved to local variable before `render_destroy(render)` — prevents use-after-free
- `in_range` is already declared in `parser.h:212` — no header edit needed for the move
