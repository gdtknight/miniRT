# Feature Specification: miniRT - Ray Tracing 3D Renderer

**Feature Branch**: `001-raytracer`  
**Created**: 2025-12-15  
**Status**: Draft  
**Input**: User description: "Ray Tracing protocol-based 3D image generation program in C"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Basic Scene Rendering (Priority: P1)

As a user, I provide a scene description file (.rt format) to the miniRT program, and the program generates a rendered 3D image displayed in a window showing the defined geometric objects (spheres, planes, cylinders) from the camera's viewpoint.

**Why this priority**: This is the core functionality - without scene rendering, the program has no value. This establishes the foundation for all other features.

**Independent Test**: Create a simple scene file with one sphere and one light source. Run `./miniRT scene.rt` and verify a rendered sphere appears in the window. This delivers immediate visual feedback demonstrating ray tracing works.

**Acceptance Scenarios**:

1. **Given** a valid .rt file with camera, light, and one sphere, **When** user runs `./miniRT scene.rt`, **Then** a window opens displaying the rendered sphere with proper shading
2. **Given** a scene with multiple objects (sphere, plane, cylinder), **When** rendering executes, **Then** all objects appear correctly positioned according to their coordinates
3. **Given** a scene with overlapping objects, **When** ray tracing calculates intersections, **Then** only the nearest visible surface is displayed
4. **Given** an invalid file path, **When** user runs the program, **Then** program outputs "Error\n" followed by explicit error message and exits cleanly

---

### User Story 2 - Scene File Parsing (Priority: P1)

As a user, I create scene description files using the .rt format with elements for ambient lighting (A), camera (C), light (L), and objects (sp, pl, cy), and the program correctly interprets all elements regardless of their order in the file.

**Why this priority**: Parsing is a prerequisite for rendering - the program must understand scene definitions before it can render them. This is equally critical as rendering itself.

**Independent Test**: Create a .rt file with elements in random order, mixed spacing, and valid parameters. Program should parse successfully and identify all elements correctly, which can be verified by adding debug output or by successful rendering.

**Acceptance Scenarios**:

1. **Given** a scene file with A, C, L declared once each, **When** parsing executes, **Then** program accepts the file and stores all parameters correctly
2. **Given** a scene file with objects in any order, **When** parsing executes, **Then** all objects are recognized regardless of position in file
3. **Given** a scene file with multiple ambient lighting declarations, **When** parsing executes, **Then** program outputs "Error\n" with message "Ambient lighting can only be declared once" and exits
4. **Given** a scene file with RGB value 256 (out of range 0-255), **When** parsing executes, **Then** program outputs "Error\n" with message indicating invalid color range and exits
5. **Given** a scene file with normalized vector [2.0, 0, 0] (invalid magnitude), **When** parsing executes, **Then** program outputs "Error\n" with message indicating vector must be normalized and exits
6. **Given** a scene file with FOV of 200 (exceeds range 0-180), **When** parsing executes, **Then** program outputs "Error\n" with message indicating invalid FOV range and exits

---

### User Story 3 - Lighting System Implementation (Priority: P2)

As a user viewing a rendered scene, I see realistic lighting effects where ambient lighting prevents objects from appearing completely black, diffuse lighting creates brightness gradients based on light direction, and hard shadows appear behind objects blocking light sources.

**Why this priority**: While the program can render shapes without proper lighting, realistic lighting transforms flat shapes into recognizable 3D objects. This significantly enhances visual quality and validates the ray tracing algorithm.

**Independent Test**: Create a scene with one sphere and one light positioned at different angles. Verify the sphere shows a gradient from bright (facing light) to darker (facing away), with ambient lighting preventing pure black. Move the light and verify the bright area moves accordingly.

**Acceptance Scenarios**:

1. **Given** a scene with ambient lighting ratio 0.2, **When** rendering executes, **Then** all surfaces receive at least 20% of their base color brightness even in shadow
2. **Given** a scene with a sphere and directional light, **When** rendering calculates diffuse lighting, **Then** surfaces perpendicular to light direction appear brightest, surfaces parallel appear darkest
3. **Given** a scene with one object blocking light from reaching another object, **When** shadow calculation executes, **Then** a hard-edged shadow appears on the blocked surface
4. **Given** a light with brightness ratio 0.6, **When** rendering applies light intensity, **Then** the maximum brightness from that light source is 60% of full intensity

---

### User Story 4 - Ray-Object Intersection Calculation (Priority: P2)

As the rendering engine, I calculate precise intersection points between camera rays and all geometric objects (spheres, planes, cylinders including caps) to determine which surfaces are visible from each pixel.

**Why this priority**: Accurate intersection mathematics is the core of ray tracing. Without correct intersections, objects will appear distorted or disappear. This must work before adding advanced features like transformations.

**Independent Test**: Render a scene with a sphere at known coordinates. Verify the sphere appears circular (not oval) and at the expected screen position based on camera settings. Test with rays at different angles to verify all intersection cases.

**Acceptance Scenarios**:

1. **Given** a ray from camera and a sphere, **When** intersection calculation executes, **Then** the nearest intersection point is computed correctly using the quadratic equation for ray-sphere intersection
2. **Given** a ray and a plane, **When** intersection calculation executes, **Then** the intersection point is computed correctly using ray-plane intersection formula
3. **Given** a ray and a cylinder, **When** intersection calculation executes, **Then** intersections with the cylindrical surface, top cap, and bottom cap are all detected correctly
4. **Given** a ray originating inside a sphere, **When** intersection calculation executes, **Then** the exit point is computed correctly for proper transparency/refraction handling
5. **Given** a ray missing all objects, **When** intersection calculation executes, **Then** no intersection is returned and background color is used

---

### User Story 5 - Window Management (Priority: P2)

As a user viewing a rendered scene, I can interact with the program window by pressing ESC to exit, clicking the red X button to close, minimizing the window, and switching between applications without crashes or hangs.

**Why this priority**: Proper window management ensures the program behaves like a standard desktop application. Users expect these controls to work reliably. This is essential for usability but doesn't affect core rendering.

**Independent Test**: Launch program with a valid scene. Test: press ESC (should exit cleanly), launch again and click X button (should exit cleanly), minimize window (should remain responsive), switch to another application and back (should redraw correctly).

**Acceptance Scenarios**:

1. **Given** the program is displaying a rendered scene, **When** user presses ESC key, **Then** the window closes and program exits with return code 0
2. **Given** the program is displaying a rendered scene, **When** user clicks the red X button, **Then** the window closes and program exits with return code 0
3. **Given** the program window is open, **When** user minimizes the window, **Then** the window minimizes without crashing and restores correctly when un-minimized
4. **Given** the program is running, **When** user switches to another application and back, **Then** the rendered scene remains visible without corruption
5. **Given** any exit condition, **When** program terminates, **Then** all allocated memory is freed with no memory leaks detected by tools like Valgrind

---

### User Story 6 - Object Transformations (Priority: P3)

As a scene designer, I position objects, lights, and cameras anywhere in 3D space using translation coordinates, and I orient planes and cylinders using rotation via normalized direction vectors.

**Why this priority**: Transformations enable diverse scene compositions. While important for flexibility, basic rendering works without transformations if objects use default orientations. This enhances creative possibilities but isn't required for MVP.

**Independent Test**: Create two scene files with identical objects but different camera positions and orientations. Verify both render correctly with different perspectives. Create scenes with cylinders at various orientations and verify they appear correctly angled.

**Acceptance Scenarios**:

1. **Given** a camera with position coordinates (x, y, z), **When** rendering executes, **Then** the scene is rendered from that viewpoint in 3D space
2. **Given** a camera with normalized direction vector, **When** rendering executes, **Then** the camera looks in that direction with proper orientation
3. **Given** a plane with a normalized normal vector, **When** ray intersection executes, **Then** the plane orientation in space matches the specified normal direction
4. **Given** a cylinder with a normalized axis vector, **When** rendering executes, **Then** the cylinder is oriented along that axis in 3D space
5. **Given** a sphere at any position, **When** transformation is requested, **Then** position changes work but rotation has no effect (spheres are rotationally symmetric)
6. **Given** a light at any position, **When** transformation is requested, **Then** position changes work but rotation has no effect (point lights are omnidirectional)

---

### User Story 7 - Adjustable Object Properties (Priority: P3)

As a scene designer, I specify object dimensions in the scene file: sphere diameter, cylinder diameter and height, allowing me to create objects of various sizes without changing object types.

**Why this priority**: Adjustable dimensions provide flexibility in scene design. While useful, a program with fixed-size objects would still demonstrate ray tracing principles. This enhances creative control but is not critical for core functionality.

**Independent Test**: Create multiple scene files with identical object positions but different diameter/height values. Verify rendered objects appear at correct sizes - larger values produce visibly larger objects, smaller values produce smaller objects.

**Acceptance Scenarios**:

1. **Given** a sphere with diameter 10.0, **When** rendering executes, **Then** the sphere's radius is 5.0 units and it appears at the correct visual size
2. **Given** two spheres with diameters 5.0 and 20.0, **When** rendering executes, **Then** the second sphere appears 4 times larger in diameter than the first
3. **Given** a cylinder with diameter 10.0 and height 20.0, **When** rendering executes, **Then** the cylinder has radius 5.0 and height 20.0 with correct proportions
4. **Given** a cylinder with diameter larger than height, **When** rendering executes, **Then** the cylinder appears short and wide as expected

---

### Edge Cases

- **File does not exist**: Program outputs "Error\n" with message "Cannot open file: [filename]" and exits
- **File has wrong extension**: Program outputs "Error\n" with message "Invalid file extension, expected .rt" and exits
- **Empty scene file**: Program outputs "Error\n" with message "Scene file is empty or incomplete" and exits
- **Missing mandatory elements (A, C, or L)**: Program outputs "Error\n" with message "Missing mandatory element: [A/C/L]" and exits
- **Multiple declarations of singleton elements**: Program outputs "Error\n" with message "[Element] can only be declared once" and exits
- **Malformed numeric values**: Program outputs "Error\n" with message "Invalid numeric value at line [N]" and exits
- **Ray parallel to cylinder axis**: Intersection calculation handles edge case where ray is parallel to infinite cylinder axis
- **Ray tangent to sphere**: Intersection calculation correctly identifies single tangent point (discriminant = 0)
- **Camera FOV at boundary values (0 or 180)**: Rendering handles extreme field-of-view values without distortion artifacts
- **Objects at same distance from camera**: Depth comparison uses consistent tie-breaking to avoid flickering
- **Very small objects (diameter < 0.1)**: Rendering maintains precision for small geometric objects
- **Very large scenes (>100 objects)**: Ray tracing completes in reasonable time or provides feedback on progress
- **Ambient lighting ratio 0.0**: All shadow areas appear completely black with only direct lighting visible
- **Ambient lighting ratio 1.0**: All surfaces appear at full brightness regardless of lighting
- **Light brightness ratio 0.0**: Light source has no effect on scene (only ambient lighting visible)
- **Cylinder with zero height**: Program outputs "Error\n" with message "Cylinder height must be positive" and exits
- **Sphere with zero diameter**: Program outputs "Error\n" with message "Sphere diameter must be positive" and exits

## Requirements *(mandatory)*

### Functional Requirements

**Core Rendering:**

- **FR-001**: Program MUST accept exactly one command-line argument: a scene description file with .rt extension
- **FR-002**: Program MUST create a window using MinilibX library to display the rendered scene
- **FR-003**: Program MUST implement ray tracing algorithm to generate 3D images from scene descriptions
- **FR-004**: Program MUST render three mandatory geometric objects: planes, spheres, and cylinders
- **FR-005**: Program MUST calculate all intersection points between rays and objects, including intersections when rays originate inside objects
- **FR-006**: Program MUST handle both the cylindrical surface and circular caps (top and bottom) of cylinders

**Scene File Parsing:**

- **FR-007**: Program MUST parse scene files where elements can appear in any order
- **FR-008**: Program MUST accept elements separated by one or more spaces or newlines
- **FR-009**: Program MUST enforce that Ambient lighting (A), Camera (C), and Light (L) are each declared exactly once per scene
- **FR-010**: Program MUST allow multiple object declarations (sp, pl, cy) in a single scene
- **FR-011**: Program MUST validate all numeric ranges: RGB values [0-255], ratios [0.0-1.0], normalized vectors [-1,1], FOV [0-180]
- **FR-012**: Program MUST verify that direction vectors and normal vectors are normalized (magnitude = 1.0)

**Lighting System:**

- **FR-013**: Program MUST implement ambient lighting that applies a base illumination to all surfaces based on the ambient ratio [0.0-1.0]
- **FR-014**: Program MUST implement diffuse lighting (Lambertian reflection) based on the angle between surface normal and light direction
- **FR-015**: Program MUST calculate hard shadows by casting shadow rays from surface points to light sources
- **FR-016**: Program MUST apply light brightness ratio [0.0-1.0] to modulate light intensity
- **FR-017**: Program MUST combine ambient and diffuse lighting components to produce final pixel colors

**Object Properties:**

- **FR-018**: Spheres MUST support adjustable diameter parameter
- **FR-019**: Cylinders MUST support adjustable diameter parameter
- **FR-020**: Cylinders MUST support adjustable height parameter
- **FR-021**: All objects MUST support RGB color specification [0-255] for each channel

**Transformations:**

- **FR-022**: Camera MUST support position transformation (translation) via x,y,z coordinates
- **FR-023**: Camera MUST support orientation transformation via normalized 3D direction vector
- **FR-024**: Camera MUST implement field-of-view (FOV) parameter [0-180 degrees] for horizontal viewing angle
- **FR-025**: Objects (planes, spheres, cylinders) MUST support position transformation via x,y,z coordinates
- **FR-026**: Planes MUST support orientation transformation via normalized normal vector
- **FR-027**: Cylinders MUST support orientation transformation via normalized axis vector
- **FR-028**: Lights MUST support position transformation via x,y,z coordinates
- **FR-029**: Spheres and lights MUST NOT implement rotation (spheres are symmetric, lights are omnidirectional)

**Window Management:**

- **FR-030**: Program MUST exit cleanly when ESC key is pressed
- **FR-031**: Program MUST exit cleanly when the window's close button (red X) is clicked
- **FR-032**: Program MUST handle window minimization without crashing
- **FR-033**: Program MUST handle window focus changes (switching to/from other applications) without corruption
- **FR-034**: Program MUST use MinilibX images for rendering (strongly recommended for performance)

**Error Handling:**

- **FR-035**: Program MUST output "Error\n" followed by an explicit error message for any file or parsing error
- **FR-036**: Program MUST exit with appropriate return code after displaying error message
- **FR-037**: Program MUST validate file exists and is readable before parsing
- **FR-038**: Program MUST validate file extension is .rt
- **FR-039**: Program MUST detect and report missing mandatory elements (A, C, L)
- **FR-040**: Program MUST detect and report duplicate declarations of singleton elements (A, C, L)
- **FR-041**: Program MUST detect and report malformed numeric values
- **FR-042**: Program MUST detect and report out-of-range parameter values
- **FR-043**: Program MUST free all allocated memory before exiting (no memory leaks)

**Build System:**

- **FR-044**: Makefile MUST include 'all' rule to compile the program producing executable named 'miniRT'
- **FR-045**: Makefile MUST include 'clean' rule to remove object files
- **FR-046**: Makefile MUST include 'fclean' rule to remove object files and executable
- **FR-047**: Makefile MUST include 're' rule to rebuild from scratch
- **FR-048**: Makefile MUST link with MinilibX library
- **FR-049**: Makefile MUST link with math library using -lm flag
- **FR-050**: Program MAY use libft library if desired

### Key Entities

- **Scene**: Represents the complete 3D environment to be rendered, containing camera, lights, ambient lighting, and geometric objects

- **Camera**: Defines the viewpoint with attributes: position (x,y,z coordinates), orientation (normalized direction vector), field-of-view (angle in degrees 0-180)

- **Ambient Light**: Global illumination with attributes: ratio (brightness multiplier 0.0-1.0), RGB color (0-255 per channel)

- **Point Light**: Directional light source with attributes: position (x,y,z coordinates), brightness ratio (0.0-1.0), RGB color (0-255 per channel)

- **Sphere**: Geometric object with attributes: center position (x,y,z coordinates), diameter (positive value), RGB color (0-255 per channel)

- **Plane**: Infinite flat surface with attributes: point on plane (x,y,z coordinates), normal vector (normalized direction perpendicular to surface), RGB color (0-255 per channel)

- **Cylinder**: Geometric object with attributes: center position (x,y,z coordinates), axis vector (normalized direction), diameter (positive value), height (positive value), RGB color (0-255 per channel)

- **Ray**: Mathematical construct for ray tracing with attributes: origin point (x,y,z coordinates), direction vector (normalized), used to determine visibility and lighting

- **Intersection**: Result of ray-object collision with attributes: distance from ray origin, surface normal at intersection point, material color, used for rendering and shadow calculations

## Success Criteria *(mandatory)*

### Measurable Outcomes

**Rendering Quality:**

- **SC-001**: Spheres appear as perfect circles when viewed head-on, demonstrating accurate ray-sphere intersection mathematics
- **SC-002**: Cylinders display both curved surfaces and flat circular caps correctly, showing proper cap intersection detection
- **SC-003**: Shadows have hard, well-defined edges with no gradual falloff, validating binary shadow ray detection
- **SC-004**: Objects maintain correct proportions when camera orientation changes, proving transformation mathematics work correctly
- **SC-005**: Scenes with overlapping objects show correct depth ordering with nearer objects occluding farther ones

**Lighting Accuracy:**

- **SC-006**: Surfaces facing light sources appear brighter than surfaces facing away, demonstrating diffuse lighting calculation
- **SC-007**: All surfaces maintain minimum visibility equal to ambient lighting ratio even in complete shadow
- **SC-008**: Light brightness ratio directly controls maximum illumination intensity, scalable from 0% to 100%
- **SC-009**: Objects blocking light create shadows on surfaces behind them, proving shadow ray implementation

**Parsing Robustness:**

- **SC-010**: Program successfully parses scene files with elements in any order, validating order-independent parsing
- **SC-011**: Program correctly rejects scene files with duplicate A, C, or L declarations, showing validation logic works
- **SC-012**: Program detects and reports all out-of-range values (RGB > 255, ratio > 1.0, FOV > 180, non-normalized vectors) with specific error messages
- **SC-013**: Error messages include specific details about what failed and where, enabling users to fix scene files without guessing

**User Experience:**

- **SC-014**: Users can exit the program via ESC key or window close button, both methods exit cleanly without hangs or crashes
- **SC-015**: Window minimization and restoration work smoothly without requiring program restart
- **SC-016**: Users can modify object sizes (diameter, height) in scene files and immediately see size changes in rendered output
- **SC-017**: Users can reposition camera and see the scene from different viewpoints by changing camera position/direction values

**Technical Performance:**

- **SC-018**: Program completes without memory leaks as verified by memory analysis tools (Valgrind, LeakSanitizer)
- **SC-019**: Simple scenes (1-5 objects) render in under 5 seconds on standard hardware
- **SC-020**: Program handles scenes with up to 20 objects without crashing or excessive delay (under 30 seconds)

**Code Quality:**

- **SC-021**: Makefile successfully builds program with 'make all' command producing 'miniRT' executable
- **SC-022**: Makefile 'make clean' removes all object files, 'make fclean' removes all generated files, 'make re' rebuilds from scratch
- **SC-023**: Program uses only permitted functions (standard library functions, MinilibX, math library, specified system calls)

### Assumptions

- **Display environment**: Program runs on systems with X11/XQuartz display server (required for MinilibX)
- **Input format**: Scene files use ASCII text encoding with Unix or Windows line endings
- **Coordinate system**: Right-handed 3D coordinate system with Y-axis pointing up
- **Color model**: RGB color space with 8 bits per channel (0-255 range)
- **Ray origin**: Camera rays originate from camera position and pass through virtual screen pixels
- **Shadow detection**: Binary visibility (fully lit or fully shadowed) with no partial shadows or penumbra
- **Intersection precision**: Floating-point precision sufficient for typical scene scales (objects sized 1-100 units)
- **Camera projection**: Perspective projection based on FOV (not orthographic)
- **Cylinder caps**: Cylinders are finite with solid circular caps at both ends (not infinite or open-ended)
- **Performance target**: Reasonable rendering time for educational/demonstration purposes, not real-time interactive rates
- **Error recovery**: Program exits on first error rather than attempting to continue with partial scene data
- **Lighting model**: Phong reflection model components limited to ambient and diffuse (specular reserved for bonus)
- **Thread safety**: Single-threaded execution (no concurrent rendering)

### Scope Boundaries

**In Scope:**
- Rendering three geometric primitives: planes, spheres, cylinders
- Ambient and diffuse lighting with hard shadows
- Scene file parsing with comprehensive error validation
- Basic transformations: translation for all objects, rotation for planes/cylinders via normal/axis vectors
- Window management with standard close/minimize operations
- RGB color specification for objects and lights

**Out of Scope (Reserved for Bonus/Future):**
- Specular reflection (Phong highlights)
- Checkerboard or texture patterns
- Multiple colored light sources (mandatory part uses single light)
- Additional geometric objects (cone, hyperboloid, paraboloid)
- Bump mapping or normal mapping
- Soft shadows or area lights
- Reflection or refraction effects
- Anti-aliasing or supersampling
- Animation or interactive camera controls
- Real-time rendering or GPU acceleration
- Loading texture images from files
- Advanced materials (metallic, glass, subsurface scattering)
