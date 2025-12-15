# Data Model: miniRT Ray Tracing

**Date**: 2025-12-15  
**Feature**: miniRT - Ray Tracing 3D Renderer  
**Purpose**: Define core entities, structures, and relationships

## Core Entities

### 1. Vector (t_vec3)

**Purpose**: Fundamental 3D coordinate and direction representation

**Fields**:
- `x` (double): X-axis component
- `y` (double): Y-axis component  
- `z` (double): Z-axis component

**Validation Rules**:
- No range restrictions for position vectors
- Direction/normal vectors MUST be normalized (magnitude = 1.0, tolerance ±0.01)

**Operations**:
- Addition, subtraction, scalar multiplication
- Dot product, cross product
- Normalization, magnitude calculation

**Relationships**:
- Used by: Camera, Light, Sphere, Plane, Cylinder, Ray
- Basis for all spatial calculations

---

### 2. Color (t_color)

**Purpose**: RGB color representation in integer format

**Fields**:
- `r` (int): Red channel [0-255]
- `g` (int): Green channel [0-255]
- `b` (int): Blue channel [0-255]

**Validation Rules**:
- Each channel MUST be in range [0, 255]
- Invalid values trigger parse error

**Operations**:
- Convert to floating point for lighting calculations
- Multiply by lighting intensity
- Clamp to valid range after calculations

**Relationships**:
- Used by: Ambient light, Point light, all geometric objects

---

### 3. Ambient Light (t_ambient)

**Purpose**: Global base illumination for entire scene

**Fields**:
- `ratio` (double): Brightness multiplier [0.0-1.0]
- `color` (t_color): RGB color of ambient light

**Validation Rules**:
- Ratio MUST be in range [0.0, 1.0]
- MUST appear exactly once in scene file
- Color channels MUST be [0-255]

**Scene File Format**:
```
A  <ratio>  <R,G,B>
Example: A  0.2  255,255,255
```

**State Transitions**: None (static after parsing)

**Relationships**:
- One per scene (singleton)
- Applied to all rendered surfaces

---

### 4. Camera (t_camera)

**Purpose**: Defines viewpoint and projection parameters

**Fields**:
- `position` (t_vec3): Camera position in 3D space
- `orientation` (t_vec3): Normalized viewing direction vector
- `fov` (double): Horizontal field of view in degrees [0-180]

**Validation Rules**:
- Orientation vector MUST be normalized
- FOV MUST be in range [0, 180]
- MUST appear exactly once in scene file

**Scene File Format**:
```
C  <x,y,z>  <norm_x,norm_y,norm_z>  <fov>
Example: C  0,0,0  0,0,1  90
```

**Derived Properties**:
- Viewport dimensions calculated from FOV
- Camera coordinate system (up, right vectors) derived from orientation

**State Transitions**: None (static after parsing)

**Relationships**:
- One per scene (singleton)
- Used by rendering engine to generate rays

---

### 5. Point Light (t_light)

**Purpose**: Directional light source with position and intensity

**Fields**:
- `position` (t_vec3): Light position in 3D space
- `brightness` (double): Light intensity ratio [0.0-1.0]
- `color` (t_color): RGB color of light

**Validation Rules**:
- Brightness MUST be in range [0.0, 1.0]
- MUST appear exactly once in scene file (mandatory part)
- Color channels MUST be [0-255]

**Scene File Format**:
```
L  <x,y,z>  <brightness>  <R,G,B>
Example: L  -40,50,0  0.6  255,255,255
```

**State Transitions**: None (static after parsing)

**Relationships**:
- One per scene (mandatory part)
- Used for diffuse lighting and shadow calculations

---

### 6. Sphere (t_sphere)

**Purpose**: Spherical geometric object

**Fields**:
- `center` (t_vec3): Center position in 3D space
- `diameter` (double): Sphere diameter (must be positive)
- `color` (t_color): Surface RGB color

**Validation Rules**:
- Diameter MUST be > 0
- Color channels MUST be [0-255]

**Scene File Format**:
```
sp  <x,y,z>  <diameter>  <R,G,B>
Example: sp  0,0,20  20  255,0,0
```

**Derived Properties**:
- Radius = diameter / 2.0

**Intersection Math**:
- Quadratic equation: at² + bt + c = 0
- Surface normal: (hit_point - center) / radius

**State Transitions**: None (static after parsing)

**Relationships**:
- Multiple spheres allowed per scene
- Part of scene object list

---

### 7. Plane (t_plane)

**Purpose**: Infinite flat surface

**Fields**:
- `point` (t_vec3): Any point on the plane
- `normal` (t_vec3): Normalized vector perpendicular to surface
- `color` (t_color): Surface RGB color

**Validation Rules**:
- Normal vector MUST be normalized
- Color channels MUST be [0-255]

**Scene File Format**:
```
pl  <x,y,z>  <norm_x,norm_y,norm_z>  <R,G,B>
Example: pl  0,-10,0  0,1,0  200,200,200
```

**Intersection Math**:
- t = [(point - ray.origin) · normal] / (ray.direction · normal)
- Surface normal: plane.normal (constant across entire plane)

**State Transitions**: None (static after parsing)

**Relationships**:
- Multiple planes allowed per scene
- Part of scene object list

---

### 8. Cylinder (t_cylinder)

**Purpose**: Finite cylindrical object with caps

**Fields**:
- `center` (t_vec3): Center position of cylinder
- `axis` (t_vec3): Normalized axis direction vector
- `diameter` (double): Cylinder diameter (must be positive)
- `height` (double): Cylinder height along axis (must be positive)
- `color` (t_color): Surface RGB color

**Validation Rules**:
- Axis vector MUST be normalized
- Diameter MUST be > 0
- Height MUST be > 0
- Color channels MUST be [0-255]

**Scene File Format**:
```
cy  <x,y,z>  <axis_x,axis_y,axis_z>  <diameter>  <height>  <R,G,B>
Example: cy  0,0,0  0,1,0  10  20  0,255,0
```

**Derived Properties**:
- Radius = diameter / 2.0
- Bottom cap center: center
- Top cap center: center + axis * height

**Intersection Math**:
- Cylindrical surface: quadratic equation in projected space
- Top/bottom caps: plane intersection + radius check
- Surface normal: varies by intersection type (body vs caps)

**State Transitions**: None (static after parsing)

**Relationships**:
- Multiple cylinders allowed per scene
- Part of scene object list

---

### 9. Ray (t_ray)

**Purpose**: Mathematical ray for tracing and intersection tests

**Fields**:
- `origin` (t_vec3): Ray starting point
- `direction` (t_vec3): Normalized ray direction vector

**Validation Rules**:
- Direction MUST be normalized

**Creation Contexts**:
- Camera rays: generated from camera position through viewport pixels
- Shadow rays: from surface point toward light source
- Reflection rays: (bonus feature, not in mandatory part)

**Operations**:
- Point at distance t: origin + t * direction
- Intersection tests with all geometric objects

**State Transitions**: Created dynamically during rendering, not stored

**Relationships**:
- Used by rendering engine and intersection tests
- Not part of scene file (runtime construct)

---

### 10. Intersection (t_hit)

**Purpose**: Result of ray-object intersection calculation

**Fields**:
- `distance` (double): Distance from ray origin to hit point (t value)
- `point` (t_vec3): 3D coordinates of intersection point
- `normal` (t_vec3): Surface normal at intersection (normalized)
- `color` (t_color): Object color at intersection
- `hit` (bool/int): Whether intersection occurred

**Validation Rules**:
- Distance MUST be positive (intersections behind camera rejected)
- Normal MUST be normalized
- Normal MUST face toward ray origin (for proper lighting)

**State Transitions**:
- Created during intersection test
- Used for lighting calculation
- Discarded after pixel color determined

**Relationships**:
- Generated by object intersection functions
- Consumed by lighting calculation functions
- Not stored persistently

---

### 11. Scene (t_scene)

**Purpose**: Container for all scene data

**Fields**:
- `ambient` (t_ambient): Ambient light settings
- `camera` (t_camera): Camera configuration
- `light` (t_light): Point light source
- `spheres` (t_sphere[]): Dynamic array of spheres
- `sphere_count` (int): Number of spheres
- `planes` (t_plane[]): Dynamic array of planes
- `plane_count` (int): Number of planes
- `cylinders` (t_cylinder[]): Dynamic array of cylinders
- `cylinder_count` (int): Number of cylinders

**Validation Rules**:
- Ambient, camera, light MUST each exist exactly once
- Object arrays can be empty or contain multiple objects
- Memory must be allocated for dynamic arrays

**Construction**:
- Built during scene file parsing
- Validated after parsing completes

**State Transitions**:
- Parsing → Validation → Rendering → Cleanup

**Relationships**:
- Root container for entire scene graph
- Referenced by rendering engine
- Cleaned up on program exit

---

### 12. Render Context (t_render)

**Purpose**: Runtime rendering state and resources

**Fields**:
- `mlx` (void*): MinilibX instance pointer
- `win` (void*): Window pointer
- `img` (void*): Image buffer pointer
- `img_data` (char*): Direct pixel buffer access
- `width` (int): Window width in pixels
- `height` (int): Window height in pixels
- `bits_per_pixel` (int): Color depth
- `line_length` (int): Bytes per image line
- `endian` (int): Byte order

**Validation Rules**:
- Width and height MUST be positive
- Pointers MUST be valid after initialization

**State Transitions**:
- Initialize → Render → Display → Event Loop → Cleanup

**Relationships**:
- Manages MinilibX resources
- References scene data
- Cleaned up on program exit

---

## Entity Relationships Diagram

```
Scene (1)
├── Ambient (1) ─────────────┐
├── Camera (1) ──────────────┤
├── Light (1) ───────────────┤
├── Spheres (*) ─────────────┤── All use Color
├── Planes (*) ──────────────┤── All use Vec3
└── Cylinders (*) ───────────┘

Render Context (1)
├── References Scene
├── Manages MinilibX resources
└── Generates Rays → Intersections → Colors
```

---

## Memory Management

**Allocation Strategy**:
1. Parse scene file → allocate Scene structure
2. Dynamic arrays for objects (realloc as needed)
3. Initialize MinilibX resources
4. Render image buffer

**Deallocation Order** (reverse allocation):
1. Destroy MinilibX image
2. Destroy MinilibX window
3. Destroy/free MinilibX display
4. Free scene object arrays
5. Free scene structure

**Ownership Rules**:
- Scene owns all geometric objects
- Render context owns MinilibX resources
- Main function owns both scene and render context
- Cleanup function handles all deallocations

---

## Summary

**Core Data Structures**: 12 entities defined
- **Singletons**: Ambient, Camera, Light (1 per scene)
- **Collections**: Spheres, Planes, Cylinders (0-N per scene)
- **Computed**: Ray, Intersection (runtime only)
- **Containers**: Scene, Render Context (lifecycle managers)

**Total Structure Count**: ~12 struct definitions
**Validation Points**: Parsing (ranges, normalization), Pre-render (completeness)
**Memory Model**: Explicit allocation/deallocation, no garbage collection

**Next Steps**: Create API contracts for rendering pipeline and scene file format
