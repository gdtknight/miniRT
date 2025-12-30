# Data Model: Cross-Platform Keyboard Support

**Date**: 2025-12-30  
**Feature**: Cross-platform keyboard keycode definitions  
**Branch**: 006-cross-platform-keyboard

## Overview

This document defines the data structures and entities for cross-platform keyboard support. Since this feature involves compile-time constants rather than runtime data structures, the "data model" describes the logical organization of keycode definitions and their relationships.

## Entities

### 1. Keycode Constant (Compile-Time Entity)

**Description**: Platform-specific integer constant representing a physical keyboard key.

**Attributes**:
- **Name**: Symbolic identifier (e.g., `KEY_ESC`, `KEY_W`, `KEY_TAB`)
- **Value (Linux)**: X11 KeySym integer value (e.g., 65307 for ESC)
- **Value (macOS)**: Carbon virtual keycode integer value (e.g., 53 for ESC)
- **Physical Key**: Human-readable key label (e.g., "ESC", "W", "Tab")
- **Function Group**: Functional category (Camera, Object, Lighting, Application, Rendering)

**Lifecycle**:
- Defined at compile time via preprocessor `#define` directives
- Selected based on platform detection macros (`__APPLE__` or `__linux__`)
- Immutable once compiled
- No runtime allocation or deallocation

**Validation Rules**:
- Each keycode constant MUST be defined for both platforms
- Values MUST be positive integers
- Names MUST follow `KEY_*` naming convention
- No duplicate names within same platform
- Values should correspond to actual hardware keycodes

**State Transitions**: N/A (compile-time constants have no state)

**Example**:
```c
// Linux/X11
#define KEY_ESC 65307

// macOS
#define KEY_ESC 53
```

### 2. Functional Key Group (Logical Entity)

**Description**: Logical grouping of related keyboard controls by their function in the application.

**Groups**:

#### Camera Control Group
- **Keys**: W, A, S, D (movement), R, F (pitch rotation)
- **Purpose**: Navigate camera position and orientation in 3D space
- **State Impact**: Modifies `t_scene.camera.position` and `t_scene.camera.direction`
- **Priority**: P1 (essential for basic usage)

#### Object Manipulation Group
- **Keys**: [ (previous), ] (next) for selection; Numpad 1-8 for movement
- **Purpose**: Select and move objects in the scene
- **State Impact**: Modifies `t_render.selection` and object positions
- **Priority**: P2 (important for scene editing)

#### Lighting Control Group
- **Keys**: Insert, Delete, Home, End, Page Up, Page Down
- **Purpose**: Adjust light source position
- **State Impact**: Modifies `t_scene.light.position`
- **Priority**: P3 (enhancement for scene quality)

#### Application Control Group
- **Keys**: ESC (exit), H (HUD toggle), TAB (navigation), Shift (modifier), Arrow Up/Down
- **Purpose**: Control application state and interface
- **State Impact**: Various (exit, `t_render.hud.visible`, page navigation, etc.)
- **Priority**: P1 (essential for basic usage)

#### Rendering Options Group
- **Keys**: B (BVH), Q (adaptive sampling), I (info display)
- **Purpose**: Toggle rendering optimizations and debug info
- **State Impact**: Modifies `t_scene.render_state` flags
- **Priority**: P3 (developer/power user features)

**Relationships**:
- Each functional group maps to specific handler functions in window.c
- Groups share common behavior patterns (e.g., all camera keys trigger low-quality rendering)
- Some keys (Shift) modify behavior of other keys (Shift+TAB for reverse navigation)

### 3. Platform Configuration (Compile-Time Entity)

**Description**: Platform-specific build configuration determined at compile time.

**Attributes**:
- **Platform Identifier**: `__APPLE__` or `__linux__` preprocessor macro
- **Keycode Set**: Complete set of KEY_* definitions for the platform
- **MiniLibX Variant**: Platform-specific MiniLibX library (minilibx-macos or minilibx-linux)
- **Compiler**: clang (macOS) or gcc/clang (Linux)

**Detection Method**:
- Automatic via compiler-defined macros
- No user configuration required
- Verified at compile time (compilation fails if no platform detected)

**Impact**:
- Determines which keycode definitions are active
- Selects correct MiniLibX library in Makefile
- Ensures platform-appropriate event handling

## Relationships

### Keycode → Handler Function (Many-to-One)

Multiple keycode constants map to the same handler function:

```
KEY_W, KEY_A, KEY_S, KEY_D → handle_camera_move()
KEY_R, KEY_F → handle_camera_pitch()
KEY_BRACKET_LEFT, KEY_BRACKET_RIGHT → handle_object_selection()
KEY_KP_1..KEY_KP_8 → handle_object_move()
KEY_INSERT, KEY_DELETE, KEY_HOME, KEY_END, KEY_PGUP, KEY_PGDN → handle_light_move()
```

This relationship is defined in `handle_key()` function via if-else chains or switch statements.

### Platform → Keycode Set (One-to-Many)

Each platform has its own complete set of keycode definitions:

```
__APPLE__ → {KEY_ESC=53, KEY_W=13, KEY_A=0, ...}
__linux__ → {KEY_ESC=65307, KEY_W=119, KEY_A=97, ...}
```

Only one set is active at compile time based on platform detection.

### Functional Group → Rendering State (One-to-Many)

Some functional groups trigger rendering state changes:

```
Camera Control Group → low_quality=1, dirty=1
Object Manipulation Group → low_quality=1, dirty=1
Lighting Control Group → low_quality=1, dirty=1
Rendering Options Group → dirty=1 only
```

## Data Flow

### Compile Time

1. **Platform Detection**:
   - Compiler defines `__APPLE__` or `__linux__`
   - Preprocessor evaluates conditional directives
   
2. **Keycode Selection**:
   - Appropriate `#define` block is included
   - Platform-specific keycode constants are defined
   - Other platform's definitions are excluded

3. **Binary Generation**:
   - Constants are compiled into the executable
   - No runtime platform checks needed
   - Zero overhead for platform detection

### Runtime

1. **Key Press Event**:
   - MiniLibX receives OS keyboard event
   - MiniLibX converts to platform-specific keycode
   - Calls `handle_key(keycode, render)` with the keycode

2. **Keycode Matching**:
   - `handle_key()` compares keycode against KEY_* constants
   - Matches key to appropriate handler function
   - Handler function executes (moves camera, selects object, etc.)

3. **State Update**:
   - Handler modifies relevant state (camera position, selection, etc.)
   - Sets dirty flag and/or low_quality flag
   - Next render loop iteration updates display

## Constraints & Invariants

### Compile-Time Constraints

1. **Completeness**: Every KEY_* constant MUST be defined for both platforms
2. **Consistency**: KEY_* names MUST be identical across platforms (only values differ)
3. **Non-Overlap**: Within a platform, each physical key MUST map to exactly one constant
4. **Positive Values**: All keycode values MUST be positive integers

### Runtime Invariants

1. **Deterministic Mapping**: Same physical key press always produces same keycode
2. **Platform Consistency**: Same KEY_* constant always represents same physical key on same platform
3. **Handler Stability**: Handler functions work identically regardless of platform
4. **State Coherence**: Keyboard actions produce identical state changes on both platforms

### 42 School Constraints

1. **No Runtime Configuration**: All platform detection at compile time only
2. **No External Libraries**: Uses only standard C preprocessor and compiler features
3. **No Function Calls for Platform Detection**: Pure preprocessor directives only
4. **Simplicity**: Minimal code changes, maximum clarity

## Schema Definition

Since this is a compile-time configuration rather than runtime data, the "schema" is expressed as C preprocessor directives:

```c
/* Platform-specific keycode definitions */
#ifdef __APPLE__
    /* macOS keycode set */
    #define KEY_ESC 53
    #define KEY_W 13
    /* ... 30+ more keycodes ... */
#elif defined(__linux__)
    /* Linux/X11 keycode set */
    #define KEY_ESC 65307
    #define KEY_W 119
    /* ... 30+ more keycodes ... */
#else
    #error "Unsupported platform - requires macOS or Linux"
#endif
```

**Schema Properties**:
- **Mutual Exclusivity**: Only one platform's definitions active at compile time
- **Completeness Check**: Compilation fails if platform not detected
- **Type Safety**: Preprocessor constants are type-checked by C compiler as integers
- **Scope**: Definitions are file-scoped within window.c

## Testing Implications

### Compile-Time Testing

- **Platform Detection**: Verify correct keycode set is selected on each platform
- **Completeness**: Ensure all KEY_* constants are defined on both platforms
- **Build Success**: Compilation must succeed without warnings on both platforms

### Runtime Testing

- **Functional Consistency**: Same physical key produces same application behavior
- **State Verification**: Camera/object/light states change identically
- **Edge Cases**: Key combinations (Shift+TAB), rapid presses, simultaneous presses

### Regression Testing

- **Ubuntu Behavior**: Verify Linux behavior is completely unchanged
- **macOS Behavior**: Verify macOS behavior matches Linux behavior exactly
- **Cross-Platform**: Test identical scene files on both platforms with identical results

## Migration Notes

### Current State (Before)

- All keycode definitions use Linux/X11 values
- macOS keycodes are incorrect, causing controls to malfunction
- No platform detection in keycode definitions

### Target State (After)

- Keycode definitions separated by platform with `#ifdef` directives
- Correct values for both Linux/X11 and macOS
- Automatic platform detection via compiler macros
- Identical behavior on both platforms

### Migration Path

1. Replace existing keycode definitions (lines 37-71 in window.c)
2. Add platform detection directives (`#ifdef __APPLE__` / `#elif defined(__linux__)`)
3. Insert macOS keycode values in __APPLE__ block
4. Keep Linux keycode values in __linux__ block
5. No changes to handler functions or logic

**Risk**: Incorrect macOS keycode values would cause wrong keys to trigger actions. Mitigated by comprehensive testing on macOS hardware.

**Rollback**: Git revert restores original Linux-only definitions.
