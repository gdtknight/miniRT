# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
make                # Build miniRT (compiles libft and MiniLibX automatically)
make clean          # Remove object files
make fclean         # Remove objects + binary + clean libraries
make re             # Full rebuild
make bonus          # Same as `make` (bonus features are compiled into the main binary)
make norm           # Run norminette on src/ and includes/
```

Run: `./miniRT scenes/valid/valid_smoke_simple.rt` (simplest scene) or `./miniRT <scene.rt> --bvh-vis` for BVH tree visualization.

There is no formal test framework. Manual test files exist in `tests/` and test scenes in `scenes/valid/`.

## 42 Norm v4.1 — Critical Rules

All C code **must** pass `norminette`. Key constraints:

- **Max 25 lines** per function body (excluding braces)
- **Max 4 parameters** per function, **max 5 variable declarations** per function
- **Max 5 function definitions** per `.c` file
- **80-column line limit** (tabs count as their display width)
- **Forbidden**: `for`, `do...while`, `switch/case`, `goto`, ternary `?:`, VLAs
- **Naming**: `s_` structs, `t_` typedefs, `u_` unions, `e_` enums, `g_` globals; all snake_case
- **Formatting**: tab indentation (4-char width), braces on own line (except struct/enum/union declarations), declarations before code with one blank line separator, no inline comments inside function bodies
- **No declaration+initialization** on same line (except globals, statics, constants)
- **Structs declared in .h only**, not in .c files
- Every `.c` and `.h` file starts with the 42 header comment block

## Architecture Overview

miniRT is a real-time ray tracer with interactive camera/object manipulation via MiniLibX.

### Rendering Pipeline

```
main.c → parse .rt file → build BVH → mlx_loop()
  └─ render_loop (per frame):
       render_scene_to_buffer() → per-pixel camera ray → trace (BVH or brute-force)
       → intersect objects → Phong lighting (ambient+diffuse+specular+shadows) → pixel buffer
       → hud_render() overlay → mlx_put_image_to_window()
```

### Module Map

| Directory | Purpose |
|-----------|---------|
| `src/render/` | Render loop, camera ray generation, ray tracing dispatcher, debounce FSM, performance metrics |
| `src/spatial/` | BVH construction (midpoint split, depth limit 20), traversal, any-hit shadow optimization, AABB ops |
| `src/lighting/` | Phong model, soft shadows (stratified sampling), shadow offset LUT, `fast_pow32()` specular |
| `src/ray/` | Ray-object intersection: sphere (quadratic), plane (linear), cylinder, cone (body + caps) |
| `src/parser/` | `.rt` file parsing with strict validation, bonus options (checker, bump map) |
| `src/window/` | MiniLibX window setup, keyboard event handlers, camera/object/light manipulation |
| `src/hud/` | HUD overlay (scene info, FPS, object list) with pagination |
| `src/math/` | Vec3 operations (add, sub, dot, cross, normalize) |
| `src/scene/` | Scene lifecycle, object list (dynamic array), bit flags |
| `src/texture/` | Checkerboard patterns, XPM bump map loading and normal perturbation |
| `src/bvh_vis/` | Console BVH tree dump (`--bvh-vis` flag) |

### Key Design Decisions

- **Planes excluded from BVH**: planes have infinite AABBs, so they are stored separately in `t_bvh.plane_refs` and tested after BVH traversal
- **BVH any-hit for shadows**: `bvh_any_hit()` returns immediately on first occlusion (no closest-hit needed)
- **Debounce FSM**: 4-state machine (IDLE→ACTIVE→FINAL→COOLDOWN) renders at low quality (2x2 blocks) during interaction, then full quality after settling
- **Camera basis caching**: `t_camera_cache` with dirty flag avoids recomputing per-pixel

### Key Headers

- `includes/minirt.h` — Scene, camera, lights, ambient; core rendering prototypes
- `includes/objects.h` — `t_object` with type-discriminated union (`u_object_data`)
- `includes/ray.h` — `t_ray` (with precomputed `inv_dir`), `t_hit`
- `includes/spatial.h` — BVH node, AABB, traversal context
- `includes/window.h` — `t_render` context (MLX, scene ref, HUD state, selection, debounce)
- `includes/shadow.h` — Shadow config, offset LUT

### Platform Support

macOS (OpenGL + AppKit) and Linux (X11 + Xext). OS detection is automatic in the Makefile. On Linux, `mlx_destroy_display()` is called for cleanup.
