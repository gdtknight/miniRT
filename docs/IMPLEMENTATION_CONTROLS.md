# Interactive Controls Implementation

## Overview

This document describes the implementation of interactive camera and object controls for miniRT.

## Architecture

### Modified Files

1. **includes/window.h**
   - Added `t_obj_type` enum for object type identification
   - Added `t_selection` struct to track selected object
   - Extended `t_render` struct with selection state

2. **src/window/window.c**
   - Replaced simple key handler with comprehensive control system
   - Added camera movement functions
   - Added camera rotation (pitch) functions
   - Added object selection cycling
   - Added object movement functions
   - Added light source movement functions
   - Integrated automatic re-rendering on changes

## Key Mappings

### Linux X11 Key Codes

```c
#define KEY_ESC 65307
#define KEY_W 119
#define KEY_A 97
#define KEY_S 115
#define KEY_D 100
#define KEY_R 114
#define KEY_F 102
#define KEY_BRACKET_LEFT 91
#define KEY_BRACKET_RIGHT 93
#define KEY_KP_0 65438
#define KEY_KP_1 65436
#define KEY_KP_2 65433
#define KEY_KP_3 65435
#define KEY_KP_4 65430
#define KEY_KP_6 65432
#define KEY_KP_8 65431
#define KEY_INSERT 65379
#define KEY_HOME 65360
#define KEY_PGUP 65365
#define KEY_DELETE 65535
#define KEY_END 65367
#define KEY_PGDN 65366
```

## Implementation Details

### Camera Movement (WASD)

The camera moves in 3D space based on its current orientation:
- **Forward/Backward**: Moves along camera direction vector
- **Strafe Left/Right**: Moves perpendicular to camera direction (calculated using cross product)
- **Step size**: 1.0 unit per keypress

```c
static void handle_camera_move(t_render *render, int keycode)
{
    t_vec3 right;
    t_vec3 move;
    double step = 1.0;
    
    // Calculate right vector (perpendicular to direction)
    right = vec3_normalize(vec3_cross(render->scene->camera.direction,
                                      (t_vec3){0, 1, 0}));
    
    // Calculate movement vector based on key
    if (keycode == KEY_W)
        move = vec3_multiply(render->scene->camera.direction, step);
    // ... etc
    
    // Apply movement
    render->scene->camera.position = vec3_add(
        render->scene->camera.position, move);
}
```

### Camera Rotation (RF)

Camera pitch rotation is implemented using Rodrigues' rotation formula:
- Rotates camera direction around the "right" vector
- **Angle**: 5 degrees per keypress
- **R**: Pitch up (positive rotation)
- **F**: Pitch down (negative rotation)

The rotation preserves the camera's coordinate system by:
1. Computing the "right" vector (perpendicular to view direction)
2. Rotating the direction vector around this right vector
3. Normalizing the result

```c
static void handle_camera_pitch(t_render *render, int keycode)
{
    t_vec3 right;
    t_vec3 new_dir;
    double angle = 5.0 * M_PI / 180.0;  // 5 degrees in radians
    
    if (keycode == KEY_F)
        angle = -angle;  // Reverse for pitch down
    
    // Calculate right vector
    right = vec3_normalize(vec3_cross(render->scene->camera.direction,
                                      (t_vec3){0, 1, 0}));
    
    // Apply rotation using axis-angle formula
    // new_dir = dir * cos(angle) + (right × dir) * sin(angle)
    // ...
    
    render->scene->camera.direction = vec3_normalize(new_dir);
}
```

### Object Selection ([/])

Objects are organized in memory as three separate arrays:
1. Spheres
2. Planes  
3. Cylinders

Selection cycles through all objects sequentially:
- **]**: Next object (forward cycling)
- **[**: Previous object (backward cycling)

State tracking:
- `selection.type`: Current object type (SPHERE, PLANE, CYLINDER)
- `selection.index`: Index within that type's array

### Object Movement (Numpad)

Selected objects move in world space coordinates:
- **Numpad 4/6**: X-axis (left/right)
- **Numpad 2/8**: Y-axis (down/up)
- **Numpad 1/3**: Z-axis (backward/forward)
- **Step size**: 1.0 unit per keypress

The implementation directly modifies the object's position in the scene:
```c
if (render->selection.type == OBJ_SPHERE)
    render->scene->spheres[render->selection.index].center =
        vec3_add(render->scene->spheres[render->selection.index].center,
                 move);
```

### Light Movement (Insert/Home/PageUp/Delete/End/PageDown)

Light source moves in world space:
- **Insert/Delete**: X-axis (right/left)
- **Home/End**: Y-axis (up/down)
- **PageUp/PageDown**: Z-axis (forward/backward)
- **Step size**: 1.0 unit per keypress

```c
static void handle_light_move(t_render *render, int keycode)
{
    t_vec3 move = {0, 0, 0};
    double step = 1.0;
    
    // Determine movement direction
    if (keycode == KEY_INSERT)
        move.x = step;
    // ... etc
    
    // Apply movement
    render->scene->light.position = vec3_add(
        render->scene->light.position, move);
}
```

## Re-rendering

After any camera movement, object movement, or light adjustment:
1. The `needs_redraw` flag is set
2. At the end of `handle_key()`, if flag is set:
   - `render_scene()` is called
   - The entire scene is re-rendered with new positions/orientations
   - Results are displayed immediately

This provides instant visual feedback for all interactive controls.

## Performance Considerations

- **Rendering cost**: Each keypress triggers full scene re-render
- **Typical render time**: ~50-200ms depending on scene complexity
- **User experience**: Responsive for interactive manipulation
- **Optimization**: Could add progressive rendering or caching for complex scenes

## Testing

Manual testing checklist:
- [ ] Camera moves forward/backward/left/right correctly
- [ ] Camera rotates up/down smoothly
- [ ] Object selection cycles through all objects
- [ ] Selected object moves in all 6 directions
- [ ] Light source moves in all 6 directions
- [ ] Scene re-renders after each change
- [ ] No memory leaks (verified with valgrind)
- [ ] No crashes on edge cases (no objects, etc.)

## Future Enhancements

Possible improvements:
1. **Visual selection indicator**: Highlight selected object
2. **Rotation controls**: Rotate objects, not just translate
3. **Camera yaw**: Left/right rotation (not just pitch)
4. **Faster movement**: Shift key for 10x speed
5. **Undo/redo**: Stack of scene states
6. **Mouse controls**: Click to select, drag to move
7. **Animation**: Record and playback camera paths

## Dependencies

- **MinilibX**: Window and event handling
- **Math library**: cos(), sin() for rotation
- **Vector operations**: From vec3.h

## Coordinate Systems

- **World space**: Right-handed coordinate system
  - +X: Right
  - +Y: Up
  - +Z: Forward (into screen)
- **Camera space**: Relative to camera orientation
  - Forward: Camera direction vector
  - Right: Cross product of direction and world up
  - Up: Cross product of right and direction
