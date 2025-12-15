# Rendering Pipeline Contract

**Version**: 1.0  
**Date**: 2025-12-15  
**Purpose**: Formal specification for ray tracing rendering algorithm

## Overview

The rendering pipeline transforms a parsed scene into a 2D image by tracing rays from the camera through each pixel, calculating intersections with objects, applying lighting, and determining final pixel colors.

## Pipeline Stages

```
1. Initialization
   ↓
2. Camera Ray Generation
   ↓
3. Object Intersection
   ↓
4. Lighting Calculation
   ↓
5. Color Composition
   ↓
6. Pixel Write
```

---

## 1. Initialization Stage

**Input**: Parsed scene (t_scene), window dimensions
**Output**: Render context (t_render) with MinilibX resources initialized

**Contract**:
```c
t_render *init_render(t_scene *scene, int width, int height);
```

**Preconditions**:
- Scene is fully parsed and validated
- Width and height are positive integers (typical: 800x600, 1920x1080)

**Postconditions**:
- MinilibX instance created (`mlx_init()`)
- Window created (`mlx_new_window()`)
- Image buffer created (`mlx_new_image()`)
- Direct pixel buffer access obtained (`mlx_get_data_addr()`)
- Camera viewport calculated based on FOV and aspect ratio

**Viewport Calculation**:
```
aspect_ratio = width / height
viewport_height = 2.0 * tan(fov_radians / 2.0)
viewport_width = viewport_height * aspect_ratio
```

**Camera Coordinate System**:
- Forward: camera.orientation (normalized)
- Right: normalize(cross(forward, world_up))
- Up: cross(right, forward)

---

## 2. Camera Ray Generation

**Input**: Pixel coordinates (x, y), camera parameters
**Output**: Ray (t_ray) from camera through pixel

**Contract**:
```c
t_ray generate_camera_ray(t_camera *camera, int x, int y, 
                          int width, int height);
```

**Preconditions**:
- Pixel coordinates within bounds: 0 ≤ x < width, 0 ≤ y < height
- Camera viewport calculated

**Postconditions**:
- Ray origin = camera position
- Ray direction is normalized
- Ray passes through center of pixel (x, y) on virtual viewport

**Ray Direction Calculation**:
```
// Normalize pixel coordinates to [-1, 1] range
u = (2.0 * x / width) - 1.0
v = 1.0 - (2.0 * y / height)  // Inverted for screen coordinates

// Calculate point on viewport
viewport_point = camera.position + 
                 u * viewport_width * camera.right +
                 v * viewport_height * camera.up +
                 camera.forward

// Ray direction
direction = normalize(viewport_point - camera.position)
```

**Invariants**:
- Ray direction magnitude = 1.0

---

## 3. Object Intersection

**Input**: Ray, scene objects
**Output**: Nearest intersection (t_hit) or no hit

**Contract**:
```c
t_hit find_nearest_intersection(t_ray ray, t_scene *scene);
```

**Preconditions**:
- Ray direction is normalized
- Scene contains at least one object type array (may be empty)

**Postconditions**:
- Returns nearest intersection with t > EPSILON (avoid self-intersection)
- If no intersection: hit.hit = false
- If intersection: hit contains distance, point, normal, color

**Intersection Testing Order**:
1. Test all spheres
2. Test all planes
3. Test all cylinders
4. Keep track of nearest hit (smallest positive t)

### 3.1 Ray-Sphere Intersection

**Contract**:
```c
t_hit intersect_sphere(t_ray ray, t_sphere sphere);
```

**Algorithm**:
```
// Quadratic equation: at² + bt + c = 0
oc = ray.origin - sphere.center
a = dot(ray.direction, ray.direction)  // = 1.0 for normalized rays
b = 2.0 * dot(oc, ray.direction)
c = dot(oc, oc) - radius²

discriminant = b² - 4ac

if discriminant < 0:
    return no_hit  // Ray misses sphere

t1 = (-b - sqrt(discriminant)) / (2a)
t2 = (-b + sqrt(discriminant)) / (2a)

// Choose nearest positive t
if t1 > EPSILON:
    t = t1
else if t2 > EPSILON:
    t = t2
else:
    return no_hit

// Calculate hit point and normal
hit.point = ray.origin + t * ray.direction
hit.normal = normalize(hit.point - sphere.center)
hit.distance = t
hit.color = sphere.color
```

**Invariants**:
- EPSILON = 0.0001 (avoid self-intersection)
- Normal always points outward from sphere center
- Normal magnitude = 1.0

### 3.2 Ray-Plane Intersection

**Contract**:
```c
t_hit intersect_plane(t_ray ray, t_plane plane);
```

**Algorithm**:
```
denom = dot(ray.direction, plane.normal)

if abs(denom) < EPSILON:
    return no_hit  // Ray parallel to plane

t = dot(plane.point - ray.origin, plane.normal) / denom

if t < EPSILON:
    return no_hit  // Plane behind ray origin

hit.point = ray.origin + t * ray.direction
hit.normal = plane.normal  // May need to flip if denom < 0
hit.distance = t
hit.color = plane.color
```

**Invariants**:
- Normal orientation: flip if dot(ray.direction, normal) > 0 (two-sided planes)

### 3.3 Ray-Cylinder Intersection

**Contract**:
```c
t_hit intersect_cylinder(t_ray ray, t_cylinder cylinder);
```

**Algorithm** (simplified overview):
```
// Test infinite cylinder body
oc = ray.origin - cylinder.center
ca = cylinder.axis

// Project onto plane perpendicular to axis
d_proj = ray.direction - dot(ray.direction, ca) * ca
oc_proj = oc - dot(oc, ca) * ca

// Quadratic equation in projected space
a = dot(d_proj, d_proj)
b = 2.0 * dot(oc_proj, d_proj)
c = dot(oc_proj, oc_proj) - radius²

discriminant = b² - 4ac

if discriminant >= 0:
    t1 = (-b - sqrt(discriminant)) / (2a)
    t2 = (-b + sqrt(discriminant)) / (2a)
    
    // Check if hits are within height [0, h]
    for each t in [t1, t2]:
        hit_point = ray.origin + t * ray.direction
        height_pos = dot(hit_point - cylinder.center, ca)
        if 0 <= height_pos <= cylinder.height and t > EPSILON:
            candidate = t

// Test top cap (plane at center + axis * height)
cap_top_hit = intersect_plane(ray, 
                               plane_at(cylinder.center + cylinder.axis * height,
                                       cylinder.axis))
if cap_top_hit.hit:
    dist_from_axis = distance(cap_top_hit.point projection onto axis)
    if dist_from_axis <= radius:
        candidate = cap_top_hit.distance

// Test bottom cap (plane at center)
cap_bottom_hit = intersect_plane(ray, plane_at(cylinder.center, cylinder.axis))
// Similar check as top cap

// Return nearest valid candidate
```

**Invariants**:
- Body normal: perpendicular to axis, pointing outward
- Cap normals: ±cylinder.axis depending on top/bottom

---

## 4. Lighting Calculation

**Input**: Intersection point, normal, scene lighting
**Output**: Final color (t_color)

**Contract**:
```c
t_color calculate_lighting(t_hit hit, t_scene *scene, t_ray view_ray);
```

**Preconditions**:
- hit.hit = true (valid intersection)
- hit.normal is normalized
- Scene contains ambient and light

**Postconditions**:
- Color channels clamped to [0, 255]

**Lighting Formula**:
```
final_color = ambient_component + diffuse_component

// Ambient component (always present)
ambient = scene.ambient.ratio * scene.ambient.color * hit.color

// Diffuse component (if not in shadow)
light_dir = normalize(scene.light.position - hit.point)
in_shadow = cast_shadow_ray(hit.point, light_dir, scene)

if not in_shadow:
    n_dot_l = max(0, dot(hit.normal, light_dir))
    diffuse = scene.light.brightness * scene.light.color * 
              hit.color * n_dot_l
else:
    diffuse = 0

// Combine and clamp
final = clamp(ambient + diffuse, 0, 255)
```

### 4.1 Shadow Ray Casting

**Contract**:
```c
bool cast_shadow_ray(t_vec3 point, t_vec3 light_dir, t_scene *scene);
```

**Algorithm**:
```
// Create shadow ray with small offset to avoid self-intersection
shadow_ray.origin = point + EPSILON * light_dir
shadow_ray.direction = light_dir

// Calculate distance to light
light_distance = distance(point, scene.light.position)

// Test intersection with all objects
for each object in scene:
    hit = intersect(shadow_ray, object)
    if hit.hit and hit.distance < light_distance:
        return true  // In shadow

return false  // No obstruction, fully lit
```

**Invariants**:
- Shadow ray offset prevents self-intersection
- Only intersections closer than light matter

---

## 5. Color Composition

**Input**: Calculated lighting color (float RGB [0.0-1.0])
**Output**: Integer RGB color [0-255]

**Contract**:
```c
t_color compose_color(t_color_f lighting);
```

**Algorithm**:
```
r = clamp((int)(lighting.r * 255), 0, 255)
g = clamp((int)(lighting.g * 255), 0, 255)
b = clamp((int)(lighting.b * 255), 0, 255)
```

**Optional Enhancements**:
- Gamma correction: `color^(1/2.2)`
- Tone mapping for HDR

---

## 6. Pixel Write

**Input**: Pixel coordinates (x, y), color
**Output**: Pixel set in image buffer

**Contract**:
```c
void put_pixel(t_render *render, int x, int y, t_color color);
```

**Algorithm**:
```
// Calculate pixel offset in image buffer
pixel_offset = (y * render->line_length) + (x * (render->bits_per_pixel / 8))

// Write color (assuming 32-bit RGBA or XRGB format)
int color_value = (color.r << 16) | (color.g << 8) | color.b
*(int *)(render->img_data + pixel_offset) = color_value
```

**Preconditions**:
- x, y within image bounds
- Image buffer valid

**Postconditions**:
- Pixel at (x, y) set to specified color

---

## Complete Rendering Loop

**Main Render Function**:
```c
void render_scene(t_render *render, t_scene *scene)
{
    for (int y = 0; y < render->height; y++)
    {
        for (int x = 0; x < render->width; x++)
        {
            // 1. Generate camera ray
            t_ray ray = generate_camera_ray(&scene->camera, 
                                            x, y, 
                                            render->width, 
                                            render->height);
            
            // 2. Find nearest intersection
            t_hit hit = find_nearest_intersection(ray, scene);
            
            // 3. Calculate color
            t_color color;
            if (hit.hit)
                color = calculate_lighting(hit, scene, ray);
            else
                color = (t_color){0, 0, 0};  // Background black
            
            // 4. Write pixel
            put_pixel(render, x, y, color);
        }
    }
    
    // Display rendered image
    mlx_put_image_to_window(render->mlx, render->win, 
                           render->img, 0, 0);
}
```

---

## Performance Considerations

**Optimization Opportunities**:
1. **Early Ray Termination**: Skip intersection tests for distant objects
2. **Spatial Partitioning**: Use bounding volume hierarchy for many objects
3. **Vector Math**: Minimize sqrt() calls, cache dot products
4. **Memory Access**: Optimize pixel buffer writes

**Benchmarks** (reference for validation):
- Simple scene (3-5 objects): < 5 seconds on modern hardware
- Complex scene (20 objects): < 30 seconds

---

## Error Handling

**Numerical Issues**:
- Division by zero: Check denominators before division
- Floating point precision: Use EPSILON for comparisons
- Overflow: Clamp color values

**Resource Issues**:
- Image buffer NULL: Abort rendering
- Invalid pixel coordinates: Bounds checking in debug mode

---

## Testing Contracts

**Unit Tests Required**:
1. `test_ray_sphere_intersection()`: All cases (miss, tangent, two hits)
2. `test_ray_plane_intersection()`: Parallel, perpendicular, oblique
3. `test_ray_cylinder_intersection()`: Body, caps, miss
4. `test_shadow_ray()`: Occluded, unoccluded, grazing
5. `test_lighting_calculation()`: Ambient only, diffuse, combined
6. `test_camera_ray_generation()`: Corner pixels, center pixel

**Integration Tests Required**:
1. Render known scene, compare output image
2. Verify sphere appears circular
3. Verify shadows have hard edges
4. Verify color accuracy

---

## Version History

- **1.0** (2025-12-15): Initial rendering pipeline specification
