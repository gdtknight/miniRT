# Data Model: Keyboard Controls and BVH Enforcement

**Feature**: Keyboard Controls Finalization and BVH Enforcement  
**Date**: 2026-01-01  
**Status**: Phase 1 Design

## Overview

This document defines the data structures, state management, and relationships for the keyboard controls finalization and BVH enforcement feature. The feature primarily modifies existing key code mappings and enforces BVH usage without introducing new complex data structures.

## Entities

### 1. Keyboard Input Mapping

**Description**: Defines the relationship between physical keys and scene manipulation functions.

**Attributes**:
- `keycode` (int): Platform-specific numeric key identifier
- `function` (enum): Semantic function (CAMERA_FORWARD, OBJECT_MOVE_X_POS, etc.)
- `axis` (enum): Affected axis for movement operations (X, Y, Z, NONE)
- `direction` (int): Movement direction (+1, -1, or 0 for non-movement keys)

**Relationships**:
- One keycode maps to exactly one function
- Multiple keycodes may map to same function on different platforms (macOS vs Linux)
- Keys are organized into functional groups: Camera Translation (W/X/A/D/Q/Z), Camera Rotation (E/C/S), Object Movement (R/T/F/G/V/B), UI Controls (H/TAB/Arrow keys)

**Validation Rules**:
- Keycode must be valid for target platform (macOS virtual key code or Linux X11 KeySym)
- No two keys may have conflicting functions
- All required movement axes must have both positive and negative direction keys

**State Transitions**: N/A (static mapping defined at compile time)

**Storage**: Defined as preprocessor constants in window.c, organized by platform using conditional compilation

### 2. Camera State

**Description**: Current position and orientation of the viewpoint, manipulated by camera control keys.

**Attributes**:
- `position` (t_vec3): Camera position in world space (x, y, z coordinates)
- `direction` (t_vec3): Normalized view direction vector
- `initial_position` (t_vec3): Starting position for reset functionality
- `initial_direction` (t_vec3): Starting direction for reset functionality

**Relationships**:
- One camera per scene
- Camera state modified by W/X/A/D/Q/Z keys (translation) and E/C keys (rotation)
- Reset triggered by S key restores initial state
- Referenced by t_render structure for rendering calculations

**Validation Rules**:
- Direction vector must remain normalized (magnitude = 1.0)
- Position values must be finite (no NaN or infinity)
- Rotation operations must preserve direction vector normalization

**State Transitions**:
```
[Initial State] --S key--> [Reset to Initial]
[Any State] --W/X/A/D/Q/Z--> [Translated State]
[Any State] --E/C--> [Rotated State]
```

**Storage**: Part of t_scene structure in scene.h, persists for application lifetime

### 3. Selected Object State

**Description**: The currently active scene object that responds to object movement keys (R/T/F/G/V/B).

**Attributes**:
- `type` (t_obj_type enum): Object type (SPHERE, PLANE, CYLINDER, or NONE)
- `index` (int): Index within type-specific object array
- `position` (t_vec3*): Pointer to object's position in scene (center or reference point)

**Relationships**:
- One selected object at a time (or NONE if no selection)
- Selection changed by [ and ] bracket keys (not part of this feature)
- Object position modified by R/T/F/G/V/B keys
- Position changes trigger scene re-render with BVH rebuild

**Validation Rules**:
- If type != NONE, index must be valid for object array (0 <= index < count)
- Position pointer must be non-NULL when object is selected
- Object type must match one of the defined scene object types

**State Transitions**:
```
[NONE] --select key--> [SPHERE/PLANE/CYLINDER]
[Selected] --R/T/F/G/V/B--> [Selected with moved position]
[Selected] --deselect--> [NONE]
```

**Storage**: Part of t_render structure in window.h, selection state persists until changed

### 4. BVH Acceleration Structure

**Description**: Spatial acceleration structure that optimizes ray-object intersection tests.

**Attributes**:
- `root` (t_bvh_node*): Root node of BVH tree
- `enabled` (int): Always 1 (permanently enabled)
- `total_nodes` (int): Total number of BVH nodes (for metrics)
- `max_depth` (int): Maximum tree depth (for metrics)

**Relationships**:
- One BVH per scene
- Built automatically after scene parsing
- Rebuilt after object movement operations
- Referenced during ray tracing for intersection acceleration

**Validation Rules**:
- `enabled` must always be 1 (non-configurable)
- `root` must be non-NULL after successful BVH build
- `total_nodes` and `max_depth` must be positive after build

**State Transitions**:
```
[Unbuilt] --scene load--> [Built and Enabled]
[Built] --object movement--> [Rebuilt with new geometry]
```

**Storage**: Part of t_scene structure, lifetime tied to scene

### 5. Render State Flags

**Description**: Control flags that manage rendering behavior during interaction.

**Attributes**:
- `dirty` (int): Indicates scene needs re-rendering (1 = needs render, 0 = up to date)
- `low_quality` (int): Enables fast preview rendering during interaction (1 = low quality, 0 = full quality)
- `shift_pressed` (int): Tracks Shift key modifier state for key combinations

**Relationships**:
- Part of t_render structure
- `dirty` flag set by any key that modifies scene (movement keys)
- `low_quality` flag set during continuous movement, cleared on key release
- `shift_pressed` used for TAB+Shift combination (not modified by this feature)

**Validation Rules**:
- All flags are boolean (0 or 1)
- `dirty` must be set whenever scene geometry changes
- `low_quality` cleared before final high-quality render

**State Transitions**:
```
[Clean] --movement key press--> [Dirty + Low Quality]
[Dirty + Low Quality] --key release--> [Dirty + Full Quality]
[Dirty] --render complete--> [Clean]
```

**Storage**: Part of t_render structure, ephemeral state during application runtime

## Entity Relationships Diagram

```
┌─────────────────┐
│   t_render      │
├─────────────────┤
│ dirty           │
│ low_quality     │
│ shift_pressed   │
└────────┬────────┘
         │
         ├──────> ┌─────────────────┐
         │        │   t_selection   │
         │        ├─────────────────┤
         │        │ type            │
         │        │ index           │
         │        └─────────────────┘
         │
         └──────> ┌─────────────────┐
                  │   t_scene       │
                  ├─────────────────┤
                  │ camera          │───> ┌─────────────┐
                  │ spheres[]       │     │ t_camera    │
                  │ planes[]        │     ├─────────────┤
                  │ cylinders[]     │     │ position    │
                  │ bvh*            │     │ direction   │
                  └────────┬────────┘     └─────────────┘
                           │
                           └──────> ┌─────────────────┐
                                    │   t_bvh         │
                                    ├─────────────────┤
                                    │ root            │
                                    │ enabled (=1)    │
                                    │ total_nodes     │
                                    │ max_depth       │
                                    └─────────────────┘
```

## Key Code Organization

### Camera Controls Group
- **Translation**: W (forward), X (backward), A (left), D (right), Q (up world), Z (down world)
- **Rotation**: E (pitch up), C (pitch down)
- **Reset**: S (return to initial state)

### Object Movement Group
- **X-axis**: R (negative/left), T (positive/right)
- **Y-axis**: F (negative/down), G (positive/up)
- **Z-axis**: V (negative/backward), B (positive/forward)

### UI Controls Group (unchanged by this feature)
- **HUD**: H (toggle), TAB (cycle selection), Shift+TAB (reverse cycle)
- **Navigation**: Arrow Up/Down (page navigation), [ / ] (object selection)

## Implementation Notes

### Memory Management
- No dynamic allocation required for key mappings (compile-time constants)
- BVH memory managed by existing spatial.c functions (no changes needed)
- Camera state and selection state use existing structures (no new allocations)

### Performance Considerations
- Key code lookups are O(1) switch/if-else chains (no hash tables needed)
- BVH rebuild triggered only when objects move (not on every key press)
- Low quality rendering during interaction maintains responsiveness

### Cross-Platform Handling
- Key codes defined separately for macOS and Linux using conditional compilation
- All other data structures platform-independent
- Same semantic mapping across platforms (W=forward on all systems)

### Error Handling
- Invalid key codes ignored (no matching case in switch)
- Object movement with no selection is silently ignored
- BVH build failure would prevent rendering (already handled by existing code)

## Data Flow

1. **Key Press Event**:
   ```
   MLX → handle_key() → keycode → switch(keycode) → function dispatch
   ```

2. **Object Movement**:
   ```
   R/T/F/G/V/B key → handle_object_move() → move_selected_object() 
   → modify object position → set dirty flag → trigger low quality render
   → rebuild BVH → final full quality render
   ```

3. **Camera Movement**:
   ```
   W/X/A/D/Q/Z key → handle_camera_move() → modify camera.position
   → set dirty flag → trigger render
   ```

4. **Camera Rotation**:
   ```
   E/C key → handle_camera_pitch() → modify camera.direction
   → normalize direction → set dirty flag → trigger render
   ```

5. **Camera Reset**:
   ```
   S key → restore camera.position/direction from initial state
   → set dirty flag → trigger render
   ```

## Validation Checklist

- [ ] All 6 object movement directions have unique key bindings
- [ ] All 9 camera control keys are defined for both platforms
- [ ] No key conflicts between camera and object controls
- [ ] BVH enabled flag cannot be disabled
- [ ] Camera direction remains normalized after rotation
- [ ] Object movement triggers BVH rebuild
- [ ] Low quality flag set during movement, cleared after
- [ ] Dirty flag set by all scene-modifying keys
- [ ] Invalid key codes are safely ignored
- [ ] Selection state validated before object movement

## Next Steps

- Create contracts/keyboard_layout.h with complete key code definitions
- Update agent context with keyboard control decisions
- Document usage patterns in quickstart.md
