# Implementation Plan: BVH Performance Verification and Debugging Tools

**Branch**: `008-bvh-performance-diagnostics` | **Date**: 2025-12-31 | **Spec**: [specs/008-bvh-performance-diagnostics/spec.md](spec.md)
**Input**: Feature specification from `/specs/008-bvh-performance-diagnostics/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Add real-time performance monitoring and diagnostics tools to visualize BVH acceleration effectiveness. System will display FPS, frame time, BVH status in HUD, track intersection test statistics, measure BVH tree quality metrics, and provide developers with immediate feedback on spatial acceleration performance. Implementation activates existing metrics infrastructure defined in `metrics.h` and integrates performance display into the current HUD system.

## Technical Context

**Language/Version**: C (C99 standard with 42 School norminette compliance)  
**Primary Dependencies**: MiniLibX (graphics), libft (custom C library), math.h (standard math functions)  
**Storage**: N/A (all data in-memory during runtime)  
**Testing**: Manual testing with test scenes (simple/complex object counts), valgrind for memory validation  
**Target Platform**: macOS and Linux with X11 support  
**Project Type**: Single desktop application (ray tracer with real-time controls)  
**Performance Goals**: <16ms frame time (60 FPS) for simple scenes, 10x+ speedup with BVH on complex scenes (50+ objects), <5% overhead from metrics collection  
**Constraints**: 42 School function restrictions (no pthread/fork), 25-line function limit (norminette), stack-allocated metrics structures, microsecond-precision timing  
**Scale/Scope**: Support scenes from 1-100+ objects, metrics for thousands of rays per frame, HUD display integrated with existing left-side panel

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Code Quality Automation** (Principle II):
- ✅ All code changes will pass norminette with zero warnings
- ✅ Memory leak check with valgrind will show zero leaks (metrics use stack allocation)
- ✅ Build will succeed on both macOS and Linux platforms
- ✅ No manual quality gate exceptions required

**42 School Constraints Check** (Principle VII):
- ✅ All functions used are from allowed list: `gettimeofday()` (sys/time.h), `printf()` (stdio.h), `malloc/free` (stdlib.h), `mlx_string_put()` (MiniLibX)
- ✅ pthread functions will NOT be used (single-threaded metrics collection)
- ✅ fork/pipe/multiprocessing will NOT be used  
- ✅ External parallelization libraries (SIMD, OpenMP, etc.) will NOT be used
- ✅ Optimizations use only allowed techniques: algorithmic (BVH traversal tracking), caching (per-frame counter resets), memory layout (struct field ordering)

**Project Structure Standards** (Principle I):
- ✅ New metrics implementation files will be in `src/render/` directory
- ✅ Performance display code will extend existing `src/hud/` module
- ✅ No scattered helper scripts or backup files will be created

**Documentation Standards**:
- ✅ All C functions will have English Doxygen-style comments
- ✅ 42 Header format will be included in all new C files
- ✅ Complex algorithms (FPS calculation, efficiency metrics) will include explanation comments

**GATE STATUS**: ✅ **PASSED** - No constitution violations detected. All requirements use allowed functions and follow norminette standards.

## Project Structure

### Documentation (this feature)

```text
specs/008-bvh-performance-diagnostics/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output - timer precision, HUD layout, metrics calculation
├── data-model.md        # Phase 1 output - metrics structures, counter types, display format
├── quickstart.md        # Phase 1 output - enabling metrics, interpreting output, key bindings
├── contracts/           # Phase 1 output - metrics API contracts, HUD extension interface
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
# Single project structure (miniRT ray tracer)
src/
├── render/
│   ├── render.c                  # (modify) Add metrics_start_frame/end_frame calls
│   ├── render_state.c            # (existing) Already includes metrics_init
│   ├── trace.c                   # (modify) Add metrics_add_ray counter calls
│   ├── metrics.c                 # (new) Implement timer and counter operations
│   └── metrics_calc.c            # (new) Calculate FPS, efficiency, averages
├── spatial/
│   ├── bvh_traverse.c            # (modify) Add metrics_add_bvh_node_visit and skip counters
│   ├── bvh_init.c                # (modify) Log build statistics to terminal
│   └── bvh_intersect.c           # (modify) Add metrics_add_intersect_test counters
├── hud/
│   ├── hud_render.c              # (modify) Add performance section rendering
│   ├── hud_performance.c         # (new) Render FPS, frame time, BVH stats
│   └── hud_format.c              # (existing) Reuse float/number formatting
└── window/
    └── window.c                  # (modify) Hook metrics display to 'I' key toggle

includes/
├── metrics.h                     # (existing) Already defines t_metrics structure
└── hud.h                         # (modify) Add hud_render_performance() declaration

tests/
├── integration/
│   ├── test_metrics_overhead.sh  # Measure <5% overhead requirement
│   └── test_bvh_effectiveness.sh # Verify 10x+ speedup on complex scenes
└── scenes/
    ├── simple_scene.rt           # 3-5 objects for overhead testing
    └── complex_scene.rt          # 50+ objects for BVH validation
```

**Structure Decision**: Single project structure is used as miniRT is a standalone desktop application. All performance metrics code integrates into the existing `src/render/` and `src/hud/` directories. The metrics.h header already exists with the necessary structures defined, so implementation focuses on activating this infrastructure and adding display capabilities to the HUD system.

## Complexity Tracking

**No Constitution Violations** - This section is intentionally left empty as the constitution check passed without requiring any exceptions or justifications. All implementation follows allowed functions, norminette standards, and existing project structure.
