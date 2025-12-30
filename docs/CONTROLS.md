# miniRT Interactive Controls

## Camera Movement (1st Person View)

### Translation (WASD Keys)
- **W**: Move forward (in the direction camera is looking)
- **S**: Move backward (opposite to camera direction)
- **A**: Move left (strafe left)
- **D**: Move right (strafe right)

### Rotation (RF Keys)
- **R**: Pitch up (look upward)
- **F**: Pitch down (look downward)

Movement speed: 1.0 unit per key press
Rotation speed: 5 degrees per key press

## Object Selection

### Selection Keys
- **[**: Select previous object
- **]**: Select next object

Selection cycles through all objects in the scene:
1. Spheres (first to last)
2. Planes (first to last)
3. Cylinders (first to last)
4. Back to spheres

### Object Movement (Keyboard Numpad)
- **Numpad 4**: Move left (X-)
- **Numpad 6**: Move right (X+)
- **Numpad 2**: Move down (Y-)
- **Numpad 8**: Move up (Y+)
- **Numpad 1**: Move backward (Z-)
- **Numpad 3**: Move forward (Z+)

Movement step: 1.0 unit per key press

## Light Source Control

### Light Position (Insert/Home/Page Keys)
- **Insert**: Move light right (X+)
- **Delete**: Move light left (X-)
- **Home**: Move light up (Y+)
- **End**: Move light down (Y-)
- **Page Up**: Move light forward (Z+)
- **Page Down**: Move light backward (Z-)

Movement step: 1.0 unit per key press

## General Controls
- **ESC**: Exit program

## Notes
- Scene is automatically re-rendered after camera movement, object movement, or light adjustment
- Object selection does not trigger re-render (only visual indicator would be shown if implemented)
- All movements are in world space coordinates

## Testing the Controls

Try these sequences with `scenes/test_all_objects.rt`:

1. **Camera Movement**: Press W several times to move forward toward the objects
2. **Camera Rotation**: Press R to look up, F to look down
3. **Object Selection**: Press ] to cycle through objects
4. **Object Movement**: Press Numpad 8 to move selected object upward
5. **Light Control**: Press Home to move light source upward
