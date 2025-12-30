# Implementation Plan: Cross-Platform Keyboard Support

**Branch**: `006-cross-platform-keyboard` | **Date**: 2025-12-30 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/006-cross-platform-keyboard/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Enable miniRT to work with keyboard controls on both Ubuntu and macOS by implementing platform-specific keycode definitions using conditional compilation. The current implementation uses Linux/X11 keycodes exclusively, causing keyboard controls to malfunction on macOS. The solution uses preprocessor directives (`__APPLE__` for macOS, `__linux__` for Linux) to select appropriate keycode constants at compile time while maintaining identical key handling logic across both platforms.

## Technical Context

**Language/Version**: C (C99 standard), compiled with gcc/clang  
**Primary Dependencies**: MiniLibX (X11 for Linux, OpenGL/AppKit for macOS), libm (math library)  
**Storage**: N/A (no persistent storage for keyboard handling)  
**Testing**: Manual testing on Ubuntu 20.04+ and macOS 10.14+, automated build verification  
**Target Platform**: Linux (Ubuntu 20.04+) and macOS (10.14+ Mojave)  
**Project Type**: Single application (ray tracer with GUI)  
**Performance Goals**: Immediate keyboard response (<16ms latency for interactive controls)  
**Constraints**: Must comply with 42 School function constraints (no pthread, no external libraries), norminette coding standard compliance  
**Scale/Scope**: 30+ keyboard controls across 5 functional categories (camera, object, lighting, application, rendering)

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### Principle I: Project Structure Standards ✅ PASS (Re-verified)
- Changes limited to `src/window/window.c` (keycode definitions section)
- No new files required in source tree
- Documentation properly organized in `specs/006-cross-platform-keyboard/`
- All artifacts (research.md, data-model.md, contracts/, quickstart.md) created
- Maintains existing organization perfectly

### Principle II: Code Quality Automation ✅ PASS (Re-verified)
- Changes validated by norminette (preprocessor directives compliant)
- Build verification on both Ubuntu and macOS confirmed in quickstart.md
- No memory impact (only compile-time constants)
- All existing quality gates apply and will pass
- Zero runtime overhead from changes

### Principle III: Documentation and Wiki Synchronization ✅ PASS (Re-verified)
- Complete documentation set created in specs/006-cross-platform-keyboard/
- Korean specification synchronization noted in plan
- No wiki synchronization required for internal refactoring
- All documentation follows standard template structure

### Principle IV: Workflow System ✅ PASS (Re-verified)
- Standard PR workflow applies
- Must pass norminette + build + test on both platforms
- Platform-specific testing procedures defined in quickstart.md
- Clear validation checklist provided for review

### Principle V: Tools and Environment Standards ✅ PASS (Re-verified)
- Already supports Linux and macOS (Makefile handles platform detection)
- Uses existing MiniLibX dependency (platform-specific versions)
- No new dependencies required
- Test scenes remain unchanged
- Comprehensive testing guide provided in quickstart.md

### Principle VI: Bilingual Specification Management ✅ PASS (Re-verified)
- English spec exists at `specs/006-cross-platform-keyboard/spec.md`
- Korean translation location documented: `docs/specs/006-cross-platform-keyboard/spec.md`
- Will be synchronized in same PR
- Content semantically equivalent between languages

### Principle VII: 42 School Function Constraints ✅ PASS (Re-verified)

**Allowed Functions Check**:
- ✅ Uses only preprocessor directives (`#ifdef`, `#define`) - compile-time only
- ✅ Uses only MiniLibX functions (`mlx_*`) - already allowed
- ✅ No new function calls required
- ✅ Maintains existing allowed function usage
- ✅ All implementation in contracts/keycodes.h uses only standard C preprocessor

**Prohibited Functions Check**:
- ✅ No pthread functions used
- ✅ No fork/pipe/multiprocessing used
- ✅ No external libraries required
- ✅ No SIMD or parallelization techniques
- ✅ No prohibited optimization techniques introduced

**Implementation Approach Validation**:
- Uses compile-time conditional compilation (`#ifdef __APPLE__`, `#ifdef __linux__`)
- Defines platform-specific integer constants for keycodes
- No runtime overhead or additional function calls
- Purely preprocessor-based solution compliant with all 42 constraints
- Implementation reviewed in research.md Q2 and Q4
- Contract defined in contracts/keycodes.h confirms compliance

**Post-Design Confirmation**: All Phase 1 artifacts (research.md, data-model.md, contracts/keycodes.h, quickstart.md) reviewed. No violations detected. Implementation approach fully compliant with 42 School constraints.

## Project Structure

### Documentation (this feature)

```text
specs/006-cross-platform-keyboard/
├── spec.md              # Feature specification (English)
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output - Platform keycode research
├── data-model.md        # Phase 1 output - Keycode mapping model
├── quickstart.md        # Phase 1 output - Testing guide for both platforms
└── contracts/           # Phase 1 output - Keycode definition contracts
    └── keycodes.h       # Platform-specific keycode constant definitions
```

### Source Code (repository root)

```text
src/window/
├── window.c             # MODIFIED: Platform-specific keycode definitions (lines 37-71)
└── (other files)        # UNCHANGED

includes/
└── window.h             # UNCHANGED (no API changes)

Makefile                 # UNCHANGED (already handles platform detection)

docs/specs/006-cross-platform-keyboard/
└── spec.md              # Korean translation (synchronized)
```

**Structure Decision**: Minimal change approach - modify only the keycode definition section in `src/window/window.c` (lines 37-71). No new files added to source tree. Use conditional compilation (`#ifdef __APPLE__` / `#ifdef __linux__`) to select appropriate keycode sets. All keyboard handler functions remain unchanged, ensuring behavioral consistency across platforms. The existing Makefile already detects the platform and selects correct MiniLibX version, so no build system changes required.

## Complexity Tracking

> **No violations - section not applicable**

All Constitution principles pass without exceptions. No additional complexity introduced beyond the minimal keycode definition changes.
