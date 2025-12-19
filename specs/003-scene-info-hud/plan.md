# Implementation Plan: Scene Information HUD

**Branch**: `001-scene-info-hud` | **Date**: 2025-12-19 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/001-scene-info-hud/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Implement a HUD (Heads-Up Display) system that displays parsed RT file information (camera, lighting, objects) in the top-left corner of the miniRT window. The HUD will support object selection navigation (Tab/Shift+Tab), pagination for large object lists, toggling visibility, and maintain performance targets (≤2ms render overhead). The system will use MiniLibX primitives for text rendering and semi-transparent backgrounds while adhering to 42 Norm standards.

## Technical Context

**Language/Version**: C (C99 standard), compiled with cc/gcc  
**Primary Dependencies**: MiniLibX (minilibx-linux on Linux, minilibx-macos on macOS), X11/Xext (Linux), OpenGL/AppKit (macOS)  
**Storage**: N/A (in-memory scene data from RT file parsing)  
**Testing**: Manual testing with RT scene files, valgrind for memory leak detection  
**Target Platform**: Linux (X11) and macOS (Cocoa/AppKit), 42 School environment  
**Project Type**: Single project (C graphics application)  
**Performance Goals**: HUD rendering ≤2ms overhead per frame, updates within 16ms (60fps target)  
**Constraints**: 42 Norm compliance (25-line function limit, naming conventions), no external libraries beyond MLX, static scene data arrays (max 100 objects per type), must use MLX primitives only (mlx_string_put, mlx_pixel_put, or image buffers)  
**Scale/Scope**: Single window application, ~100 objects maximum in scene, HUD displaying 5-10 objects per page with pagination

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### Principle I: Project Structure Standards ✅
- **Status**: PASS
- **Verification**: 
  - HUD source files will be added to `src/hud/` directory (new module)
  - Header file `includes/hud.h` will define HUD API
  - No CI scripts needed for this feature
  - Follows existing project structure pattern (src/{module}/, includes/)

### Principle II: Code Quality Automation ✅
- **Status**: PASS
- **Verification**:
  - All code will be norminette-compliant (42 Norm)
  - No memory leaks (all allocations paired with frees)
  - Build integrated into existing Makefile
  - Manual testing with valgrind for memory verification

### Principle III: Documentation and Wiki Synchronization ✅
- **Status**: PASS
- **Verification**:
  - English spec exists at `specs/001-scene-info-hud/spec.md`
  - Korean translation will be added to `docs/specs/001-scene-info-hud/`
  - Will follow bilingual documentation requirements

### Principle IV: Workflow System ✅
- **Status**: PASS
- **Verification**:
  - Using feature branch `001-scene-info-hud`
  - Following spec → plan → implementation workflow
  - All quality gates applicable (norminette, build, manual test)

### Principle V: Tools and Environment Standards ✅
- **Status**: PASS
- **Verification**:
  - Cross-platform support maintained (Linux/macOS via existing MLX setup)
  - No new external dependencies
  - Test scenes in `scenes/` directory will be used
  - Logs in `logs/` directory as per existing structure

### Principle VI: Bilingual Specification Management ✅
- **Status**: PASS
- **Verification**:
  - English spec completed at `specs/001-scene-info-hud/spec.md`
  - Korean translation will be created at `docs/specs/001-scene-info-hud/spec.md`
  - Both will be synchronized in the same commit

**Overall Gate Status**: ✅ PASS - All constitution principles satisfied. No violations requiring justification.

## Project Structure

### Documentation (this feature)

```text
specs/001-scene-info-hud/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
│   └── hud_api.h        # HUD module API contract
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
includes/
├── minirt.h             # Main header (existing)
├── window.h             # Window/render context (existing)
└── hud.h                # NEW: HUD module public API

src/
├── hud/                 # NEW: HUD implementation module
│   ├── hud_init.c       # HUD initialization and state management
│   ├── hud_render.c     # HUD rendering (background + text)
│   ├── hud_format.c     # Text formatting utilities
│   ├── hud_navigation.c # Object selection and pagination
│   └── hud_toggle.c     # Toggle visibility handling
├── window/
│   └── window.c         # Updated: integrate HUD rendering and key handling
└── [other existing modules]

tests/
└── manual/              # Manual testing with RT scenes
    └── hud_test_scenes/ # Scenes for HUD feature testing
```

**Structure Decision**: Single project structure with new HUD module. The HUD is a self-contained module that integrates with the existing rendering loop and keyboard handling. Module boundaries are clear: HUD owns display logic, window module owns event dispatch and rendering coordination, scene parsing remains unchanged.

## Complexity Tracking

**No violations requiring justification** - this feature follows existing project patterns and conventions without adding unnecessary complexity.

## Phase 0: Research - Status ✅ COMPLETE

**Document**: [research.md](./research.md)

**Research Areas Completed**:
1. ✅ MiniLibX text rendering capabilities (mlx_string_put)
2. ✅ Semi-transparent background rendering (alpha-blended image buffer)
3. ✅ Object selection state management (unified indexing in t_render)
4. ✅ Pagination system design (8 objects per page, arrow key navigation)
5. ✅ Performance optimization strategies (lazy updates, dirty flags, <2ms target)
6. ✅ Keyboard input integration (Tab/Shift+Tab/H/arrows in handle_key)
7. ✅ Text formatting for numerical values (custom ft_format_* functions)

**Key Decisions**:
- Text rendering: `mlx_string_put()` (sufficient, no external deps)
- Background: Pre-created alpha-blended image buffer (one-time overhead)
- Selection: Extend existing `t_render` with unified object indexing
- Pagination: Fixed 8 objects/page, reduces render calls
- Performance: Dirty flag pattern, lazy updates, <2ms budget

**All NEEDS CLARIFICATION items resolved** ✅

---

## Phase 1: Design & Contracts - Status ✅ COMPLETE

**Documents Generated**:
1. ✅ [data-model.md](./data-model.md) - Data structures and relationships
2. ✅ [contracts/hud_api.h](./contracts/hud_api.h) - Public API contract
3. ✅ [quickstart.md](./quickstart.md) - User guide and examples

**Data Model Entities**:
- `t_hud_state` - HUD visibility, pagination, background image
- `t_selection` - Object selection tracking (existing, extended usage)
- `t_render` - Extended with `shift_pressed` flag and embedded `t_hud_state`
- `t_scene` - Read-only data source (no modifications)

**API Contract Functions** (8 public functions):
- `hud_init()` / `hud_cleanup()` - Lifecycle management
- `hud_render()` - Main rendering function
- `hud_toggle()` - Visibility control
- `hud_select_next()` / `hud_select_prev()` - Object navigation
- `hud_page_down()` / `hud_page_up()` - Page navigation
- `hud_mark_dirty()` - Force redraw

**Integration Points**:
- Window module: `handle_key()`, `render_loop()`, `init_window()`, `close_window()`
- Scene parser: No changes needed (read-only access)
- Render system: No changes to core rendering (HUD is separate overlay)
- MiniLibX: `mlx_string_put()`, `mlx_put_image_to_window()`, image buffer management

**Agent Context Updated** ✅:
- GitHub Copilot context file updated with C, MiniLibX, project type

---

## Constitution Re-Check (Post Phase 1)

### Principle I: Project Structure Standards ✅
**Status**: PASS (unchanged)
- Design confirms clean module structure: `src/hud/` with 5 implementation files
- API contract in `includes/hud.h` follows existing header pattern
- No changes to CI scripts needed

### Principle II: Code Quality Automation ✅
**Status**: PASS (unchanged)
- Design ensures 42 Norm compliance via function decomposition (all <25 lines)
- Memory management clearly defined (init/cleanup pairs, no leaks)
- All existing quality gates applicable

### Principle III: Documentation and Wiki Synchronization ✅
**Status**: PASS (unchanged)
- All design documents created in English
- Korean placeholder created at `docs/specs/001-scene-info-hud/spec.md`
- Ready for wiki sync on release

### Principle IV: Workflow System ✅
**Status**: PASS (unchanged)
- Design phase complete following workflow template
- Ready for Phase 2 (task breakdown)
- All artifacts generated as specified

### Principle V: Tools and Environment Standards ✅
**Status**: PASS (unchanged)
- Design maintains cross-platform compatibility (Linux/macOS)
- No new dependencies introduced
- Test strategy defined (manual testing with RT scenes)

### Principle VI: Bilingual Specification Management ✅
**Status**: PASS (unchanged)
- English documentation complete
- Korean spec directory structure created
- Synchronization maintained

**Overall Gate Status**: ✅ PASS - Design validated against all constitution principles.

---

## Next Steps

**Phase 2**: Task breakdown (NOT included in `/speckit.plan` command)

The `/speckit.tasks` command will:
1. Generate `tasks.md` with implementation task breakdown
2. Create subtasks for each source file (hud_init.c, hud_render.c, etc.)
3. Define testing checklist (valgrind, norminette, manual RT file tests)
4. Specify Makefile integration steps
5. Define acceptance criteria for each user story

**Implementation Ready**: ✅
- All technical unknowns resolved
- Data model defined
- API contract specified
- Integration points identified
- Performance targets established
- 42 Norm compliance path clear

---

## Summary

**Branch**: `001-scene-info-hud`  
**Implementation Plan Status**: ✅ COMPLETE (Phases 0-1)

**Deliverables**:
- ✅ plan.md (this file)
- ✅ research.md (7 research areas, all decisions documented)
- ✅ data-model.md (4 core entities, data flow diagrams, memory management)
- ✅ contracts/hud_api.h (8 public functions, 10+ helper functions)
- ✅ quickstart.md (user guide, examples, troubleshooting)
- ✅ Agent context updated (Copilot)
- ✅ Korean spec directory created

**Constitution Compliance**: ✅ ALL GATES PASS

**Performance Targets**:
- HUD rendering: ≤2ms overhead per frame
- Updates: within 16ms (60fps)
- Memory footprint: ~500KB (background image)

**Key Technologies**:
- Language: C (C99)
- Library: MiniLibX (mlx_string_put, image buffers)
- Platform: Linux/macOS (cross-platform)
- Standards: 42 Norm (25-line functions, naming conventions)

**Ready for**: Phase 2 (Task Breakdown via `/speckit.tasks`)

---

**Plan Complete**: 2025-12-19  
**Reviewed By**: Implementation Agent  
**Status**: ✅ READY FOR IMPLEMENTATION
