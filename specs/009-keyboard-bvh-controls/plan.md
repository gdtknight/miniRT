# Implementation Plan: Keyboard Controls Finalization and BVH Enforcement

**Branch**: `009-keyboard-bvh-controls` | **Date**: 2026-01-01 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/009-keyboard-bvh-controls/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

This feature finalizes keyboard controls for object manipulation by replacing the current Numpad-based system with an ergonomic key layout (R/T/F/G/V/B) that keeps the user's hand in a compact region. Additionally, it permanently enforces BVH acceleration structures to ensure optimal rendering performance at all times without requiring user intervention. The implementation maintains existing camera controls and adds new object movement keys while removing all BVH toggle functionality from the user interface.

## Technical Context

**Language/Version**: C (C99), compiled with gcc/clang with -Wall -Wextra -Werror  
**Primary Dependencies**: MiniLibX (school graphics library), libft (custom C library), math.h  
**Storage**: N/A (scene data loaded from .rt files, no persistent storage)  
**Testing**: Manual testing with test scenes, valgrind for memory leak detection  
**Target Platform**: macOS (Darwin) and Linux (X11), cross-platform support required
**Project Type**: Single project (ray tracing application)  
**Performance Goals**: 15+ FPS with 100+ objects, 10x speedup for 1000+ object scenes with BVH  
**Constraints**: 42 School function restrictions (no pthread, no fork, no external parallelization libraries), norminette coding standards, <100ms key press response time  
**Scale/Scope**: Small-scale interactive ray tracer with real-time scene manipulation, 6 new object movement keys + existing 9 camera keys, BVH permanently enabled

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**42 School Constraints Check** (Principle VII):
- ✅ All functions used MUST be from allowed list (libc, math, mlx_*, libft, get_next_line)
  - Current: Uses only standard C library, math.h, and MiniLibX functions
  - No changes needed: New key mappings use existing MLX key event handling
- ✅ pthread functions MUST NOT be used
  - Current: No pthread usage detected in codebase
  - No changes needed: BVH enforcement is configuration only, no new threading
- ✅ fork/pipe/multiprocessing MUST NOT be used  
  - Current: No fork/pipe usage detected
  - No changes needed: Feature is single-process only
- ✅ External parallelization libraries (SIMD, OpenMP, etc.) MUST NOT be used
  - Current: No external libraries beyond allowed list
  - No changes needed: BVH is already implemented using allowed techniques
- ✅ Optimizations MUST use only allowed techniques (algorithmic, caching, math, memory layout)
  - Current: BVH uses spatial partitioning (allowed algorithmic optimization)
  - No changes needed: Only enforcing existing BVH, not adding new optimizations

**Project Structure Standards** (Principle I):
- ✅ Source files properly organized in src/ directory
- ✅ Headers in includes/ directory
- ✅ Documentation in docs/ directory
- ✅ No scattered automation scripts

**Code Quality Automation** (Principle II):
- ✅ All changes MUST pass norminette
- ✅ Build MUST succeed without errors
- ✅ Memory leak check with valgrind MUST show zero leaks
- ⚠️ Note: Keyboard mapping changes require testing on both macOS and Linux platforms

**Documentation and Wiki Synchronization** (Principle III):
- ⚠️ Documentation updates required for new key mappings (FR-013)
- ⚠️ Documentation updates required to remove BVH toggle references (FR-014)

**Workflow System** (Principle IV):
- ✅ Feature developed on dedicated branch: 009-keyboard-bvh-controls
- ✅ PR workflow with automated checks will be followed

**Tools and Environment Standards** (Principle V):
- ✅ Cross-platform support for macOS and Linux maintained
- ✅ Platform-specific key codes already handled in window.c

**Bilingual Specification Management** (Principle VI):
- ⚠️ Korean documentation in docs/specs/ must be created/updated alongside English spec

**GATE STATUS**: ✅ **PASS** - All non-negotiable principles satisfied. Warning items are documentation tasks that will be addressed in Phase 1.

## Project Structure

### Documentation (this feature)

```text
specs/009-keyboard-bvh-controls/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
│   └── keyboard_layout.h  # Key code mappings and constants
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
src/
├── window/
│   └── window.c                 # [MODIFY] Update keyboard handling
│       - Replace KEY_KP_* with R/T/F/G/V/B for object movement
│       - Add KEY_X, KEY_E, KEY_C, KEY_Z for camera controls
│       - Update handle_object_move() function
│       - Update handle_key() dispatcher
├── main.c                       # [MODIFY] Remove BVH toggle logic
│   - Make bvh_enabled permanently true
│   - Remove conditional BVH initialization
├── keyguide/
│   └── keyguide_render.c        # [MODIFY] Update displayed key mappings
│       - Replace Numpad references with R/T/F/G/V/B
│       - Add new camera control keys (X/E/C/Z)

includes/
├── window.h                     # [MODIFY] Add new key code defines
│   - Add KEY_T, KEY_G, KEY_V, KEY_E, KEY_C, KEY_X, KEY_Z
├── spatial.h                    # [NO CHANGE] BVH already defined
└── keyguide.h                   # [NO CHANGE] Interface unchanged

docs/
├── CONTROLS.md                  # [CREATE/UPDATE] Document new keyboard layout
└── specs/
    └── 009-keyboard-bvh-controls/  # [CREATE] Korean translation
        └── spec.md

tests/
└── manual/
    └── keyboard_test_checklist.md  # [CREATE] Testing checklist
```

**Structure Decision**: Single project structure is maintained. The feature requires modifications to existing keyboard input handling in `src/window/window.c`, updates to key code definitions in `includes/window.h`, and changes to the key guide display in `src/keyguide/keyguide_render.c`. BVH enforcement requires minimal changes to `src/main.c` to remove toggle logic. Documentation updates are needed for user-facing keyboard layout information.

## Complexity Tracking

> **No violations detected - table not required**

All constitution principles are satisfied:
- Feature uses only allowed functions from 42 School constraints
- No additional project structure complexity introduced
- Changes are localized to existing keyboard handling code
- BVH acceleration already implemented, only enforcement changed
- Cross-platform support maintained through existing platform detection

## Phase Completion Status

### Phase 0: Outline & Research ✅ COMPLETE

**Deliverable**: `research.md`

**Key Decisions Made**:
1. **Keyboard Layout**: R/T/F/G/V/B for object movement (ergonomic left-hand layout)
2. **Camera Extensions**: Added X/E/C/Z keys for complete 6DOF control
3. **BVH Enforcement**: Remove conditional logic, make permanently enabled
4. **Platform Handling**: Extend existing #ifdef pattern for cross-platform support
5. **Documentation Strategy**: Update in-game keyguide + create CONTROLS.md reference
6. **Testing Approach**: Manual checklist covering all scenarios on both platforms

All technical unknowns resolved. No open questions remaining.

### Phase 1: Design & Contracts ✅ COMPLETE

**Deliverables**:
- `data-model.md` - Entity definitions and state management
- `contracts/keyboard_layout.h` - Complete key code mappings with documentation
- `quickstart.md` - Developer implementation guide with step-by-step instructions
- Agent context updated via `.specify/scripts/bash/update-agent-context.sh copilot`

**Key Artifacts**:
1. **Data Model**: Defined 5 core entities (Keyboard Mapping, Camera State, Selected Object, BVH, Render Flags)
2. **API Contract**: Complete keyboard layout with platform-specific key codes and functional enumerations
3. **Implementation Guide**: 10-step quickstart with code examples and testing checklist

**Constitution Re-check**: ✅ **PASS**
- No new violations introduced by design
- All changes use existing allowed functions
- Documentation requirements identified and addressed in design

### Phase 2: Task Breakdown - NOT STARTED

**Note**: Phase 2 (task breakdown) is handled by the `/speckit.tasks` command, which is a separate workflow step. The implementation plan stops at Phase 1 design completion as per the agent instructions.

## Implementation Readiness Assessment

### ✅ Ready to Implement

**Confidence Level**: HIGH

**Rationale**:
1. All technical decisions documented with clear alternatives analysis
2. Existing codebase already contains 90% of required functionality
3. Changes are surgical and localized to keyboard handling
4. Cross-platform approach proven in existing code
5. BVH implementation already complete, only configuration change needed
6. No new data structures or complex algorithms required

**Estimated Implementation Time**: 2-4 hours (as noted in quickstart.md)

**Risk Assessment**: LOW
- No 42 School constraint violations
- No breaking changes to existing functionality
- Changes are additive (new keys) and modifications (replace old keys)
- BVH already stable, just making it mandatory

### Prerequisites for Implementation

1. ✅ Feature specification complete and approved
2. ✅ Research phase completed with all decisions documented
3. ✅ Design phase completed with contracts defined
4. ✅ Constitution compliance verified
5. ✅ Agent context updated with technology stack
6. ⚠️ Korean translation of spec needs creation (post-implementation)

### Next Actions

**For Developer Implementing This Feature**:
1. Read `quickstart.md` for step-by-step implementation guide
2. Review `contracts/keyboard_layout.h` for complete key code reference
3. Review `data-model.md` for entity relationships and validation rules
4. Follow 10-step implementation process in quickstart
5. Use testing checklist for validation on both platforms
6. Create PR with reference to this implementation plan

**For Project Manager/Reviewer**:
1. Run `/speckit.tasks` command to generate detailed task breakdown
2. Assign implementation tasks to developer
3. Schedule cross-platform testing (macOS + Linux)
4. Plan documentation review for Korean translation
5. Prepare release notes highlighting new keyboard controls

## References

- **Feature Specification**: `specs/009-keyboard-bvh-controls/spec.md`
- **Research Document**: `specs/009-keyboard-bvh-controls/research.md`
- **Data Model**: `specs/009-keyboard-bvh-controls/data-model.md`
- **API Contract**: `specs/009-keyboard-bvh-controls/contracts/keyboard_layout.h`
- **Developer Guide**: `specs/009-keyboard-bvh-controls/quickstart.md`
- **Constitution**: `.specify/memory/constitution.md`
- **Branch**: `009-keyboard-bvh-controls`

## Plan Metadata

- **Plan Version**: 1.0
- **Created**: 2026-01-01
- **Last Updated**: 2026-01-01
- **Status**: Phase 1 Complete - Ready for Task Breakdown
- **Next Command**: `/speckit.tasks` (to be run separately)
