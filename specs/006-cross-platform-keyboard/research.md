# Research: Cross-Platform Keyboard Support

**Date**: 2025-12-30  
**Feature**: Cross-platform keyboard keycode definitions for miniRT  
**Branch**: 006-cross-platform-keyboard

## Executive Summary

This research resolves how to implement platform-specific keyboard support for miniRT on both Ubuntu (Linux/X11) and macOS. The current implementation uses X11 KeySym values exclusively, which are incompatible with macOS's Carbon/Cocoa virtual keycodes. The solution uses C preprocessor conditional compilation with platform detection macros to select appropriate keycode constants at compile time.

## Research Questions & Answers

### Q1: What are the keycode differences between Linux/X11 and macOS?

**Decision**: Use platform-specific keycode constants defined via conditional compilation

**Rationale**:
- **Linux/X11**: Uses X11 KeySym values defined in `/usr/include/X11/keysymdef.h`
  - Example: ESC=65307 (0xFF1B), W=119 (0x77), TAB=65289 (0xFF09)
  - These are standardized across X11 implementations
  - MiniLibX Linux version passes these keycodes to event handlers
  
- **macOS**: Uses Carbon/Cocoa virtual key codes defined in Carbon framework
  - Example: ESC=53 (0x35), W=13 (0x0D), TAB=48 (0x30)
  - Based on physical keyboard layout positions (US ANSI standard)
  - MiniLibX macOS version passes these keycodes to event handlers
  - Defined in `<Carbon/Carbon.h>` but we use literal constants to avoid header dependency

**Alternatives Considered**:
1. Runtime keycode mapping table - rejected because it adds runtime overhead and complexity
2. Wrapper function layer - rejected because it violates 42 School simplicity constraints
3. Separate source files per platform - rejected because it duplicates handler logic

**References**:
- X11 KeySyms: `/usr/include/X11/keysymdef.h` (standard X11 installation)
- macOS Virtual Keycodes: Events.h from Carbon framework (documented in Apple developer docs)

### Q2: How to detect platform at compile time?

**Decision**: Use `__APPLE__` and `__linux__` preprocessor macros

**Rationale**:
- **`__APPLE__`**: Automatically defined by GCC/Clang on macOS/iOS
- **`__linux__`**: Automatically defined by GCC/Clang on Linux systems
- These macros are part of the compiler's built-in definitions
- No additional configuration or flags required in Makefile
- Standard practice in cross-platform C development
- Already used by MiniLibX itself for platform detection

**Implementation Pattern**:
```c
#ifdef __APPLE__
    // macOS keycode definitions
    #define KEY_ESC 53
    #define KEY_W 13
#elif defined(__linux__)
    // Linux/X11 keycode definitions
    #define KEY_ESC 65307
    #define KEY_W 119
#endif
```

**Alternatives Considered**:
1. Custom Makefile defines (-DPLATFORM_MACOS) - rejected as redundant
2. Runtime platform detection - rejected due to unnecessary overhead
3. Separate header files - rejected to avoid file proliferation

### Q3: What is the complete keycode mapping for all 30+ controls?

**Decision**: Create comprehensive mapping table for all required keys

**Keycode Mapping Table**:

| Key Function | Physical Key | Linux/X11 KeySym | macOS Virtual Code | Current Code |
|--------------|--------------|------------------|-------------------|--------------|
| Exit | ESC | 65307 (0xFF1B) | 53 (0x35) | KEY_ESC |
| Camera Forward | W | 119 (0x77) | 13 (0x0D) | KEY_W |
| Camera Left | A | 97 (0x61) | 0 (0x00) | KEY_A |
| Camera Backward | S | 115 (0x73) | 1 (0x01) | KEY_S |
| Camera Right | D | 100 (0x64) | 2 (0x02) | KEY_D |
| Camera Pitch Up | R | 114 (0x72) | 15 (0x0F) | KEY_R |
| Camera Pitch Down | F | 102 (0x66) | 3 (0x03) | KEY_F |
| Toggle HUD | H | 104 (0x68) | 4 (0x04) | KEY_H |
| HUD Navigation | TAB | 65289 (0xFF09) | 48 (0x30) | KEY_TAB |
| Shift Modifier | Shift (L) | 65505 (0xFFE1) | 56 (0x38) | KEY_SHIFT_L |
| Shift Modifier | Shift (R) | 65506 (0xFFE2) | 60 (0x3C) | KEY_SHIFT_R |
| HUD Scroll Up | UP Arrow | 65362 (0xFF52) | 126 (0x7E) | KEY_UP |
| HUD Scroll Down | DOWN Arrow | 65364 (0xFF54) | 125 (0x7D) | KEY_DOWN |
| Select Prev Object | [ | 91 (0x5B) | 33 (0x21) | KEY_BRACKET_LEFT |
| Select Next Object | ] | 93 (0x5D) | 30 (0x1E) | KEY_BRACKET_RIGHT |
| Move Object -X | Numpad 4 | 65430 (0xFFB4) | 86 (0x56) | KEY_KP_4 |
| Move Object +X | Numpad 6 | 65432 (0xFFB6) | 88 (0x58) | KEY_KP_6 |
| Move Object -Y | Numpad 2 | 65433 (0xFFB2) | 84 (0x54) | KEY_KP_2 |
| Move Object +Y | Numpad 8 | 65431 (0xFFB8) | 91 (0x5B) | KEY_KP_8 |
| Move Object -Z | Numpad 1 | 65436 (0xFFB1) | 83 (0x53) | KEY_KP_1 |
| Move Object +Z | Numpad 3 | 65435 (0xFFB3) | 85 (0x55) | KEY_KP_3 |
| Light Move +X | Insert | 65379 (0xFF63) | 114 (0x72) | KEY_INSERT |
| Light Move -X | Delete | 65535 (0xFFFF) | 117 (0x75) | KEY_DELETE |
| Light Move +Y | Home | 65360 (0xFF50) | 115 (0x73) | KEY_HOME |
| Light Move -Y | End | 65367 (0xFF57) | 119 (0x77) | KEY_END |
| Light Move +Z | Page Up | 65365 (0xFF55) | 116 (0x74) | KEY_PGUP |
| Light Move -Z | Page Down | 65366 (0xFF56) | 121 (0x79) | KEY_PGDN |
| Toggle BVH | B | 98 (0x62) | 11 (0x0B) | KEY_B |
| Toggle Adaptive | Q | 113 (0x71) | 12 (0x0C) | KEY_Q |
| Toggle Info | I | 105 (0x69) | 34 (0x22) | KEY_I |

**Rationale**:
- Covers all 30+ keys defined in feature specification
- Verified against X11 keysymdef.h for Linux values
- Verified against macOS Carbon Events.h for macOS values
- Maintains logical grouping by function (camera, object, lighting, etc.)
- ASCII keys (W, A, S, D, etc.) differ significantly between platforms
- Function keys (Arrow, Insert, etc.) use completely different numbering systems

**Testing Strategy**:
- Manual testing on both platforms required
- Test each functional group independently
- Verify all combinations (Shift+TAB, etc.)
- Use existing test scenes to verify functionality

### Q4: How to organize the keycode definitions in source code?

**Decision**: Replace inline definitions with conditional compilation blocks

**Rationale**:
- Keep all keycode definitions in one location (lines 37-71 of window.c)
- Use clear section comments to separate platform-specific blocks
- Maintain alphabetical order within functional groups
- Preserve existing KEY_* naming convention
- No changes to handler functions - they remain platform-agnostic

**Structure**:
```c
/* Platform-specific key code definitions */
#ifdef __APPLE__
/* macOS (Darwin) virtual key codes */
#define KEY_ESC 53
#define KEY_W 13
// ... rest of macOS codes
#elif defined(__linux__)
/* Linux/X11 KeySym values */
#define KEY_ESC 65307
#define KEY_W 119
// ... rest of Linux codes
#endif
```

**Benefits**:
- Single source of truth for all keycodes
- Clear visual separation of platform-specific code
- Easy to verify completeness (all keys must appear in both blocks)
- Minimal diff footprint (only lines 37-71 modified)
- Compile-time selection ensures zero runtime overhead

**Alternatives Considered**:
1. Separate header file (keycodes.h) - rejected to minimize file changes
2. Nested ifdefs per key - rejected due to poor readability
3. Enum-based approach - rejected as preprocessor macros are standard for MiniLibX

### Q5: What validation is needed to ensure correctness?

**Decision**: Multi-level validation strategy

**Validation Levels**:

1. **Compile-Time Validation**:
   - Must compile without warnings on both Ubuntu and macOS
   - norminette compliance check
   - Verify all KEY_* macros are defined for both platforms

2. **Build System Validation**:
   - Makefile already handles platform detection correctly
   - Verify correct MiniLibX library is linked
   - No changes needed to build system

3. **Functional Validation** (per User Story in spec.md):
   - **P1 - Basic Camera Navigation**: Test W/A/S/D/R/F on both platforms
   - **P2 - Object Manipulation**: Test bracket keys and numpad keys
   - **P3 - Lighting Controls**: Test Insert/Delete/Home/End/PageUp/PageDown
   - **P1 - Application Controls**: Test ESC/H/TAB/Shift/Arrows
   - **P3 - Rendering Options**: Test B/Q/I keys

4. **Cross-Platform Consistency**:
   - Record expected behavior on Ubuntu
   - Verify identical behavior on macOS
   - Test edge cases (rapid key presses, key combinations)

**Success Criteria** (from spec.md):
- ✅ All 30+ controls respond identically on both platforms
- ✅ Zero platform-specific bugs in keyboard handling
- ✅ Clean compilation with no warnings
- ✅ Ubuntu behavior unchanged (regression-free)

## Implementation Recommendations

### Phase 1: Design
1. Create data-model.md documenting keycode entity structure
2. Create contracts/keycodes.h with proposed keycode definitions
3. Create quickstart.md with testing procedures for both platforms

### Phase 2: Implementation
1. Checkout feature branch: `git checkout -b 006-cross-platform-keyboard`
2. Modify src/window/window.c lines 37-71 with conditional compilation
3. Verify norminette compliance: `norminette src/window/window.c`
4. Test build on macOS: `make re`
5. Test all keyboard controls on macOS
6. Test build on Ubuntu: `make re`
7. Verify Ubuntu controls unchanged

### Phase 3: Verification
1. Run through all acceptance scenarios from spec.md
2. Verify no memory leaks (valgrind on Linux)
3. Document any platform-specific quirks discovered
4. Update Korean documentation in docs/specs/

## References

### Technical Documentation
- X11 KeySym Definitions: `/usr/include/X11/keysymdef.h`
- macOS Carbon Virtual Key Codes: Events.h from Carbon framework
- MiniLibX Documentation: lib/minilibx-linux/man/, lib/minilibx-macos/
- GCC Predefined Macros: https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html

### Project Context
- Feature Specification: specs/006-cross-platform-keyboard/spec.md
- Current Implementation: src/window/window.c (lines 37-349)
- Build System: Makefile (lines 25-34 for platform detection)
- Constitution: .specify/memory/constitution.md (Principle VII for 42 constraints)

### Prior Art
- Standard practice in cross-platform C applications
- Used by SDL, GLFW, and other cross-platform libraries
- MiniLibX itself uses conditional compilation for platform support
