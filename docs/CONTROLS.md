# miniRT Interactive Controls

## Camera Movement (1st Person View)

### Translation
- **W**: Move forward (in the direction camera is looking)
- **X**: Move backward (opposite to camera direction)
- **A**: Move left (strafe left)
- **D**: Move right (strafe right)
- **Q**: Move up (world space Y+)
- **Z**: Move down (world space Y-)

### Rotation
- **E**: Pitch up (look upward)
- **C**: Pitch down (look downward)

### Reset
- **S**: Reset camera to initial position and orientation

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

### Object Movement (R/T/F/G/V/B Keys)
- **R**: Move left (X-)
- **T**: Move right (X+)
- **F**: Move down (Y-)
- **G**: Move up (Y+)
- **V**: Move backward (Z-)
- **B**: Move forward (Z+)

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

## UI Controls
- **H**: Toggle HUD visibility
- **TAB**: Cycle object selection forward
- **Shift+TAB**: Cycle object selection backward
- **↑/↓**: Navigate HUD pages
- **I**: Toggle performance info display

## General Controls
- **ESC**: Exit program

## Performance

BVH acceleration is permanently enabled for optimal rendering performance with complex scenes. This ensures consistent frame rates even with hundreds of objects.

## Notes
- Scene is automatically re-rendered after camera movement, object movement, or light adjustment
- All movements are in world space coordinates
- Low quality rendering is used during continuous movement for smooth interaction
- High quality rendering is triggered when keys are released

## Testing the Controls

Try these sequences with any scene file:

1. **Camera Movement**: Press W/X/A/D to move around the scene
2. **Camera World Movement**: Press Q/Z to move up and down in world space
3. **Camera Rotation**: Press E to look up, C to look down
4. **Camera Reset**: Press S to return to the initial camera position
5. **Object Selection**: Press ] to cycle through objects
6. **Object Movement**: Press R/T/F/G/V/B to move selected object in all directions
7. **Light Control**: Press Home/End to move light source vertically
