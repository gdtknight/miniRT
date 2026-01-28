# Implementation Plan: Advanced Rendering Performance Optimization

**Branch**: `002-rendering-optimization` | **Date**: 2025-12-19 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/002-rendering-optimization/spec.md`

## Summary

Optimize miniRT rendering performance through spatial acceleration
(BVH), adaptive quality control, and progressive tile rendering.
Parallel ray tracing (US1) is excluded due to 42 School's function
restrictions (pthread not allowed). BVH provides the primary
performance gain (10-100x for complex scenes).

## Technical Context

**Language/Version**: C99 (42 Norminette compliant)
**Primary Dependencies**: MinilibX, libm, libc
**Storage**: N/A
**Testing**: Manual verification with test scenes, valgrind
**Target Platform**: macOS (primary), Linux (secondary)
**Project Type**: single
**Performance Goals**: <3s render for 50-object scenes, >10 FPS interactive
**Constraints**: No pthread, no forbidden functions, Norminette compliant
**Scale/Scope**: Scenes with 10-100 objects

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- [x] **I. Norminette**: All new code MUST pass Norminette v4.1
- [x] **II. Memory Safety**: BVH tree MUST be fully freed on cleanup;
  no leaks from spatial structures or metrics buffers
- [x] **III. Allowed Functions**: ONLY libc/libm/MinilibX/gettimeofday;
  pthread explicitly excluded (US1 SKIPPED)
- [x] **IV. Modular Architecture**: New modules (`src/spatial/`,
  `src/render/`) with headers in `includes/`
- [x] **V. Visual Correctness**: BVH traversal MUST produce identical
  output to brute-force intersection (pixel-perfect)
- [x] **VI. Stability**: No crashes from BVH traversal edge cases
  (empty scenes, single object, degenerate geometry)
- [x] **VII. Window Management**: Quality switching and progressive
  rendering MUST NOT affect window responsiveness

## Project Structure

### Documentation (this feature)

```text
specs/002-rendering-optimization/
├── spec.md
├── plan.md              # This file
├── research.md
├── data-model.md
├── quickstart.md
├── tasks.md
└── checklists/
    └── requirements.md
```

### Source Code (repository root)

```text
includes/
├── spatial.h            # BVH and AABB structures/API
├── metrics.h            # Performance metrics structures/API
├── render_state.h       # Render state, quality, progressive
├── render_quality.h     # Quality mode API
└── render_progressive.h # Progressive tile API

src/spatial/
├── aabb.c               # AABB operations
├── aabb_basic.c         # Basic AABB functions
├── aabb_shapes.c        # Shape-specific bounds
├── bounds.c             # Object bounding box calculation
├── bvh_init.c           # BVH initialization entry point
├── bvh_build_core.c     # Recursive BVH construction
├── bvh_build_split.c    # SAH split plane evaluation
├── bvh_build_partition.c # Object partitioning
├── bvh_traverse.c       # Ray-BVH intersection traversal
└── bvh_lifecycle.c      # BVH memory management

src/render/
├── render.c             # Main render loop (modified)
├── render_quality.c     # Quality mode switching
├── render_state.c       # Render state management
├── render_progressive.c # Progressive tile system
├── metrics_calc.c       # Metrics calculation
├── metrics_counters.c   # Ray/BVH counter accumulation
├── metrics_frame.c      # Frame timing
└── trace.c              # Ray tracing with BVH integration
```

**Structure Decision**: Spatial acceleration in dedicated `src/spatial/`
module; rendering infrastructure in `src/render/`; metrics integrated
into render pipeline.

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| BVH recursive build | SAH requires evaluating split planes at each level | Flat object list is O(n) per ray; unacceptable for >20 objects |
| Multiple render files | Norminette 5-function limit per file | Single render.c would exceed function limit |

## Implementation Phases

### Phase 0: Research (COMPLETE)

See [research.md](research.md) for findings:
- pthread NOT allowed (US1 SKIPPED)
- SAH selected for BVH construction
- Slab method for AABB intersection
- gettimeofday for timing/metrics

### Phase 1: Infrastructure (COMPLETE)

Headers and foundational structures:
- `includes/spatial.h` - BVH/AABB types
- `includes/metrics.h` - Performance metric types
- `includes/render_state.h` - Quality/progressive state
- Timer utilities using `gettimeofday()`

### Phase 2: BVH Spatial Acceleration (COMPLETE)

Core implementation:
1. AABB structure and intersection test (slab method)
2. Per-primitive bounding box computation (sphere, plane, cylinder)
3. BVH construction with SAH split evaluation
4. BVH traversal integrated into `trace_ray()`
5. BVH rebuild on scene changes
6. Memory management and cleanup

**Result**: 10-100x speedup for scenes with >20 objects

### Phase 3: Adaptive Quality (PARTIALLY COMPLETE)

Implemented:
- Binary quality mode (LOW/HIGH)
- Interaction state tracking with timestamp
- 1-second upgrade delay after interaction stops
- LOW mode: 2x2 pixel block rendering

Remaining:
- Ray sample reduction in LOW mode
- Simplified shadow calculation in LOW mode
- Frame rate monitoring
- Quality mode display in HUD

### Phase 4: Progressive Tile Rendering (PARTIALLY COMPLETE)

Implemented:
- Tile structure (32x32 pixels default)
- Tile coordinate generation
- Progressive state tracking

Remaining:
- Non-sequential tile ordering (spiral pattern)
- Per-tile display updates
- Render cancellation preserving completed tiles
- Tile completion percentage display

### Phase 5: Performance Metrics (COMPLETE)

Implemented:
- Frame timing with rolling average FPS
- Ray count tracking
- BVH node visit counting
- BVH efficiency estimation
- Metrics display via HUD

## Key Decisions

1. **US1 SKIPPED**: pthread not allowed; BVH becomes primary
   optimization
2. **SAH over median split**: Higher quality BVH trees justify
   slightly more complex construction
3. **Binary quality**: LOW/HIGH sufficient; multi-level adds
   complexity without proportional benefit
4. **32x32 tiles**: Balance between per-tile overhead and visual
   feedback granularity
