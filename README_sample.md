*This project has been created as part of the 42 curriculum by yoshin, jyoo.*

# miniRT — C Raytracer

[![Language](https://img.shields.io/badge/language-C-blue?style=flat-square)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)](LICENSE)

---

## Description

miniRT is an interactive raytracer written in C, built as part of the 42 school graphics project. The goal is to render simple 3D scenes described in `.rt` files using the ray tracing technique, displaying the result in a window managed by the MiniLibX library.

The renderer supports three geometric primitives (sphere, plane, cylinder) with Phong lighting (ambient + diffuse + specular), hard/soft shadows via stochastic sampling, and a BVH (Bounding Volume Hierarchy) acceleration structure for efficient intersection testing. Users can interactively move the camera, select and transform objects, and adjust light positions in real-time through keyboard controls, with a HUD overlay showing scene information and performance metrics.

---

## Instructions

### Prerequisites

**Linux**
- `gcc`, `make`
- `libx11-dev`, `libxext-dev`, `libbsd-dev`

**macOS**
- `clang`, `make`
- OpenGL, AppKit (Xcode Command Line Tools)

### Compilation

```bash
# Clone with submodules (libft)
git clone --recursive https://github.com/gdtknight/miniRT.git
cd miniRT

# Linux: install required libraries
sudo apt-get install libx11-dev libxext-dev libbsd-dev

# Build
make
```

Other Makefile targets:

| Target | Description |
|--------|-------------|
| `make` | Build the project |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and binary |
| `make re` | Full rebuild |

### Execution

```bash
./miniRT <scene_file.rt>

# Examples
./miniRT scenes/valid/valid_smoke_simple.rt
./miniRT scenes/valid/valid_scene_complex.rt
```

### Controls

| Category | Key | Action |
|----------|-----|--------|
| **Camera Move** | W / X | Forward / Backward |
| | A / D | Left / Right |
| | Q / Z | Up / Down |
| **Camera Rotate** | E / C | Pitch Up / Down |
| **Camera Reset** | S | Reset to initial position |
| **Object Select** | TAB | Cycle to next object |
| **Object Move** | R/T, F/G, V/B | X, Y, Z axis |
| **Object Resize** | Y/U | Radius -/+ |
| | N/M | Height -/+ (cylinder) |
| **Object Rotate** | I/J, O/K, P/L | X, Y, Z axis |
| **Light Move** | [ / ], ; / ', , / . | X, Y, Z axis |
| **UI** | H | Toggle HUD |
| | Up / Down | HUD page navigation |
| | ESC | Exit |

---

## Features

- **Geometry**: Sphere, plane, cylinder
- **Lighting**: Phong model (ambient + diffuse + specular)
- **Shadows**: Hard and soft shadows (stochastic sampling with pre-computed offset LUT)
- **BVH Acceleration**: Median split, plane separation, any-hit shadow optimization
- **Interactive Controls**: Camera movement/rotation, object selection/move/resize/rotate, light movement
- **Debounce Rendering**: 4-state FSM (IDLE → ACTIVE → FINAL → COOLDOWN) for smooth interaction with low-quality preview and full-quality final render
- **HUD Overlay**: Scene info, performance metrics (FPS, ray count, BVH efficiency), key guide

---

## Scene File Format

Scenes are defined in `.rt` files:

```
# Ambient light (required, exactly 1)
A  0.2  255,255,255

# Camera (required, exactly 1)
C  0,0,-15  0,0,1  70

# Light (required, exactly 1)
L  0,8,-8  0.7  255,255,255

# Sphere (optional)
sp  0,0,0  6  255,0,0

# Plane (optional)
pl  0,-10,0  0,1,0  200,200,200

# Cylinder (optional)
cy  0,0,0  0,1,0  5  10  0,255,0
```

| ID | Format | Description |
|----|--------|-------------|
| A | `A <ratio> <R,G,B>` | Ambient light (ratio: 0.0–1.0) |
| C | `C <x,y,z> <nx,ny,nz> <fov>` | Camera (FOV: 0–180) |
| L | `L <x,y,z> <brightness> <R,G,B>` | Point light (brightness: 0.0–1.0) |
| sp | `sp <x,y,z> <diameter> <R,G,B>` | Sphere |
| pl | `pl <x,y,z> <nx,ny,nz> <R,G,B>` | Plane |
| cy | `cy <x,y,z> <nx,ny,nz> <d> <h> <R,G,B>` | Cylinder |

---

## Performance Benchmark

> Environment: Apple M1, 1440×900, `-O0` (no optimization flags)

| Scene | Baseline | After 4 Rounds | Improvement |
|-------|----------|-----------------|-------------|
| S2 (20 spheres) | 1,835.7ms | 1,218.5ms | **33.6%** |
| S3 (50 spheres) | 3,366.0ms | 1,432.2ms | **57.4%** |
| S4 (7sp+7cy+3pl) | 27,173.7ms | 6,066.8ms | **77.7%** |

Key metric changes on S4: shadow tests 369M → 83.7M, BVH skip rate 32.9% → 82.0%.

---

## Project Structure

```
miniRT/
├── src/
│   ├── main.c              # Entry point
│   ├── parser/             # .rt file parsing (14 files)
│   ├── render/             # Render pipeline, metrics, debounce FSM
│   ├── spatial/            # BVH construction/traversal, AABB
│   ├── lighting/           # Phong lighting, shadows
│   ├── ray/                # Ray-object intersection
│   ├── math/               # Vector operations
│   ├── hud/                # HUD overlay
│   ├── keyguide/           # Key guide rendering
│   ├── window/             # MiniLibX window/events
│   ├── scene/              # Scene management
│   ├── bvh_vis/            # BVH visualization
│   └── utils/              # Error handling, timer
├── includes/               # Header files (23)
├── scenes/                 # Test scene files
├── lib/                    # libft, MiniLibX
├── docs/                   # Wiki documentation source
└── Makefile
```

---

## Resources

### References

- [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html) — Peter Shirley. Core ray tracing concepts: ray-sphere intersection, surface normals, Lambertian diffuse.
- [Scratchapixel](https://www.scratchapixel.com/) — Detailed tutorials on ray-plane intersection, ray-cylinder intersection, and the Phong reflection model.
- [BVH Construction (PBRT)](https://pbr-book.org/4ed/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies) — Bounding Volume Hierarchy theory, SAH and median split strategies.
- [MiniLibX Documentation](https://harm-smits.github.io/42docs/libs/minilibx) — MiniLibX API reference for window management, image manipulation, and event handling.

### AI Usage

AI (Claude) was used as a development assistant in the following areas:

- **Documentation**: README, GitHub Wiki pages, and CHANGELOG were drafted and reviewed with AI assistance. All technical content was verified against actual source code.
- **Code review and debugging**: AI helped identify edge cases in ray-intersection logic, review epsilon threshold choices, and trace rendering pipeline issues.
- **Performance analysis**: AI assisted in analyzing benchmark results and suggesting optimization strategies (BVH plane separation, shadow any-hit acceleration, offset LUT pre-computation).
- **Refactoring guidance**: AI provided feedback on code structure to comply with 42 Norm constraints (function/variable/file limits) during the debounce FSM redesign.

All code was written, understood, and validated by the project author. AI was not used to generate the core algorithms (ray tracing, BVH construction, Phong lighting) directly.

---

## Release History

| Version | Date | Changes |
|---------|------|---------|
| v2.3.0 | 2026-02 | Debounce FSM redesign, keymap overhaul, dead code removal |
| v2.2.0 | 2026-02 | Plane BVH separation, shadow any-hit, 4-round optimization (S4 77.7%↑) |
| v2.1.0 | 2026-01 | BVH enabled by default, debounce rendering, BVH visualization |
| v2.0.0 | 2026-01 | Code refactoring, modularization |

See the full [CHANGELOG](docs/CHANGELOG.md) for details.

---

## License

MIT License — see [LICENSE](LICENSE)
