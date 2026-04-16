# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Run

```bash
make                # Build miniRT (compiles libft and MiniLibX automatically)
make clean          # Remove object files
make fclean         # Remove objects + binary + clean libraries
make re             # Full rebuild
make bonus          # Identical to `make` — bonus features are always compiled into the single miniRT binary
make norm           # Run norminette on src/ and includes/
```

`lib/libft` is a git submodule — clone with `--recursive` or run `git submodule update --init` after clone.

**Linux dependencies**: `sudo apt-get install libx11-dev libxext-dev libbsd-dev`

Object files go to `build/` (auto-created). Dependency tracking via `-MMD -MP` means incremental builds work correctly. `compile_commands.json` at the repo root provides clangd/LSP integration.

Run: `./miniRT scenes/valid/valid_smoke_simple.rt` (simplest scene) or `./miniRT <scene.rt> --bvh-vis` for BVH tree visualization. The `--bvh-vis` flag can appear before or after the scene file.

## Testing & Validation

No formal test framework. Manual scene-based testing:

```bash
./miniRT scenes/valid/valid_smoke_simple.rt           # smoke test
./miniRT scenes/valid/<scene>.rt                      # visual check
make norm                                             # norminette gate
```

Scene categories:
- `scenes/valid/` — correct `.rt` files that should render
- `scenes/invalid/` — malformed files that should fail parse with a specific error
- `scenes/perf/` — larger scenes for benchmarking BVH performance

Texture assets (bump maps) are in `textures/`, referenced by scene files.

**Memory check (Linux)**: `valgrind --suppressions=miniRT.supp --leak-check=full ./miniRT scenes/valid/valid_smoke_simple.rt`. `miniRT.supp` filters X11/XCB/MiniLibX "still reachable" leaks that user code cannot free.

## Git Workflow

- Branch model: `main` → `develop` → feature branches (`feature/*`, `fix/*`, or `NNN-description`)
- Feature specs live in `specs/NNN-feature-name/` (speckit workflow)
- PRs target `develop`; `main` receives merges from `develop`

Follow Conventional Commits: `feat:`, `fix:`, `docs:`, `refactor:`, `perf:`, `style:`, `chore:`. Include issue reference for non-chore/docs/style commits when available (e.g., `fix: handle empty scene (#123)`).

## 42 Norm v4.1 — Critical Rules

All C code **must** pass `norminette`. Key constraints:

- **Max 25 lines** per function body (excluding braces)
- **Max 4 parameters** per function, **max 5 variable declarations** per function
- **Max 5 function definitions** per `.c` file
- **80-column line limit** (tabs count as 4-char display width)
- **Forbidden**: `for`, `do...while`, `switch/case`, `goto`, ternary `?:`, VLAs
- **Naming**: `s_` structs, `t_` typedefs, `u_` unions, `e_` enums, `g_` globals; all snake_case
- **Formatting**: tab indentation, braces on own line (except struct/enum/union declarations), declarations before code with one blank line separator, no inline comments inside function bodies
- **No declaration+initialization** on same line (except globals, statics, constants)
- **Structs declared in .h only**, not in .c files
- **Single blank line** between functions, **no trailing whitespace**
- Every `.c` and `.h` file starts with the 42 header comment block
- **Only allowed loops**: `while` (no `for`, `do...while`)

## CI/CD Pipeline

GitHub Actions (`.github/workflows/`):
- **ci.yml**: Norminette gate → parallel Linux + macOS builds → tests. Linux uses `xvfb-run` for headless X11.
- **pr-validation.yml**: Commit format, branch naming, PR template, sensitive file detection, valgrind leak check.
- **release.yml**: Triggered on `v*.*.*` tags.

Norminette must pass before any build runs.

## Architecture Overview

miniRT is a real-time ray tracer with interactive camera/object manipulation via MiniLibX.

### Rendering Pipeline

```
main.c → parse .rt file → build BVH → mlx_loop()
  render_loop (per frame):
    render_scene_to_buffer() → per-pixel camera ray → trace (BVH or brute-force)
    → intersect objects → Phong lighting (ambient+diffuse+specular+shadows) → pixel buffer
    → hud_render() overlay → mlx_put_image_to_window()
```

### Module Map

Seven feature-based packages. All headers live in `includes/` mirroring `src/` layout; `#include "package/header.h"` form required.

| Package | Directory | Purpose |
|---------|-----------|---------|
| **common** | `src/common/` | Vec3 math, error handling, format helpers, timer |
| **scene** | `src/scene/` | Scene lifecycle, object list (dynamic array), bit flags |
| | `src/scene/parser/` | `.rt` file parsing with strict validation, bonus options |
| **spatial** | `src/spatial/intersect/` | Ray-object intersection: sphere, plane, cylinder, cone (body + caps) |
| | `src/spatial/aabb/` | AABB creation, merge, intersection |
| | `src/spatial/bvh/` | BVH construction (midpoint split, depth limit 20), traversal, any-hit |
| | `src/spatial/debug/` | Console BVH tree dump (`--bvh-vis` flag) |
| **render** | `src/render/` | Render loop, camera ray, trace dispatcher, debounce FSM, MLX window/pixel |
| **lighting** | `src/lighting/shading/` | Phong model, multi-light, `fast_pow32()` specular |
| | `src/lighting/shadow/` | Soft shadows (stratified sampling), shadow offset LUT, occlusion test |
| | `src/lighting/texture/` | Checkerboard patterns, XPM bump map loading and normal perturbation |
| **interact** | `src/interact/` | Event dispatch (window close, key, expose) |
| | `src/interact/input/` | Keyboard handlers for camera/object/light manipulation |
| | `src/interact/hud/` | HUD overlay with pagination |
| | `src/interact/keyguide/` | On-screen keyboard shortcut guide |
| **metrics** | `src/metrics/` | Frame timing, shadow/intersection counters |

`metrics.h` stays at `includes/` top level as a cross-cutting concern.

### Package API Scoping Rules

**Every header file under `includes/` exposes exactly one category:**
- **Public API headers** (e.g., `render/render.h`, `scene/scene.h`): types and functions callable from any module.
- **Internal headers** (`_internal.h` suffix, e.g., `spatial/bvh_internal.h`, `spatial/bvhd_internal.h`): implementation-detail types and helpers used only within that subpackage. Do NOT include from outside the subpackage.
- **Type-only headers** (e.g., `interact/ui_types.h`, `interact/key_binds.h`, `render/window.h`): shared type definitions that multiple packages embed as struct members; break circular dependencies without pulling full API surface.

**When adding a new function:**
- If it's called only from one `.c` file → make it `static`, don't declare in any header.
- If it's called across files within one subpackage → put the declaration in the subpackage's `_internal.h`.
- If it's called from another package → put the declaration in the public package header.

**Every `.c` file must directly `#include` each header that defines a symbol it uses.** Never rely on transitive includes (e.g., if `render.h` happens to pull in `scene.h`, a file using `t_scene` must include `scene/scene.h` directly).

### Parser Invariants

The parser (`src/scene/parser/`) guarantees these invariants before rendering begins — runtime code can rely on them without redundant checks:

- Exactly one ambient light (`A`) and one camera (`C`) are present
- `light_count >= 1` and `objects.count >= 1`
- All color values are clamped to [0, 255], ratios to [0.0, 1.0]
- Axis vectors are normalized and non-zero
- Cylinder/cone dimensions are positive
- FOV ∈ [1, 179] (open interval to prevent `tan(fov/2)` divergence)

### Key Design Decisions

- **Planes excluded from BVH**: infinite AABBs, stored in `t_bvh.plane_refs`, tested after BVH traversal
- **BVH any-hit for shadows**: `bvh_any_hit()` returns immediately on first occlusion
- **Debounce FSM**: 4-state (IDLE→ACTIVE→FINAL→COOLDOWN) renders at low quality during interaction, full quality after settling
- **Camera basis caching**: `t_camera_cache` with dirty flag avoids recomputing per-pixel
- **Type safety in spatial module**: BVH functions use `t_scene *` (not `void *`) for scene parameters
- **Unified hit record**: use `t_hit` only (no `t_hit_record` alias)
- **`t_shadow_config` owned by scene**: embedded in `t_scene` so `scene.h` does not depend on `lighting/`

### Core Headers

- `includes/scene/scene.h` — Scene struct, camera, lights, ambient, shadow config
- `includes/common/objects.h` — `t_object` with type-discriminated union (`u_object_data`)
- `includes/common/ray.h` — `t_ray` (with precomputed `inv_dir`), `t_hit`, `EPSILON`, `RAY_T_MIN`, `COEFF_EPSILON`
- `includes/spatial/spatial.h` — BVH node, AABB, traversal context
- `includes/render/render.h` — `t_render` context (MLX, scene ref, HUD state, selection, debounce)
- `includes/render/window.h` — `t_mlx_img`, `t_mlx_context`, `WINDOW_WIDTH/HEIGHT`, `ASPECT_RATIO_*`
- `includes/interact/ui_types.h` — UI state (`t_selection`, `t_hud_state`, `t_keyguide_state`)
- `includes/interact/key_binds.h` — Key dispatch (`t_key_handler`, `t_key_bind`, `t_key_binds`)
- `includes/interact/input.h` — Key codes with `#ifdef __APPLE__` / `__linux__` sections
- `includes/interact/event.h` — MLX event callbacks (close, key, expose)

### Norm-Capacity Warnings

These files are at the 5-function Norm limit — adding functions requires splitting into a new file:
- `src/main.c`
- `src/common/format_helpers.c`
- `src/interact/hud/hud_scene.c`
- `src/interact/input/input_camera.c`
- `src/interact/keyguide/keyguide_render_extra.c`
- `src/lighting/texture/checkerboard.c`
- `src/metrics/metrics_frame.c`
- `src/render/render.c`
- `src/render/render_debounce.c`
- `src/scene/parser/parse_elements.c`
- `src/scene/parser/parse_number.c`
- `src/scene/parser/parse_objects.c`
- `src/spatial/aabb/aabb_bounds.c`
- `src/spatial/bvh/bvh_init.c`

Verify before adding a function: `grep -cE '^[a-zA-Z_].*\(.*\)$' <file>` (rough heuristic; trust the actual file content).

### Scene File Format (`.rt`)

```
A  <ratio>  <R,G,B>                           # Ambient (required, exactly 1)
C  <x,y,z>  <nx,ny,nz>  <fov>                 # Camera (required, exactly 1)
L  <x,y,z>  <brightness>  <R,G,B>             # Light (required, 1+)
sp <x,y,z>  <diameter>  <R,G,B>               # Sphere
pl <x,y,z>  <nx,ny,nz>  <R,G,B>               # Plane
cy <x,y,z>  <nx,ny,nz>  <d>  <h>  <R,G,B>     # Cylinder
co <x,y,z>  <nx,ny,nz>  <d>  <h>  <R,G,B>     # Cone (bonus)
```

Bonus options can follow object definitions: `checker` (checkerboard texture), `bump <file.xpm>` (bump map).

### Platform Support

macOS (OpenGL + AppKit) and Linux (X11 + Xext). OS detection is automatic in the Makefile via `uname -s`. On Linux, `mlx_destroy_display()` is called for cleanup (not available on macOS).

### Lifecycle Flow

```
scene_create() → parse_scene() → build_scene_bvh() → render_create()
  → load_all_bump_maps() → mlx_loop() [event-driven]
  → cleanup_all_bump_maps() → render_destroy() → scene_destroy()
```

All allocations follow create/destroy pairs. The render context owns the MLX connection and window; the scene owns the BVH and object list.

### Documentation

`docs/` has per-module design docs (`Module-*.md`), `Architecture.md`, `Data-Structures.md`, and `Performance.md`. `specs/` contains numbered feature specs with historical design decisions and implementation plans. `wiki/` holds the GitHub wiki content (synced via `wiki-sync.yml`).

### Interactive Controls

| Category | Keys | Action |
|----------|------|--------|
| Camera move | W/X, A/D, Q/Z | Forward/back, left/right, up/down |
| Camera rotate | E/C | Pitch up/down |
| Camera reset | S | Restore initial position |
| Object select | TAB | Cycle through objects |
| Object move | R/T, F/G, V/B | X, Y, Z axis |
| Object resize | Y/U, N/M | Radius -/+, height -/+ (cylinder/cone) |
| Object rotate | I/J, O/K, P/L | X, Y, Z axis |
| Light move | [ / ], ; / ', , / . | X, Y, Z axis |
| UI | H / Up / Down / ESC | HUD toggle / page nav / quit |
