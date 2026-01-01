# Feature Specification: Keyboard Controls Finalization and BVH Enforcement

**Feature Branch**: `009-keyboard-bvh-controls`  
**Created**: 2026-01-01  
**Status**: Draft  
**Input**: User description: "키보드 컨트롤 최종 조정 및 BVH 강제 활성화"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Object Manipulation with Optimized Key Layout (Priority: P1)

Users need to move selected objects along all three axes (X, Y, Z) using an ergonomic keyboard layout that keeps their hand in one compact region for efficient workflow.

**Why this priority**: Object manipulation is the core interactive feature that enables users to compose and adjust scenes. Without intuitive object controls, the entire ray tracing workflow becomes cumbersome.

**Independent Test**: Can be fully tested by selecting any object in a scene and pressing R/T/F/G/V/B keys to verify movement in all six directions (X-/X+, Y-/Y+, Z-/Z+). Delivers immediate value by enabling basic scene composition.

**Acceptance Scenarios**:

1. **Given** an object is selected in the scene, **When** user presses R key, **Then** object moves in negative X direction (left)
2. **Given** an object is selected in the scene, **When** user presses T key, **Then** object moves in positive X direction (right)
3. **Given** an object is selected in the scene, **When** user presses F key, **Then** object moves in negative Y direction (down)
4. **Given** an object is selected in the scene, **When** user presses G key, **Then** object moves in positive Y direction (up)
5. **Given** an object is selected in the scene, **When** user presses V key, **Then** object moves in negative Z direction (backward)
6. **Given** an object is selected in the scene, **When** user presses B key, **Then** object moves in positive Z direction (forward)
7. **Given** multiple rapid key presses, **When** user holds down any object movement key, **Then** object moves smoothly and continuously in that direction

---

### User Story 2 - Maximum Rendering Performance (Priority: P1)

Users expect consistent, maximum rendering performance at all times when viewing complex scenes with multiple objects. The system should automatically optimize rendering without requiring manual intervention.

**Why this priority**: Rendering performance directly impacts user productivity and experience. Users should not need to understand or manage acceleration structures - the system should always deliver optimal performance automatically.

**Independent Test**: Can be fully tested by loading scenes with varying complexity (10, 100, 1000+ objects) and measuring frame rate or render time. System should maintain consistent performance across all scenes without user configuration.

**Acceptance Scenarios**:

1. **Given** a scene with 100+ objects, **When** user navigates the camera, **Then** rendering maintains consistent frame rate without manual optimization
2. **Given** any scene is loaded, **When** rendering begins, **Then** acceleration structures are automatically active with no user input required
3. **Given** performance metrics are displayed, **When** user views the information, **Then** acceleration structure status shows as always enabled

---

### User Story 3 - Camera Navigation (Priority: P2)

Users need to navigate the 3D scene from different viewpoints using intuitive keyboard controls for translation and rotation to examine their ray-traced compositions from all angles.

**Why this priority**: Camera controls are essential for scene inspection but secondary to object manipulation. Users can work with default camera positions initially, but full navigation is needed for complete workflow.

**Independent Test**: Can be fully tested by loading any scene and using W/X/A/D/Q/Z keys for movement and E/C for rotation. Delivers value by enabling scene inspection from multiple viewpoints.

**Acceptance Scenarios**:

1. **Given** a scene is loaded, **When** user presses W key, **Then** camera moves forward in viewing direction
2. **Given** a scene is loaded, **When** user presses X key, **Then** camera moves backward in viewing direction
3. **Given** a scene is loaded, **When** user presses A key, **Then** camera moves left perpendicular to viewing direction
4. **Given** a scene is loaded, **When** user presses D key, **Then** camera moves right perpendicular to viewing direction
5. **Given** a scene is loaded, **When** user presses Q key, **Then** camera moves up in world space (Y+)
6. **Given** a scene is loaded, **When** user presses Z key, **Then** camera moves down in world space (Y-)
7. **Given** a scene is loaded, **When** user presses E key, **Then** camera pitches up (looks upward)
8. **Given** a scene is loaded, **When** user presses C key, **Then** camera pitches down (looks downward)
9. **Given** camera has been moved from initial position, **When** user presses S key, **Then** camera returns to initial position and orientation

---

### Edge Cases

- What happens when user presses object movement keys but no object is selected?
  - System should either select a default object or display a message indicating no object is selected
- What happens when object movement would place object outside scene bounds?
  - System should either clamp position to valid range or allow unlimited movement based on scene design
- How does system handle simultaneous camera and object movement key presses?
  - Camera keys (W/X/A/D/Q/Z/E/C/S) and object keys (R/T/F/G/V/B) are distinct sets, so should not conflict
- What happens when user holds multiple object movement keys simultaneously?
  - System should combine movements (e.g., R+F = diagonal movement in X- and Y- directions)

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST respond to R key to move selected object in negative X direction (left)
- **FR-002**: System MUST respond to T key to move selected object in positive X direction (right)
- **FR-003**: System MUST respond to F key to move selected object in negative Y direction (down)
- **FR-004**: System MUST respond to G key to move selected object in positive Y direction (up)
- **FR-005**: System MUST respond to V key to move selected object in negative Z direction (backward)
- **FR-006**: System MUST respond to B key to move selected object in positive Z direction (forward)
- **FR-007**: System MUST maintain existing camera movement controls (W/X/A/D/Q/Z for translation)
- **FR-008**: System MUST maintain existing camera rotation controls (E/C for pitch, S for reset)
- **FR-009**: System MUST enable acceleration structures (BVH) permanently without user toggle option
- **FR-010**: System MUST build acceleration structures for all scenes automatically on load
- **FR-011**: System MUST remove all user interface elements related to toggling acceleration structures
- **FR-012**: System MUST display acceleration structure performance metrics in heads-up display
- **FR-013**: System MUST update documentation to reflect new object movement key mappings (R/T/F/G/V/B)
- **FR-014**: System MUST update documentation to remove references to acceleration structure toggling
- **FR-015**: System MUST function identically on both macOS and Linux platforms
- **FR-016**: System MUST follow 42 School norminette coding standards

### Key Entities

- **Keyboard Input Mapping**: Defines the relationship between physical keys and scene manipulation functions. Includes separate mappings for camera controls (W/X/A/D/Q/Z/E/C/S) and object controls (R/T/F/G/V/B).
- **Acceleration Structure Configuration**: System-level setting that determines whether spatial acceleration is enabled. Permanently set to enabled state with no user override capability.
- **Selected Object**: The currently active scene object that responds to object movement keys (R/T/F/G/V/B). Only one object can be selected at a time.
- **Camera State**: Current position and orientation of the viewpoint, manipulated by camera control keys (W/X/A/D/Q/Z/E/C/S). Includes initial state for reset functionality.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Users can move selected objects in all six directions using R/T/F/G/V/B keys with immediate visual feedback (within one frame)
- **SC-002**: Users can perform complex object positioning tasks (e.g., arranging 10 objects in a specific pattern) 30% faster than with previous Numpad controls due to ergonomic key layout
- **SC-003**: System maintains rendering performance of 15+ frames per second with scenes containing 100+ objects, enabled by permanent acceleration structures
- **SC-004**: Users can navigate camera through scene using W/X/A/D/Q/Z/E/C/S keys with responsive feedback (movement visible within 100ms of key press)
- **SC-005**: System renders complex scenes (1000+ objects) at least 10x faster than without acceleration structures, with no user intervention required
- **SC-006**: 100% of keyboard control documentation accurately reflects the new key mappings within system documentation and on-screen help
- **SC-007**: Users complete basic scene composition tasks (camera navigation + object positioning) without consulting documentation, indicating intuitive key layout

## Assumptions

- **A-001**: Object movement distance per key press follows existing implementation standards from the current Numpad-based system
- **A-002**: Camera movement and rotation speeds remain unchanged from current implementation
- **A-003**: Object selection mechanism (which object responds to R/T/F/G/V/B keys) is already implemented and requires no modification
- **A-004**: Acceleration structure implementation (BVH) is already present in codebase and only requires configuration changes to remain permanently enabled
- **A-005**: System has sufficient memory to maintain acceleration structures for typical scenes (assumption: scenes up to 10,000 objects)
- **A-006**: Keyboard input handling supports simultaneous key presses for combined movements
- **A-007**: Documentation files (CONTROLS.md, KEYBOARD_LAYOUT.txt) exist and are maintained as part of the project
- **A-008**: Heads-up display (HUD) system exists for showing performance metrics and key bindings
