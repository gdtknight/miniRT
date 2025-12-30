# Feature Specification: HUD Improvements and Key Guide

**Feature Branch**: `007-hud-improvements`  
**Created**: 2025-12-30  
**Status**: Draft  
**Input**: User description: "HUD 개선 및 키 가이드 추가"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - View Scene Through Transparent HUD (Priority: P1)

Users working with the ray tracing application need to see the full rendered scene while simultaneously viewing scene information in the HUD overlay. Currently, the HUD has an opaque black background that completely obscures the rendered content behind it, forcing users to toggle the HUD off to see the full scene.

**Why this priority**: This is the most critical improvement because it directly affects the primary user workflow. Users need to reference scene data while viewing and adjusting the rendered output. The opaque HUD creates an inefficient workflow where users must constantly toggle visibility.

**Independent Test**: Launch the application with any scene file, press 'H' to ensure HUD is visible, and verify that the rendered scene is visible through the HUD background with readable text overlay.

**Acceptance Scenarios**:

1. **Given** HUD is visible on screen, **When** user views the left side of the window, **Then** the HUD text is clearly readable AND the rendered scene behind it is partially visible through a semi-transparent dark background
2. **Given** HUD is visible with transparent background, **When** user moves camera or objects, **Then** the underlying scene updates are visible through the HUD overlay in real-time
3. **Given** HUD is toggled off, **When** user presses 'H' to show HUD, **Then** the semi-transparent HUD appears without completely blocking the rendered content

---

### User Story 2 - See Real-Time Object Selection Updates (Priority: P2)

Users selecting different objects using bracket keys need immediate visual feedback in the HUD to confirm which object is currently selected. Currently, when users change selection, the HUD does not update to reflect the new selection until another action triggers a redraw.

**Why this priority**: This addresses a usability issue that causes confusion during object manipulation. Users cannot reliably identify which object they are controlling without visual confirmation, leading to accidental modifications.

**Independent Test**: Open a scene with multiple objects, use '[' and ']' keys to cycle through objects, and verify the HUD immediately highlights the newly selected object on each keypress.

**Acceptance Scenarios**:

1. **Given** multiple objects in scene and HUD visible, **When** user presses ']' to select next object, **Then** HUD immediately highlights the newly selected object in green text
2. **Given** first object is selected, **When** user presses '[' to select previous object (wrapping to last), **Then** HUD immediately shows the last object highlighted
3. **Given** object selection changes, **When** user views object list in HUD, **Then** only the currently selected object appears in highlight color

---

### User Story 3 - Work at Comfortable Display Resolution (Priority: P2)

Users working on modern displays (especially MacBook M1 with high-DPI screens) need a window size that properly utilizes their screen real estate. The current 800x600 window is too small for comfortable viewing on 13-inch MacBook displays with 1440x900 effective resolution.

**Why this priority**: Display comfort directly impacts user productivity and reduces eye strain. The current small window size forces users to squint or lean in to see details, particularly problematic for ray tracing work that requires careful visual inspection.

**Independent Test**: Launch the application on a MacBook M1 13-inch display and verify the window opens at 1440x900 resolution, utilizing most of the screen without requiring fullscreen mode.

**Acceptance Scenarios**:

1. **Given** application launch on MacBook M1, **When** window opens, **Then** window size is 1440x900 pixels and fits comfortably on screen
2. **Given** window is resized to 1440x900, **When** scene is rendered, **Then** entire viewport displays rendered content without cropping or distortion
3. **Given** HUD is visible at new resolution, **When** user views HUD layout, **Then** all HUD elements remain properly positioned and readable

---

### User Story 4 - Learn Controls Without Documentation (Priority: P3)

New users or users returning after a break need quick reference to keyboard controls without leaving the application or opening external documentation. Currently, users must reference separate documentation files to remember control schemes.

**Why this priority**: While not critical for experienced users, this significantly improves onboarding and reduces friction for occasional users. It eliminates context switching and makes the application more self-documenting.

**Independent Test**: Launch the application and verify a compact key guide is visible in the upper-right corner showing essential keyboard controls, toggleable with 'H' key like the main HUD.

**Acceptance Scenarios**:

1. **Given** application is launched, **When** window opens with HUD visible, **Then** key guide appears in upper-right corner showing essential controls with semi-transparent background
2. **Given** key guide is visible, **When** user presses 'H' to toggle HUD, **Then** both main HUD and key guide hide together
3. **Given** key guide is displayed, **When** user reads the overlay, **Then** all text is readable and the guide occupies no more than 300x400 pixels
4. **Given** scene is rendering behind key guide, **When** user views upper-right area, **Then** rendered content is partially visible through the semi-transparent guide background

---

### Edge Cases

- What happens when HUD text color is too similar to the rendered scene behind the transparent background? (The semi-transparent dark background should provide sufficient contrast for white text)
- How does the system handle window sizes smaller than the designed 1440x900? (HUD elements maintain relative positioning and clip if necessary)
- What if there are zero objects in the scene when using bracket keys for selection? (Selection handlers already check for zero objects and return early)
- How does the key guide appear on very small displays where 300x400 pixels is significant? (Key guide uses fixed size; assumes minimum viable display of 1440x900)

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: Main HUD MUST display with semi-transparent background allowing rendered scene to be partially visible behind it
- **FR-002**: HUD text MUST remain clearly readable (white text on semi-transparent dark background) in all typical scene conditions
- **FR-003**: System MUST update HUD display immediately when object selection changes via bracket keys '[' and ']'
- **FR-004**: Application window MUST open at 1440x900 pixel resolution on all supported platforms
- **FR-005**: System MUST maintain proper HUD layout proportions when window size changes from 800x600 to 1440x900
- **FR-006**: Key guide HUD MUST display in upper-right corner showing essential keyboard controls
- **FR-007**: Key guide MUST have semi-transparent background matching main HUD transparency level
- **FR-008**: Key guide MUST occupy no more than 300x400 pixels to maintain compact layout
- **FR-009**: Key guide MUST include these essential controls: ESC (exit), H (toggle HUD), TAB (next object), WASD (camera movement), RF (camera pitch), [] (object selection), Numpad (object movement), BQI (rendering options)
- **FR-010**: Both main HUD and key guide MUST toggle visibility together when 'H' key is pressed
- **FR-011**: System MUST implement alpha blending for semi-transparent backgrounds using direct pixel manipulation (MiniLibX does not natively support alpha)
- **FR-012**: Object selection handlers MUST call dirty flag setter to trigger HUD refresh on selection change

### Key Entities

- **HUD State**: Contains visibility flag, current page, dirty flag, and background image buffer for left-side information panel
- **Key Guide HUD State**: New entity containing visibility flag, background image buffer, and layout parameters for right-side control reference
- **Window Dimensions**: Configuration constants defining window width (1440) and height (900) used throughout initialization
- **Selection State**: Current object type and index that triggers HUD updates when modified

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Users can simultaneously view rendered scene and HUD information without toggling HUD visibility
- **SC-002**: HUD text maintains readability with minimum contrast ratio of 4.5:1 against semi-transparent background in typical scene conditions
- **SC-003**: HUD updates reflect object selection changes within 16ms (single frame at 60fps) of bracket key press
- **SC-004**: Application window occupies appropriate screen real estate (approximately 80% of 13-inch MacBook display at 1440x900 vs 40% at 800x600)
- **SC-005**: New users can identify all essential keyboard controls within 5 seconds by reading the key guide without external documentation
- **SC-006**: Key guide remains readable and non-intrusive, occupying less than 10% of total screen area (300x400 = 120K pixels out of 1.3M total pixels)

## Assumptions

- Users have displays capable of at least 1440x900 resolution (standard for MacBook M1 13-inch and modern displays since ~2010)
- MiniLibX does not provide native alpha channel support, requiring manual alpha blending in pixel buffers
- Current alpha blending constant (HUD_BG_ALPHA = 0.65) provides acceptable transparency without implementing complex blending
- Semi-transparent black background provides sufficient contrast for white text in majority of scene conditions
- Users prefer compact key guide over comprehensive control listing (showing only essential controls)
- 'H' key toggle behavior should apply to both HUDs for consistent user experience
- Window size change does not require dynamic resizing support (fixed at launch)
- Existing dirty flag pattern is sufficient for triggering HUD updates
