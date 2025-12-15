# Research: miniRT Ray Tracing Implementation

**Date**: 2025-12-15  
**Feature**: miniRT - Ray Tracing 3D Renderer  
**Purpose**: Resolve technical unknowns and establish implementation patterns

## 1. Unit Testing Framework for C

### Decision
Use **Criterion** testing framework for unit tests.

### Rationale
- **Modern and lightweight**: Criterion is a cross-platform unit testing framework for C with automatic test registration
- **Clear output**: Provides colored output, detailed failure messages, and test statistics
- **Easy setup**: Simple integration with Makefile, no complex configuration
- **42 compatible**: Does not violate norminette rules (tests are separate from source)
- **Active maintenance**: Well-documented and actively maintained
- **Fixtures support**: Built-in setup/teardown for test isolation
- **Assertions**: Rich assertion macros for various comparison types

### Alternatives Considered
1. **cmocka** - Good framework but more complex setup, requires mocking library
2. **Unity** - Lightweight but lacks automatic test discovery, requires manual test registration
3. **Custom test harness** - Simple but lacks features, would require significant development time
4. **Google Test (C++)** - Powerful but requires C++ compilation, adds complexity to C project

### Implementation Approach
- Install Criterion via package manager or compile from source
- Create `tests/unit/` directory structure mirroring `src/`
- Add separate Makefile target for tests: `make test`
- Each source module gets corresponding test file (e.g., `vector.c` → `test_vector.c`)
- Run tests as part of build verification workflow

---

## 2. Ray-Sphere Intersection Mathematics

### Decision
Use **quadratic equation method** with discriminant calculation.

### Rationale
Ray equation: `P(t) = origin + t * direction`  
Sphere equation: `|P - center|² = radius²`

Substituting ray into sphere equation yields quadratic:
```
at² + bt + c = 0
where:
  a = direction · direction (always 1 for normalized rays)
  b = 2 * direction · (origin - center)
  c = (origin - center) · (origin - center) - radius²

discriminant = b² - 4ac
```

- **discriminant < 0**: No intersection (ray misses sphere)
- **discriminant = 0**: Ray tangent to sphere (one intersection)
- **discriminant > 0**: Ray intersects sphere twice (entry and exit)

For rendering, we typically want the nearest positive t value (visible surface in front of camera).

### Best Practices
- Check discriminant before computing square root (avoid unnecessary computation)
- Reject negative t values (intersections behind camera)
- Handle numerical precision carefully for grazing angles
- Optimize by normalizing ray direction once, then use simplified formula

### References
- "Ray Tracing in One Weekend" by Peter Shirley
- Real-Time Rendering (4th Edition), Chapter 22
- Scratchapixel.com ray-sphere intersection tutorial

---

## 3. Ray-Plane Intersection Mathematics

### Decision
Use **dot product method** with normal vector.

### Rationale
Plane equation: `(P - P₀) · N = 0`  
Where P₀ is a point on plane, N is normal vector

Ray equation: `P(t) = origin + t * direction`

Substituting:
```
(origin + t * direction - P₀) · N = 0
t = [(P₀ - origin) · N] / (direction · N)
```

### Best Practices
- Check if `direction · N ≈ 0` (ray parallel to plane, no intersection)
- Reject negative t values (plane behind camera)
- Normalize normal vector once during parsing
- Consider two-sided vs one-sided planes (check if dot product sign matters)

### Edge Cases
- Ray exactly on plane (origin is P₀): return no intersection or t=0 depending on design
- Ray parallel to plane: denominator approaches zero, detect and handle

---

## 4. Ray-Cylinder Intersection Mathematics

### Decision
Use **combined approach** - intersect infinite cylinder + cap planes, then validate.

### Rationale
Cylinder defined by:
- Center point C
- Axis direction A (normalized)
- Radius r
- Height h

**Step 1: Infinite cylinder intersection**
- Project ray onto plane perpendicular to axis
- Solve quadratic equation in 2D projection
- Similar to sphere intersection but in reduced dimensions

**Step 2: Cap intersection**
- Caps are circular planes at C and C + h*A
- Use plane intersection formula for each cap
- Check if intersection point is within radius from axis

**Step 3: Validation**
- For body intersections: check if height is within [0, h]
- For cap intersections: check if distance from axis ≤ radius
- Return nearest valid positive t value

### Best Practices
- Pre-compute axis-aligned bounding box for early rejection
- Handle ray parallel to cylinder axis (special case)
- Handle ray intersecting edge between cap and body (numerical precision)
- Consider finite cylinder vs infinite cylinder in code organization

### References
- "Ray Tracing Gems" Chapter 6
- Graphics Gems II, Chapter on cylinder intersection
- SIGGRAPH course notes on ray-primitive intersection

---

## 5. MinilibX Window Management Patterns

### Decision
Use **event-driven architecture** with MinilibX hooks.

### Rationale
MinilibX provides event loop with callback hooks:
- `mlx_loop_hook()`: Called continuously (for animation, not needed for static rendering)
- `mlx_key_hook()`: Keyboard events (ESC key)
- `mlx_mouse_hook()`: Mouse events (not required for mandatory part)
- `mlx_expose_hook()`: Window redraw events (handle minimize/restore)
- `mlx_hook()`: Generic X11 event handler (window close button)

### Implementation Pattern
```c
// Initialize window
void *mlx = mlx_init();
void *win = mlx_new_window(mlx, width, height, "miniRT");

// Render to image buffer (not directly to window)
void *img = mlx_new_image(mlx, width, height);
char *img_data = mlx_get_data_addr(img, &bpp, &line_len, &endian);
// ... fill img_data with rendered pixels ...
mlx_put_image_to_window(mlx, win, img, 0, 0);

// Register event handlers
mlx_hook(win, KeyPress, KeyPressMask, handle_keypress, &data);
mlx_hook(win, DestroyNotify, NoEventMask, handle_close, &data);
mlx_hook(win, Expose, ExposureMask, handle_expose, &data);

// Enter event loop
mlx_loop(mlx);
```

### Best Practices
- Use image buffer for rendering (recommended in subject)
- Store rendered image for expose events (avoid re-rendering on minimize/restore)
- Clean up resources in exit handlers (avoid memory leaks)
- Return 0 from event handlers to continue event loop, 1 to exit
- Handle X11 events directly via `mlx_hook()` for fine-grained control

---

## 6. Phong Lighting Model (Ambient + Diffuse)

### Decision
Implement **Phong reflection model** limited to ambient and diffuse components.

### Rationale
Phong model: `I = I_ambient + I_diffuse + I_specular`

For mandatory part (no specular):
```
I = ambient_ratio * object_color + 
    Σ(light_brightness * object_color * max(0, N · L))
```

Where:
- N = surface normal at intersection point (normalized)
- L = direction from surface point to light source (normalized)
- `max(0, N · L)` ensures no negative lighting (surfaces facing away are unlit)

### Components
**Ambient lighting**: Base illumination prevents completely black areas
```
ambient = ambient_ratio * ambient_color * object_color
```

**Diffuse lighting**: Angle-dependent brightness (Lambertian reflection)
```
diffuse = light_brightness * light_color * object_color * (N · L)
```

### Best Practices
- Normalize all vectors before dot product calculations
- Clamp final color values to [0, 255] range
- Apply gamma correction for more realistic output (optional)
- Pre-compute ambient component once per object (doesn't change per light)

### Shadow Implementation
- Cast shadow ray from surface point toward light
- If shadow ray intersects any object before reaching light: surface is in shadow
- Skip diffuse component for shadowed areas (keep only ambient)
- Add small epsilon offset to shadow ray origin to avoid self-intersection

---

## 7. Scene File Parsing Strategy

### Decision
Use **line-by-line tokenization** with element-specific validation.

### Rationale
Scene file format is line-oriented with space-separated values:
```
A  0.2  255,255,255
C  0,0,0  0,0,1  90
L  -40,50,0  0.6  255,255,255
sp 0,0,20  20  255,0,0
```

### Implementation Pattern
1. **Read file line by line**
2. **Tokenize by whitespace**: split on spaces/tabs
3. **Identify element type**: first token (A, C, L, sp, pl, cy)
4. **Parse parameters**: element-specific parsing based on type
5. **Validate ranges**: check RGB [0-255], ratios [0.0-1.0], FOV [0-180]
6. **Validate vectors**: ensure normalized vectors have magnitude ≈ 1.0
7. **Track singleton elements**: error if A, C, or L appear multiple times
8. **Build scene structure**: populate scene object with parsed data

### Best Practices
- Use `strtok()` or custom tokenizer for splitting
- Use `atof()` for floating point, `atoi()` for integers
- Provide specific error messages with line numbers
- Allow flexible whitespace (multiple spaces, tabs)
- Ignore empty lines and comments (if adding comment support)
- Validate each element immediately after parsing (fail fast)

### Error Handling Pattern
```c
// Example error output
fprintf(stderr, "Error\n");
fprintf(stderr, "Invalid RGB value at line %d: value %d exceeds range [0-255]\n", 
        line_num, value);
exit(EXIT_FAILURE);
```

---

## 8. Memory Management Strategy

### Decision
Implement **centralized cleanup** with resource tracking.

### Rationale
42 projects require zero memory leaks (Valgrind clean). Strategy:
- Track all allocations in central structure
- Implement cleanup function called on both normal and error exits
- Use wrapper functions for malloc/free if needed
- Free resources in reverse allocation order

### Implementation Pattern
```c
typedef struct s_data {
    void        *mlx;
    void        *win;
    void        *img;
    t_scene     *scene;     // contains all parsed objects
    // ... other resources
} t_data;

void cleanup_data(t_data *data)
{
    if (data->img)
        mlx_destroy_image(data->mlx, data->img);
    if (data->win)
        mlx_destroy_window(data->mlx, data->win);
    if (data->mlx)
    {
        mlx_destroy_display(data->mlx);
        free(data->mlx);
    }
    if (data->scene)
        free_scene(data->scene);
}

// On exit (both normal and error)
cleanup_data(&data);
```

### Best Practices
- Register cleanup function with `atexit()` or call explicitly
- Set pointers to NULL after freeing
- Check pointers before freeing
- Free child resources before parent
- Use valgrind regularly during development: `valgrind --leak-check=full ./miniRT scene.rt`

---

## 9. 42 Norminette Compliance Patterns

### Decision
Structure code to **naturally comply** with norminette rules.

### Key Rules
- **Function length**: Maximum 25 lines per function
- **Function parameters**: Maximum 4 parameters
- **Line length**: Maximum 80 characters (norm v3) or 80 columns
- **Functions per file**: Maximum 5 functions
- **Header**: 42 standard header required in all files
- **Naming**: Snake_case for functions, UPPER_CASE for macros/constants
- **Braces**: K&R style (opening brace on same line for functions, control structures)

### Strategies
- **Break complex functions**: Extract helper functions for ray-tracing math
- **Use structures for parameters**: Instead of 6 parameters, pass one struct
- **Chain calculations**: Break long expressions across multiple lines
- **Descriptive names**: Use clear names even if longer (better than cryptic)
- **Helper functions**: Create specific helpers like `calculate_discriminant()`, `get_nearest_intersection()`

### Example Structure Parameter Pattern
```c
// Instead of: intersect_sphere(origin_x, origin_y, origin_z, dir_x, dir_y, dir_z, center, radius)
// Use:
typedef struct s_ray {
    t_vec3  origin;
    t_vec3  direction;
} t_ray;

double  intersect_sphere(t_ray ray, t_sphere sphere);
```

---

## 10. Color and RGB Handling

### Decision
Use **integer RGB representation** with floating-point intermediate calculations.

### Rationale
- Scene file uses integer RGB [0-255]
- Lighting calculations use floating point [0.0-1.0]
- Final output converts back to integer RGB

### Implementation Pattern
```c
typedef struct s_color {
    int r;  // [0, 255]
    int g;
    int b;
} t_color;

typedef struct s_color_f {
    double r;  // [0.0, 1.0]
    double g;
    double b;
} t_color_f;

// Convert for calculations
t_color_f color_to_float(t_color c) {
    return (t_color_f){c.r / 255.0, c.g / 255.0, c.b / 255.0};
}

// Convert back for display
t_color color_to_int(t_color_f c) {
    return (t_color){
        fmin(255, fmax(0, (int)(c.r * 255))),
        fmin(255, fmax(0, (int)(c.g * 255))),
        fmin(255, fmax(0, (int)(c.b * 255)))
    };
}
```

### Best Practices
- Clamp values to prevent overflow/underflow
- Use floating point for all lighting calculations
- Convert to integer only at final pixel write
- Consider using 0xRRGGBB integer format for MinilibX image buffer

---

## Summary

All NEEDS CLARIFICATION items resolved:
- ✅ Testing framework: Criterion selected for unit testing
- ✅ Ray-object intersection mathematics documented
- ✅ Lighting model strategy defined (ambient + diffuse)
- ✅ Scene parsing approach established
- ✅ MinilibX patterns identified
- ✅ Memory management strategy defined
- ✅ Norminette compliance patterns documented
- ✅ Color handling approach defined

**Next Phase**: Phase 1 - Design & Contracts (data model, API contracts, quickstart guide)
