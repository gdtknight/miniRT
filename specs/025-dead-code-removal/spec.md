# Feature Specification: Dead Code Removal & Legacy Cleanup

**Branch**: `025-dead-code-removal`
**Date**: 2026-02-04
**Reference**: `docs/codebase-review-2026-02-04.md`

---

## 1. Overview

Remove all verified dead code, orphaned modules, unused legacy types/functions, and obsolete wrapper layers from the miniRT codebase. This is a pure deletion/cleanup task with no new functionality.

## 2. Goals

- Remove 13 files total (5 headers + 8 sources): 10 immediate (FR-001: 9 + FR-002: render_state.h) + 3 conditional (FR-005: 2 + FR-006: 1)
- Remove 22 individual dead declarations/functions
- Replace 3 legacy wrapper call sites with direct calls
- Eliminate `print_error` legacy API in favor of `error_print` code-based API
- Maintain identical runtime behavior (zero functional change)

## 3. Non-Goals

- Header dependency restructuring (window.h transitive includes)
- Type count duplication consolidation (parse_objects.c / parse_cylinder.c)
- New feature development
- Error message granularity improvement (use existing ERR_PARSE_MISSING)

## 4. Functional Requirements

### FR-001: Remove immediate dead code files (9 files)

Delete files with zero external references (no pre-work needed).
render_state.h is handled separately in FR-002 due to type/declaration cleanup scope.

| File | Category |
|------|----------|
| `includes/overlay.h` | Unused header (0 includes, 8 unimplemented functions) |
| `includes/format_object_id.h` | Unused header |
| `includes/render_quality.h` | Orphaned module header |
| `includes/render_progressive.h` | Orphaned module header |
| `src/spatial/aabb_shapes.c` | 3 functions, 0 callers, no header declarations |
| `src/render/render_state.c` | Orphaned (t_render_state not in t_render) |
| `src/render/render_quality.c` | Orphaned (depends on unused t_render_state) |
| `src/render/render_progressive.c` | 3 functions, 0 callers |
| `src/utils/format_object_id.c` | 0 callers |

Update Makefile SRCS for each deleted .c file (5 lines).

Conditional deletions (3 files, require prior FR completion):

| File | Prerequisite | FR |
|------|-------------|-----|
| `src/parser/parse_validation.c` | in_range 이동 완료 | FR-005 |
| `src/utils/ft_atof.c` | parse_vector 삭제 후 호출자 0건 | FR-005 |
| `src/utils/cleanup.c` | cleanup_all callsite 치환 완료 | FR-006 |

These are deleted in their respective FR phases, with Makefile SRCS updates (3 lines).

### FR-002: Remove render_state.h dead content

Delete from `includes/render_state.h`:
- All function declarations (lines 65-78): `render_state_init`, `render_state_update`, `quality_set_mode`, `quality_should_upgrade`, `quality_handle_interaction`, `progressive_init`, `progressive_next_tile`, `progressive_reset`
- All type definitions: `t_quality_mode`, `t_interaction_state`, `t_progressive_state`, `t_tile_rect`, `t_render_state`
- Entire file becomes empty after cleanup → delete file entirely

### FR-003: Remove partial dead code from headers

- `includes/objects.h`: Delete `t_sphere`, `t_plane`, `t_cylinder` legacy typedefs (lines 74-104)
- `includes/parser.h`: Delete `parse_vector`, `parse_color` declarations (lines 189-203)
- `includes/window_internal.h`: Delete 7 unimplemented selection helper declarations + 1 duplicate `render_scene_to_buffer` declaration (lines 129-139)
- `includes/minirt.h`: Delete `cleanup_all` declaration (line 123-124) and `ft_atof` declaration (line 127)
- `includes/error.h`: Delete `print_error` declaration (line 43)
- `includes/keyguide.h`: Delete `keyguide_render_background` declaration

### FR-004: Remove dead function from source

- `src/keyguide/keyguide_render.c`: Delete `keyguide_render_background` function (lines 25-52, including docstring)
- `src/utils/error.c`: Delete `print_error` function (lines 83-100, including docstring)

### FR-005: Move `in_range` and delete legacy parser files

- Move `in_range()` from `src/parser/parse_validation.c:26-29` to `src/parser/parse_validation_strict.c` (top of file, after includes)
- No header change needed (`parser.h:212` already declares it)
- Delete `src/parser/parse_validation.c` (empty after in_range move + parse_vector/parse_color already dead)
- Delete `src/utils/ft_atof.c` (only caller was dead parse_vector)
- Delete `ft_atof` declaration from `includes/minirt.h:127`
- Update Makefile SRCS (2 lines: parse_validation.c, ft_atof.c)

### FR-006: Replace `cleanup_all` with direct calls

In `src/window/window_lifecycle.c`, replace:
```c
cleanup_all(render->scene, render);
```
With:
```c
t_scene *scene;
scene = render->scene;
render_destroy(render);
scene_destroy(scene);
```
Note: Must save `render->scene` before `render_destroy` frees render.

After replacement:
- Delete `src/utils/cleanup.c`
- Delete `cleanup_all` declaration from `includes/minirt.h:123-124`
- Update Makefile SRCS (1 line: cleanup.c)

### FR-007: Replace `print_error` with `error_print`

In `src/parser/parser.c`, replace all 7 `print_error()` calls with `error_print()` using comma operator for return value:

| Call site | Old | New |
|-----------|-----|-----|
| validate_scene:56 | `print_error("Missing ambient lighting (A)")` | `error_print(ERR_PARSE_MISSING), 0` |
| validate_scene:58 | `print_error("Missing camera (C)")` | `error_print(ERR_PARSE_MISSING), 0` |
| validate_scene:60 | `print_error("Missing light (L)")` | `error_print(ERR_PARSE_MISSING), 0` |
| validate_scene:62 | `print_error("No objects in scene")` | `error_print(ERR_PARSE_MISSING), 0` |
| parse_scene:139 | `print_error("Invalid file extension...")` | `error_print(ERR_FILE_EXT), 0` |
| parse_scene:142 | `print_error("Cannot open file")` | `error_print(ERR_FILE_OPEN), 0` |
| parse_scene:147 | `print_error("Failed to initialize...")` | `error_print(ERR_MALLOC), 0` |

## 5. Constraints

- 42 Norm v4.1: All modified files must pass norminette
- Zero functional regression: all valid scenes render identically
- Zero error regression: all 18 invalid scene files still produce error output
- Build: `make re` must succeed with zero warnings

## 6. Acceptance Criteria

1. `make re` succeeds
2. `norminette src/ includes/` passes
3. Valid scene renders correctly
4. All 18 invalid scene test files produce appropriate error output
5. No deleted symbol referenced anywhere in remaining code
6. Makefile SRCS matches actual source files
