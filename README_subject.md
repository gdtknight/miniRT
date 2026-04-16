*This project has been created as part of the 42 curriculum by yoshin, jyoo.*

# miniRT

A real-time interactive ray tracer written in C, using the MiniLibX library. miniRT renders scenes described in `.rt` files with spheres, planes, cylinders, and cones, lit by a Phong shading model with ambient, diffuse, specular, and shadow contributions.

## Description

miniRT is the 42 curriculum's first computer graphics project. The goal is to implement a ray tracer from scratch — parsing a scene description file, casting primary rays through a virtual camera, intersecting them with simple geometric primitives, and shading the resulting hit points.

This implementation goes beyond the mandatory baseline with:

- **Primitives**: sphere, plane, cylinder (with caps), cone (bonus)
- **Shading**: full Phong model (ambient + diffuse + specular)
- **Shadows**: hard shadows and bonus soft shadows via stratified sampling
- **Acceleration**: BVH (Bounding Volume Hierarchy) with midpoint split and any-hit shadow traversal; planes are separated from the BVH and tested afterwards
- **Interactivity**: live camera/object/light manipulation with a debounce FSM that drops to low quality during input and re-renders at full quality after settling
- **Textures**: checkerboard patterns and XPM bump maps
- **HUD**: on-screen scene info, performance metrics, and a paginated key guide

The renderer runs on macOS (OpenGL + AppKit MiniLibX) and Linux (X11 + Xext MiniLibX); the platform is detected automatically by the Makefile.

## Features

### Mandatory (subject-required)

- Scene parsing from `.rt` files with strict validation and `Error\n` reporting
- Primitives: sphere (`sp`), plane (`pl`), cylinder (`cy`) with caps
- Camera (`C`), ambient light (`A`), point light (`L`)
- Ambient + diffuse Lambertian shading
- Hard shadows (single occlusion ray per light)
- Object translation, rotation, and resizing where applicable
- Window/event handling via MiniLibX (clean exit on `ESC` and window close)
- Memory-leak-free execution (validated with valgrind on Linux)

### Bonus (extensions in this implementation)

- Cone primitive (`co`) with caps
- Specular highlights (full Phong reflection model)
- Soft shadows via stratified disk sampling with a precomputed offset LUT
- Multiple point lights with per-light selection (`=`)
- Checkerboard pattern (`checker:R,G,B`)
- XPM bump mapping (`bump:file.xpm`)
- BVH (Bounding Volume Hierarchy) acceleration with plane separation and any-hit shadow traversal
- BVH console visualization (`--bvh-vis`)
- Interactive camera, object, and light manipulation with a 4-state debounce FSM (low-quality preview during input, full-quality render after settling)
- HUD overlay with paginated scene info, performance metrics, and key guide

## Instructions

### Prerequisites

- `cc`, `make`
- **Linux**: `sudo apt-get install libx11-dev libxext-dev libbsd-dev`
- **macOS**: Xcode Command Line Tools

### Build

```bash
git clone --recursive <repo-url> miniRT
cd miniRT
make            # builds libft, MiniLibX, and miniRT
make bonus      # identical to `make` — bonuses are always compiled in
make clean      # remove object files
make fclean     # remove objects + binary + clean libraries
make re         # full rebuild
make norm       # run norminette gate on src/ and includes/
```

If the repository was cloned without `--recursive`, run `git submodule update --init` to fetch `lib/libft`.

### Run

```bash
./miniRT scenes/valid/valid_smoke_simple.rt        # smoke test
./miniRT <scene.rt>                                # render any scene
./miniRT <scene.rt> --bvh-vis                      # also dump the BVH tree
```

### Memory check (Linux)

```bash
valgrind --suppressions=miniRT.supp --leak-check=full \
  ./miniRT scenes/valid/valid_smoke_simple.rt
```

`miniRT.supp` filters X11/XCB/MiniLibX "still reachable" allocations that user code cannot free.

### Scene file format (`.rt`)

```
A  <ratio>           <R,G,B>                       # ambient (1, required)
C  <x,y,z>           <nx,ny,nz>  <fov>             # camera  (1, required)
L  <x,y,z>           <bright>    <R,G,B>           # light   (1+, required)
sp <x,y,z>           <diameter>  <R,G,B>
pl <x,y,z>           <nx,ny,nz>  <R,G,B>
cy <x,y,z>           <nx,ny,nz>  <d> <h>  <R,G,B>
co <x,y,z>           <nx,ny,nz>  <d> <h>  <R,G,B>  # bonus
```

Bonus options can be appended to any object line, colon-delimited:

- `checker:R,G,B` — checkerboard pattern with the given alternate color (RGB required, range `[0,255]`)
- `bump:file.xpm` — XPM bump map (path relative to the working directory)

Both options can be chained on the same object.

### Controls

| Category | Keys | Action |
|---|---|---|
| Camera move | W/X, A/D, Q/Z | Forward/back, left/right, up/down |
| Camera rotate (pitch) | E/C | Pitch up/down |
| Camera rotate (yaw) | 1/3 | Yaw right/left |
| Camera reset | S | Restore initial position |
| Object select | TAB | Cycle through objects |
| Object move | R/T, F/G, V/B | X, Y, Z axis |
| Object resize | Y/U, N/M | Radius, height (cylinder/cone) |
| Object rotate | I/J, O/K, P/L | X, Y, Z axis |
| Light select | = | Cycle through lights |
| Light move | [ / ], ; / ', , / . | X, Y, Z axis |
| UI | H / Up / Down / ESC | HUD toggle / page nav / quit |

## Performance

Measured on Apple M1, 1440×900 window, `-O0` (no compiler optimization), 16 shadow samples, BVH enabled. Raw measurements are archived in `docs/archive/benchmark-baseline.md` and `docs/archive/benchmark-results.md`.

| Scene | Baseline | Final (R4) | Improvement |
|---|---:|---:|---:|
| `perf_spheres_20.rt` (20 sp) | 1,835.7 ms | 1,218.5 ms | **33.6 %** |
| `perf_spheres_50.rt` (50 sp) | 3,366.0 ms | 1,432.2 ms | **57.4 %** |
| `perf_all_objects.rt` (7 sp + 7 cy + 3 pl) | 27,173.7 ms | 6,066.8 ms | **77.7 %** |

The four optimization rounds were: (R1) removing the BVH-fallback brute-force pass, (R2) `fast_pow32` and sqrt caching in the shading hot path, (R3) precomputed shadow offset LUT, and (R4) plane separation from the BVH plus any-hit shadow traversal. On the heaviest mixed scene this reduced shadow intersection tests from 369 M to 83.7 M and raised the BVH skip rate from 32.9 % to 82.0 %.

## Resources

### Ray tracing fundamentals

- *Ray Tracing in One Weekend* — Peter Shirley. Foundational pipeline, vector math, Lambertian/Phong shading.
- *Physically Based Rendering: From Theory to Implementation* (PBRT) — Pharr, Jakob, Humphreys. Reference for BVH construction and traversal.
- *Real-Time Rendering* (4th ed.) — Akenine-Möller et al. Phong reflection model, AABB intersection.
- Scratchapixel 2.0 — `https://www.scratchapixel.com/` — ray-primitive intersection derivations (cylinder, cone caps).
- Inigo Quilez — `https://iquilezles.org/articles/` — numerical robustness notes.

### MiniLibX

- 42 docs MiniLibX manual — image buffer, event loop, key codes per platform.

### AI usage

AI assistants (Claude Code, ChatGPT) were used as a research and review aid for the following scoped tasks. All resulting code was read line-by-line, adapted to the 42 Norm, benchmarked, and is fully understood by the authors:

- **Spatial acceleration (BVH) for performance**: discussing midpoint vs. SAH split trade-offs, depth/leaf-size heuristics, the rationale for separating infinite primitives (planes) from the BVH, and the any-hit shortcut used by shadow rays. AI was used to sanity-check the traversal stack design and AABB slab-intersection math; the implementation in `src/spatial/bvh/` and `src/spatial/aabb/` was written and tuned by the authors against the `scenes/perf/` benchmarks.
- **Soft-shadow shading**: deriving the stratified disk sampling pattern around each light, precomputing the shadow offset LUT (`cos/sin/sqrt` table) to eliminate per-sample trig, and integrating the multi-sample occlusion result into the Phong shader without breaking the hard-shadow fast path. AI helped review the bias/variance trade-off of sample counts; the shader code under `src/lighting/shading/` and `src/lighting/shadow/` was authored by us.
- **BVHD — BVH visualization for debugging spatial acceleration**: AI helped sketch a console tree-dump format (depth-indented nodes, AABB extents, primitive counts per leaf) that made it easy to diagnose pathological splits and verify plane separation. The `--bvh-vis` flag and `src/spatial/debug/` implementation were written by us.
- **Performance metrics subsystem**: brainstorming which counters meaningfully reflect ray-tracer cost — frame time, primary ray count, shadow ray count, BVH node visits, BVH skip rate, intersection tests — and how to surface them in the HUD without distorting the measurement (deferred formatting, atomic-free single-threaded counters). The `src/metrics/` module and HUD wiring were implemented by us and used to drive the four optimization rounds documented in `README.md`.
- **Refactoring under 42 Norm constraints**: splitting functions exceeding 25 lines, naming extracted helpers, and reorganizing files approaching the 5-function-per-file ceiling.
- **Documentation**: drafting per-module wiki pages and this README, edited and fact-checked by the authors.
