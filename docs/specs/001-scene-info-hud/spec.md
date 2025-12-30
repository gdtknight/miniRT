# Feature Specification: Scene Information HUD

**Feature Branch**: `001-scene-info-hud`  
**Created**: 2025-12-19  
**Status**: Draft  
**Input**: User description: "RT 파일 파싱 결과를 좌측 상단에 표시하는 HUD(Heads-Up Display) 기능을 추가하고 싶습니다."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - View Scene Configuration (Priority: P1)

As a miniRT user, I want to see the basic scene information (camera, ambient light, and light source) displayed on screen so that I can verify my RT file is being parsed correctly without manually inspecting code or logs.

**Why this priority**: This is the core value of the HUD - providing immediate visual feedback on scene configuration. Without this, users cannot verify if their RT file setup is correct.

**Independent Test**: Can be fully tested by loading any RT file and verifying that camera position, FOV, direction, ambient light ratio/color, and light source position/brightness/color appear on screen in the top-left corner.

**Acceptance Scenarios**:

1. **Given** a valid RT file is loaded, **When** the scene renders, **Then** the HUD displays camera position (x, y, z), direction vector (normalized), and FOV value
2. **Given** a valid RT file is loaded, **When** the scene renders, **Then** the HUD displays ambient light intensity ratio and RGB color values
3. **Given** a valid RT file is loaded, **When** the scene renders, **Then** the HUD displays light source position (x, y, z), brightness ratio, and RGB color values
4. **Given** the scene is displayed, **When** I view the HUD, **Then** all text is readable with good contrast against the background

---

### User Story 2 - View Object Summary (Priority: P2)

As a miniRT user, I want to see a summary count and basic properties of all objects in the scene (spheres, planes, cylinders) so that I can quickly understand the scene complexity and verify all objects are loaded.

**Why this priority**: Object information helps users confirm scene composition but is less critical than camera/lighting setup for basic verification.

**Independent Test**: Can be tested by loading RT files with varying numbers of spheres, planes, and cylinders, then verifying the HUD shows correct counts and basic properties (position, size, color) for each object type.

**Acceptance Scenarios**:

1. **Given** a RT file with multiple spheres, **When** the scene renders, **Then** the HUD displays sphere count, and for each sphere (up to page capacity): position, radius, and color
2. **Given** a RT file with multiple planes, **When** the scene renders, **Then** the HUD displays plane count, and for each plane (up to page capacity): position, normal vector, and color
3. **Given** a RT file with multiple cylinders, **When** the scene renders, **Then** the HUD displays cylinder count, and for each cylinder (up to page capacity): position, direction, diameter, height, and color
4. **Given** the object count exceeds single-page capacity, **When** viewing the HUD, **Then** pagination controls and page indicators are displayed
5. **Given** pagination is active, **When** I press the next/previous page key, **Then** the HUD displays the next/previous page of objects

---

### User Story 3 - Real-time Updates (Priority: P2)

As a miniRT user, I want the HUD to update in real-time when camera or objects move so that I can track position changes during interactive manipulation.

**Why this priority**: Real-time updates enhance the debugging and exploration experience but the HUD remains useful even with static information.

**Independent Test**: Can be tested by moving the camera or objects using keyboard/mouse controls and verifying that displayed position, direction values update immediately on screen.

**Acceptance Scenarios**:

1. **Given** the scene is displayed with HUD active, **When** the camera position changes, **Then** the HUD updates camera coordinates immediately
2. **Given** the scene is displayed with HUD active, **When** the camera rotates, **Then** the HUD updates the direction vector immediately
3. **Given** the scene is displayed with HUD active, **When** an object is moved, **Then** the HUD updates that object's position immediately

---

### User Story 4 - Toggle HUD Visibility (Priority: P3)

As a miniRT user, I want to toggle the HUD on and off with a keyboard shortcut so that I can view the rendered scene without UI elements when needed.

**Why this priority**: Useful for clean screenshots and unobstructed viewing, but not essential for the core debugging/verification purpose.

**Independent Test**: Can be tested by pressing the toggle key (e.g., 'i' or 'h') and verifying the HUD appears/disappears while maintaining scene rendering.

**Acceptance Scenarios**:

1. **Given** the HUD is visible, **When** I press the toggle key, **Then** the HUD disappears and only the rendered scene is visible
2. **Given** the HUD is hidden, **When** I press the toggle key, **Then** the HUD reappears with current scene information
3. **Given** the toggle operation occurs, **When** the HUD state changes, **Then** the rendering performance remains unaffected

---

### User Story 5 - Highlight Selected Object (Priority: P3)

As a miniRT user, I want the currently selected object to be highlighted in the HUD so that I can easily identify which object I'm manipulating.

**Why this priority**: Nice-to-have feature that improves user experience for advanced manipulation but not required for basic information display.

**Independent Test**: Can be tested by selecting different objects (via mouse click or keyboard navigation) and verifying the corresponding HUD entry is highlighted with different text color or background.

**Acceptance Scenarios**:

1. **Given** multiple objects are in the scene, **When** I select a specific object using Tab/Shift+Tab to cycle through objects, **Then** that object's information in the HUD is highlighted
2. **Given** an object is selected and highlighted, **When** I press Tab to cycle to the next object, **Then** the previous highlight is removed and the new object is highlighted
3. **Given** an object is selected and highlighted, **When** I press Shift+Tab to cycle to the previous object, **Then** the previous highlight is removed and the new object is highlighted
4. **Given** no object is selected, **When** viewing the HUD, **Then** no object information is highlighted

---

### Edge Cases

- What happens when the RT file contains a very large number of objects (e.g., 50+ spheres)? The HUD MUST implement pagination controls (next/previous page) to display objects page-by-page. Each page should show a reasonable subset (e.g., 5-10 objects per page) with page indicators (e.g., "Page 1/5").
- What happens if text color has poor contrast with the scene background? The HUD MUST render a semi-transparent dark background rectangle behind all HUD text using MLX basic drawing functions (e.g., mlx_pixel_put or mlx_put_image_to_window with pre-filled rectangle buffer) to ensure readability.
- What happens when vector values or coordinates have many decimal places? Display should round or truncate to 2-3 decimal places for readability.
- What happens if the HUD content exceeds screen height? Content should be paginated using next/previous page controls.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST display camera information including position (x, y, z coordinates), normalized direction vector, and FOV value
- **FR-002**: System MUST display ambient light information including intensity ratio and RGB color values
- **FR-003**: System MUST display light source information including position (x, y, z coordinates), brightness ratio, and RGB color values
- **FR-004**: System MUST display object count and properties for each object type (sphere, plane, cylinder)
- **FR-005**: System MUST display sphere properties: position, radius, RGB color
- **FR-006**: System MUST display plane properties: position, normal vector, RGB color
- **FR-007**: System MUST display cylinder properties: position, direction vector, diameter, height, RGB color
- **FR-008**: System MUST position the HUD in the top-left corner of the window
- **FR-009**: System MUST render HUD text with sufficient contrast for readability by displaying a semi-transparent dark background rectangle behind all HUD text using MLX basic drawing primitives
- **FR-010**: System MUST update HUD information when camera or object positions change
- **FR-011**: System MUST respond to a toggle key press to show/hide the HUD
- **FR-012**: System MUST maintain rendering performance when HUD is active (no visible frame rate degradation)
- **FR-013**: System MUST support keyboard-based object navigation using Tab (next object) and Shift+Tab (previous object) to cycle through objects in the HUD list
- **FR-014**: System MUST highlight selected object information in the HUD when an object is selected via Tab/Shift+Tab navigation
- **FR-015**: System MUST format numerical values (coordinates, ratios) with 2-3 decimal places for readability
- **FR-015**: System MUST use a compact layout that does not obscure more than 25% of the left side of the screen
- **FR-016**: System MUST implement pagination controls (next/previous page keys or arrows) when object count exceeds single-page capacity
- **FR-017**: System MUST display page indicators (e.g., "Page 1/5") when pagination is active
- **FR-018**: System MUST show a reasonable number of objects per page (5-10 objects) to maintain readability without scrolling

### Key Entities

- **HUD State**: Represents visibility status (visible/hidden), current scene data snapshot, selected object index, current page index, total page count
- **Camera Info**: Position vector (x, y, z), direction vector (normalized), FOV angle value
- **Ambient Light Info**: Intensity ratio (0.0-1.0), RGB color components (0-255 each)
- **Light Source Info**: Position vector (x, y, z), brightness ratio (0.0-1.0), RGB color components (0-255 each)
- **Object Info**: Type identifier (sphere/plane/cylinder), position, type-specific properties (radius, normal, diameter, height, direction), RGB color
- **Scene Data**: Collection of all parsed entities from RT file that need to be displayed

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Users can view all essential scene information (camera, ambient light, light source) without opening the RT file or inspecting logs
- **SC-002**: HUD displays complete information for scenes with up to 10 objects per page; scenes with more objects use pagination to maintain readability
- **SC-003**: HUD information updates within 16ms (one frame at 60fps) when camera or objects move
- **SC-004**: HUD rendering adds no more than 2ms to total frame time, maintaining 60fps performance on target hardware
- **SC-005**: Users can toggle HUD visibility with a single keypress, with state change occurring within one frame
- **SC-006**: All text displayed in the HUD is readable at standard viewing distance (50-70cm from screen) on a 1920x1080 display
- **SC-007**: Selected object highlighting via Tab/Shift+Tab navigation is visually distinguishable within 500ms of selection action

### Observability

- **OBS-001**: System MUST log HUD initialization at INFO level with enabled/disabled state
- **OBS-002**: System MUST log HUD toggle events at INFO level with new visibility state
- **OBS-003**: System MUST log HUD render time metrics at DEBUG level for performance monitoring
- **OBS-004**: System MUST log errors at ERROR level when HUD rendering fails or text display encounters issues
- **OBS-005**: System MUST log warnings at WARN level when HUD content truncation occurs due to excessive object count

## Assumptions

- The miniRT project already has working RT file parsing that populates scene data structures
- The MLX library's mlx_string_put function is sufficient for rendering text (no custom font rendering needed)
- MLX basic drawing functions (mlx_pixel_put or mlx_put_image_to_window) can render semi-transparent dark background rectangles behind text
- Camera and object manipulation controls already exist in the codebase
- Object selection mechanism exists or will be implemented separately
- The 42 Norm compliance is handled by following existing code patterns in the repository
- Standard keyboard input handling is already implemented in the project
- Tab and Shift+Tab keyboard shortcuts for object cycling do not conflict with existing keybindings
- Semi-transparent backgrounds or text outlines can be achieved using MLX primitives or alpha blending
- The toggle key 'h' (for HUD) or 'i' (for info) does not conflict with existing keybindings

## Dependencies

- MLX library (MiniLibX) must provide text rendering capabilities via mlx_string_put
- Existing RT file parser must expose parsed scene data in accessible data structures
- Existing rendering loop must allow injection of HUD rendering step (background rectangle + text overlay) without disrupting main scene rendering
- Keyboard input system must support Tab and Shift+Tab key events for object navigation
- If object selection highlighting is required, an object selection system must be present or implemented

## Out of Scope

- Custom font rendering or typography beyond what mlx_string_put provides
- Graphical UI elements (buttons, sliders, menus) - this is text-only display
- Editing scene properties through the HUD - this is read-only information display
- Performance profiling or metrics display beyond basic scene information
- Saving or exporting HUD snapshots
- Advanced layout features like resizing, repositioning, or customization by user
- Manual translation: Spec/wiki documentation translation will use automated GitHub Actions with translation API/tool

## Clarifications

### Session 2025-12-19

- Q: How should spec updates/releases synchronize with project wiki documentation? → A: GitHub Actions로 자동 번역 API/도구 사용
- Q: What observability/logging level should be included for HUD functionality? → A: 전체 구조화된 로깅 (debug/info/warn/error) + HUD 렌더 시간, 토글 이벤트 메트릭
- Q: How should many objects (e.g., 50+ objects) be displayed in the HUD when content exceeds screen height? → A: 페이지네이션: 다음/이전 페이지 컨트롤로 오브젝트를 페이지 단위로 표시
- Q: What technique should be used for HUD text background contrast to ensure readability? → A: MLX 기본 함수를 사용한 반투명 어두운 배경 사각형을 HUD 뒤에 배치
- Q: How should users navigate and select objects in the HUD list for highlighting purposes? → A: 키보드 순환 (Tab/Shift+Tab으로 오브젝트 순환)
