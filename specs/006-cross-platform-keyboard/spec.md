# Feature Specification: Cross-Platform Keyboard Support

**Feature Branch**: `006-cross-platform-keyboard`  
**Created**: 2025-12-30  
**Status**: Draft  
**Input**: User description: "macOS에서 키보드 동작이 제대로 되지 않는 부분을 수정하여 ubuntu와 macOS 양쪽 모두 지원"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Basic Camera Navigation (Priority: P1)

A developer working on macOS opens miniRT and needs to navigate the 3D scene using standard WASD keys to move the camera position and R/F keys to rotate the camera pitch. The application correctly interprets keyboard input and the camera responds identically to Ubuntu.

**Why this priority**: Camera navigation is the fundamental interaction mechanism. Without working keyboard controls, the application is unusable on macOS. This provides immediate value by making the core functionality accessible.

**Independent Test**: Can be fully tested by launching miniRT on macOS, pressing W/A/S/D keys to move camera and R/F to rotate, and verifying camera position changes match Ubuntu behavior. Delivers a fully functional camera control system.

**Acceptance Scenarios**:

1. **Given** miniRT is running on macOS, **When** user presses W key, **Then** camera moves forward in the view direction
2. **Given** miniRT is running on macOS, **When** user presses A/S/D keys, **Then** camera moves left/backward/right respectively
3. **Given** miniRT is running on macOS, **When** user presses R/F keys, **Then** camera pitch rotates up/down
4. **Given** miniRT is running on Ubuntu, **When** user performs same key presses, **Then** camera behaves identically to macOS

---

### User Story 2 - Object Manipulation Controls (Priority: P2)

A user needs to select and move objects in the scene using bracket keys for selection and numpad keys for movement. On macOS, these controls must work exactly as they do on Ubuntu to maintain consistency across platforms.

**Why this priority**: Object manipulation is a key feature for scene editing but is secondary to basic navigation. Users can still view scenes without it, but cannot edit them effectively.

**Independent Test**: Can be tested independently by loading a scene with multiple objects, using bracket keys to cycle through objects, and numpad keys to move the selected object. Verifies complete object manipulation workflow.

**Acceptance Scenarios**:

1. **Given** a scene with multiple objects is loaded on macOS, **When** user presses ] key, **Then** next object is selected (sphere → plane → cylinder → sphere)
2. **Given** an object is selected on macOS, **When** user presses numpad keys 4/6/2/8/1/3, **Then** object moves in X/Y/Z axes as expected
3. **Given** miniRT is running on Ubuntu, **When** user performs same selection and movement operations, **Then** behavior is identical to macOS

---

### User Story 3 - Lighting Controls (Priority: P3)

A user adjusts light position in the scene using Insert/Delete/Home/End/PageUp/PageDown keys. These controls must work consistently across both Ubuntu and macOS platforms.

**Why this priority**: Lighting adjustment enhances scene quality but is not required for basic viewing or object manipulation. Users can work with default lighting positions if needed.

**Independent Test**: Can be tested by loading any scene and using the six light control keys to move the light source in all three axes, observing lighting changes in real-time on both platforms.

**Acceptance Scenarios**:

1. **Given** miniRT is running on macOS, **When** user presses Insert/Delete keys, **Then** light moves along X-axis (positive/negative)
2. **Given** miniRT is running on macOS, **When** user presses Home/End keys, **Then** light moves along Y-axis (up/down)
3. **Given** miniRT is running on macOS, **When** user presses PageUp/PageDown keys, **Then** light moves along Z-axis (forward/backward)
4. **Given** same operations performed on Ubuntu, **When** user presses same keys, **Then** light moves identically

---

### User Story 4 - Application Control Keys (Priority: P1)

A user needs to control application state using ESC to exit, H to toggle HUD, TAB to navigate HUD elements, arrow keys for HUD navigation, and modifier keys (Shift) for combined actions. These must work on both platforms.

**Why this priority**: Application controls like exit and HUD toggling are essential for basic usability. ESC to exit is a universal convention users expect to work immediately.

**Independent Test**: Can be tested by launching miniRT and verifying ESC exits cleanly, H toggles HUD visibility, TAB navigates HUD sections, and Shift+TAB navigates backward. Delivers complete application control.

**Acceptance Scenarios**:

1. **Given** miniRT is running on macOS, **When** user presses ESC, **Then** application exits cleanly with proper cleanup
2. **Given** miniRT is running on macOS, **When** user presses H, **Then** HUD toggles visibility
3. **Given** HUD is visible on macOS, **When** user presses TAB, **Then** next HUD section is selected
4. **Given** HUD is visible on macOS, **When** user presses Shift+TAB, **Then** previous HUD section is selected
5. **Given** HUD is visible on macOS, **When** user presses UP/DOWN arrow keys, **Then** HUD scrolls up/down through content

---

### User Story 5 - Rendering Option Controls (Priority: P3)

A developer toggles rendering options using B (BVH acceleration), Q (adaptive sampling), and I (info display) keys to optimize performance and debug rendering. These must respond correctly on both platforms.

**Why this priority**: Rendering options are developer/power-user features for optimization and debugging. Basic users can work with default settings.

**Independent Test**: Can be tested by pressing B/Q/I keys and verifying BVH acceleration toggles, adaptive sampling toggles, and info display toggles respectively, with visual feedback on both platforms.

**Acceptance Scenarios**:

1. **Given** miniRT is running on macOS, **When** user presses B key, **Then** BVH acceleration toggles on/off
2. **Given** miniRT is running on macOS, **When** user presses Q key, **Then** adaptive sampling toggles on/off
3. **Given** miniRT is running on macOS, **When** user presses I key, **Then** rendering info display toggles on/off
4. **Given** same operations on Ubuntu, **When** user presses same keys, **Then** toggles behave identically

---

### Edge Cases

- What happens when user presses key combinations not defined in the application?
  - System ignores undefined keys without crashing or showing errors
- What happens when numpad is disabled on laptop keyboards?
  - Users can still use all other controls; object movement requires external numpad or numpad mode
- How does system handle rapid key presses during camera movement?
  - Low-quality rendering activates during interaction, high-quality rendering resumes on key release
- What happens when MiniLibX returns different keycodes than expected on a specific macOS version?
  - Application logs warning but continues operation with available keys
- What happens when user has non-US keyboard layout?
  - Physical key positions match expected behavior (WASD, not ZQSD on AZERTY)

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST detect the operating system platform (Linux/Ubuntu vs macOS) at compile time
- **FR-002**: System MUST define correct keyboard codes for each platform based on MiniLibX implementation (X11 keycodes for Linux, macOS keycodes for macOS)
- **FR-003**: System MUST support all camera movement keys (W/A/S/D forward/left/backward/right, R/F pitch up/down) on both platforms
- **FR-004**: System MUST support all object selection keys (bracket left/right) on both platforms
- **FR-005**: System MUST support all object movement keys (numpad 1-8) on both platforms
- **FR-006**: System MUST support all lighting control keys (Insert/Delete/Home/End/PageUp/PageDown) on both platforms
- **FR-007**: System MUST support all application control keys (ESC/H/TAB/Shift/Arrow Up/Down) on both platforms
- **FR-008**: System MUST support all rendering option keys (B/Q/I) on both platforms
- **FR-009**: System MUST use conditional compilation directives to select appropriate keycode definitions without duplicating handler logic
- **FR-010**: System MUST maintain identical key handling behavior between Ubuntu and macOS platforms
- **FR-011**: System MUST compile successfully on both Ubuntu and macOS without warnings
- **FR-012**: System MUST use platform detection macros (`__APPLE__` for macOS, `__linux__` for Linux) to determine correct keycode set

### Key Entities

- **Keycode Definitions**: Platform-specific integer constants mapping physical keys to system keycodes
  - Linux/X11: Uses X11 KeySym values (e.g., ESC=65307, W=119)
  - macOS: Uses Carbon/Cocoa virtual keycodes (e.g., ESC=53, W=13)
  - Organized by functional groups: camera, object, lighting, application, rendering
- **Platform Detection**: Compile-time macros that identify operating system
  - `__APPLE__`: Defined on macOS platforms
  - `__linux__`: Defined on Linux platforms
  - Determines which keycode set to include

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Application compiles without errors or warnings on both Ubuntu 20.04+ and macOS 10.14+
- **SC-002**: All 30+ keyboard controls (WASD, RF, brackets, numpad, insert/delete/home/end/pgup/pgdn, ESC, TAB, H, Shift, arrows, B, Q, I) respond identically on both platforms
- **SC-003**: User can complete full workflow (launch, navigate scene, select object, move object, adjust lighting, toggle options, exit) using only keyboard on both platforms
- **SC-004**: Zero platform-specific bugs reported in keyboard handling after deployment
- **SC-005**: Code changes are limited to keycode definition section (lines 37-71 of window.c) plus conditional compilation directives
- **SC-006**: Application behavior on Ubuntu remains unchanged after modifications (regression-free)
