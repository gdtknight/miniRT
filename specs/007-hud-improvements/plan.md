# Implementation Plan: HUD Improvements and Key Guide

**Branch**: `007-hud-improvements` | **Date**: 2025-12-30 | **Spec**: [specs/007-hud-improvements/spec.md](spec.md)
**Input**: Feature specification from `/specs/007-hud-improvements/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Enhance the miniRT ray tracer HUD system with semi-transparent backgrounds for both main HUD and new key guide overlay, add real-time object selection updates, and increase default window resolution from 800x600 to 1440x900 for comfortable viewing on modern displays. Technical approach involves manual alpha blending in MiniLibX image buffers (no native alpha support), implementing new key guide HUD in upper-right corner, triggering HUD dirty flag on selection changes, and updating window/camera initialization constants throughout codebase.

## Technical Context

**Language/Version**: C (C99 standard), 42 School norminette compliance  
**Primary Dependencies**: MiniLibX (42 School graphics library), libm (math library), libft (custom C library)  
**Storage**: N/A (no persistent storage required for this feature)  
**Testing**: Manual testing with various .rt scene files, visual verification of HUD rendering  
**Target Platform**: macOS (Darwin), Linux/X11 - cross-platform support maintained
**Project Type**: Single desktop application (ray tracer with graphical window)  
**Performance Goals**: HUD render ≤2ms, selection update within 16ms (60fps), maintain 60fps during camera movement  
**Constraints**: 42 School function constraints (no pthread/fork), norminette compliance (≤25 lines/function), MiniLibX limitations (no native alpha blending)  
**Scale/Scope**: ~15K LOC codebase, 5 HUD source files to modify, 3-4 new source files for key guide, ~10 constants to update for resolution change

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Initial Check (Pre-Phase 0)**: ✅ PASSED  
**Post-Design Check (Post-Phase 1)**: ✅ PASSED

**Project Structure Standards** (Principle I):
- ✅ All HUD implementation files in `src/hud/` directory (existing structure maintained)
- ✅ Header files in `includes/` directory (hud.h already exists, will add keyguide.h)
- ✅ No scattered automation scripts or deprecated files introduced
- ✅ Documentation will be organized in `specs/007-hud-improvements/`
- ✅ **Post-Design**: New `src/keyguide/` directory follows existing pattern, contracts in `specs/007-hud-improvements/contracts/`

**Code Quality Automation** (Principle II):
- ✅ All changes must pass norminette with zero warnings
- ✅ Build must succeed on macOS and Linux
- ✅ Memory leak check via valgrind required (image buffers properly freed)
- ✅ CI pipeline will enforce all quality gates
- ✅ **Post-Design**: Cleanup functions properly destroy MiniLibX images, no new malloc without paired free

**Documentation and Wiki Synchronization** (Principle III):
- ✅ Korean documentation will be created in `docs/specs/007-hud-improvements/`
- ✅ Both English and Korean versions synchronized
- ✅ Will update on release tag creation
- ✅ **Post-Design**: Plan, research, data-model, contracts, quickstart all created in English

**Workflow System** (Principle IV):
- ✅ Following structured workflow: spec → plan → research → design → implementation
- ✅ All checks automated in CI
- ✅ PR will include all quality validations
- ✅ **Post-Design**: Phase 0 and Phase 1 complete, ready for Phase 2 (tasks) and implementation

**Tools and Environment Standards** (Principle V):
- ✅ Supports macOS and Linux (platform-specific key codes already handled)
- ✅ MiniLibX dependency already managed in Makefile
- ✅ Test scenes in `scenes/` directory used for validation
- ✅ **Post-Design**: No new dependencies introduced, uses only existing MiniLibX and libc functions

**Bilingual Specification Management** (Principle VI):
- ✅ English specification exists in `specs/007-hud-improvements/spec.md`
- ✅ Korean translation required in `docs/specs/007-hud-improvements/spec.md`
- ✅ Will be created and synchronized simultaneously
- ⚠️ **Post-Design**: Korean translation pending (to be done during implementation phase)

**42 School Function Constraints** (Principle VII):
- ✅ Only allowed functions used: malloc, free, mlx_* functions, math.h functions
- ✅ pthread WILL NOT be used (single-threaded rendering maintained)
- ✅ fork/pipe WILL NOT be used
- ✅ No external parallelization libraries (SIMD, OpenMP) will be used
- ✅ Optimizations: Alpha blending via direct pixel manipulation (algorithmic)
- ✅ Memory layout: Image buffer optimization (cache-friendly access patterns)
- ✅ **Post-Design**: All contracts use only allowed functions (mlx_new_image, mlx_destroy_image, mlx_get_data_addr, mlx_string_put, standard arithmetic)

**Overall Status**: ✅ **APPROVED FOR IMPLEMENTATION**  
All constitution principles satisfied. No violations detected. Design follows existing patterns and constraints.

## Project Structure

### Documentation (this feature)

```text
specs/007-hud-improvements/
├── spec.md              # Feature specification (COMPLETE)
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
│   └── hud_api.h       # HUD and key guide API contract
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
# miniRT project structure (single desktop application)
includes/
├── hud.h               # MODIFY: Add key guide function declarations
├── keyguide.h          # NEW: Key guide HUD structures and API
├── minirt.h            # MODIFY: Update resolution constants/comments
├── window.h            # MODIFY: Update window constants
└── render_state.h      # MODIFY: Add key guide state if needed

src/
├── hud/                # MODIFY: Existing HUD implementation
│   ├── hud_init.c      # MODIFY: Implement alpha blending for background
│   ├── hud_render.c    # MODIFY: Update rendering for transparency
│   ├── hud_navigation.c # MODIFY: Add dirty flag on selection change
│   ├── hud_toggle.c    # MODIFY: Toggle both HUDs together
│   └── hud_format.c    # No change
├── keyguide/           # NEW: Key guide implementation directory
│   ├── keyguide_init.c    # NEW: Initialize key guide, create buffer
│   ├── keyguide_render.c  # NEW: Render key guide with transparency
│   └── keyguide_cleanup.c # NEW: Free key guide resources
├── window/
│   └── window.c        # MODIFY: Update mlx_new_window/image to 1440x900
├── render/
│   ├── camera.c        # MODIFY: Update aspect ratio from 800/600 to 1440/900
│   └── render.c        # MODIFY: Update resolution bounds/comments
└── main.c              # MODIFY: Initialize key guide in startup

tests/
└── visual/             # NEW: Visual test scenes for HUD validation
    ├── test_transparent_hud.rt
    ├── test_keyguide.rt
    └── test_1440x900.rt

Makefile                # MODIFY: Add keyguide/*.c to compilation
```

**Structure Decision**: Single project structure maintained. All HUD-related code organized in feature-specific directories (`src/hud/`, `src/keyguide/`). No architectural changes needed - this is an enhancement to existing rendering system following established patterns.

## Complexity Tracking

> **No violations detected. Constitution compliance verified.**

This feature requires no architectural complexity beyond existing patterns:
- Uses established HUD rendering pattern for key guide
- Follows existing MiniLibX image buffer approach for transparency
- Maintains single-threaded execution model
- Extends existing selection handling with dirty flag (already present)
- Resolution change is configuration update only

All implementations comply with 42 School constraints and project standards.
