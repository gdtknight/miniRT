# API Contract: Precomputed Geometry Properties

**Feature**: 011-code-refactoring  
**Module**: Scene Parsing / Object Structures  
**Files**: `includes/objects.h`, `src/parser/parse_objects.c`  
**Date**: 2026-01-01

## Overview

This contract defines the interface for precomputed geometry properties in object structures. Properties such as radius, squared radius, and half-height are computed once during scene parsing and stored in object structures for efficient access during rendering.

---

## Structure Modifications

### Enhanced Sphere Structure

```c
/**
 * @brief Sphere object with precomputed geometry for optimized intersection
 * 
 * Represents a sphere primitive in the scene. In addition to the scene file
 * parameters (center, diameter, color), this structure includes precomputed
 * values to eliminate redundant calculations during ray tracing.
 * 
 * @note Precomputed fields (radius, radius_squared) are set during parsing
 *       and remain constant throughout rendering (immutable).
 */
typedef struct s_sphere
{
    t_vec3  center;          /**< Center point in 3D world space */
    double  diameter;        /**< Diameter as specified in scene file */
    t_color color;           /**< RGB color with components [0-255] */
    double  radius;          /**< Precomputed: diameter / 2.0 */
    double  radius_squared;  /**< Precomputed: radius * radius */
}   t_sphere;
```

**Field Contracts**:

| Field | Type | Constraint | Computed When | Used Where |
|-------|------|------------|---------------|------------|
| `center` | `t_vec3` | Any valid 3D point | Parsing | Intersection tests |
| `diameter` | `double` | > 0 | Parsing | User reference only |
| `color` | `t_color` | RGB [0-255] | Parsing | Shading |
| `radius` | `double` | == diameter / 2.0 | Parsing | Intersection, lighting |
| `radius_squared` | `double` | == radius² | Parsing | Quadratic formula |

---

### Enhanced Cylinder Structure

```c
/**
 * @brief Finite cylinder with precomputed geometry for optimized intersection
 * 
 * Represents a finite cylinder primitive with flat caps. Includes precomputed
 * radius and half-height values to eliminate divisions in the render loop.
 * 
 * @note Axis vector is normalized during parsing
 * @note Precomputed fields are immutable after parsing
 */
typedef struct s_cylinder
{
    t_vec3  center;          /**< Center point of cylinder */
    t_vec3  axis;            /**< Normalized axis direction vector */
    double  diameter;        /**< Diameter as specified in scene file */
    double  height;          /**< Height as specified in scene file */
    t_color color;           /**< RGB color [0-255] */
    double  radius;          /**< Precomputed: diameter / 2.0 */
    double  radius_squared;  /**< Precomputed: radius * radius */
    double  half_height;     /**< Precomputed: height / 2.0 for cap tests */
}   t_cylinder;
```

**Field Contracts**:

| Field | Type | Constraint | Computed When | Used Where |
|-------|------|------------|---------------|------------|
| `center` | `t_vec3` | Any valid point | Parsing | Intersection |
| `axis` | `t_vec3` | Normalized (length=1) | Parsing | Intersection, caps |
| `diameter` | `double` | > 0 | Parsing | User reference only |
| `height` | `double` | > 0 | Parsing | User reference only |
| `color` | `t_color` | RGB [0-255] | Parsing | Shading |
| `radius` | `double` | == diameter / 2.0 | Parsing | Body intersection |
| `radius_squared` | `double` | == radius² | Parsing | Quadratic formula |
| `half_height` | `double` | == height / 2.0 | Parsing | Cap intersection |

---

## Parsing Interface

### Sphere Parsing Contract

```c
/**
 * @brief Parse sphere definition from scene file and compute cached values
 * 
 * Reads sphere parameters from scene file line and initializes all fields
 * including precomputed geometry properties. Validates diameter > 0.
 * 
 * @param line     Scene file line containing: "sp x y z diameter r g b"
 * @param sphere   Pointer to sphere structure to initialize
 * 
 * @return 1 on success, 0 on parse error
 * 
 * @post sphere->radius == sphere->diameter / 2.0
 * @post sphere->radius_squared == sphere->radius * sphere->radius
 * @post sphere->diameter > 0
 * @post sphere->radius > 0
 * @post sphere->radius_squared > 0
 * 
 * @note This function MUST compute radius and radius_squared immediately
 *       after parsing diameter to maintain struct invariants
 */
int parse_sphere(char *line, t_sphere *sphere);
```

**Implementation Requirements**:
```c
int parse_sphere(char *line, t_sphere *sphere)
{
    /* Parse basic fields: center, diameter, color */
    if (sscanf(line, "sp %lf %lf %lf %lf %d %d %d",
               &sphere->center.x, &sphere->center.y, &sphere->center.z,
               &sphere->diameter,
               &sphere->color.r, &sphere->color.g, &sphere->color.b) != 7)
        return (0);
    
    /* Validate diameter */
    if (sphere->diameter <= 0)
        return (error_invalid_diameter());
    
    /* REQUIRED: Compute precomputed values immediately */
    sphere->radius = sphere->diameter / 2.0;
    sphere->radius_squared = sphere->radius * sphere->radius;
    
    return (1);
}
```

---

### Cylinder Parsing Contract

```c
/**
 * @brief Parse cylinder definition and compute cached geometry values
 * 
 * Reads cylinder parameters from scene file and initializes all fields.
 * Normalizes axis vector and computes radius, squared radius, and half-height.
 * 
 * @param line      Scene file line: "cy x y z nx ny nz diameter height r g b"
 * @param cylinder  Pointer to cylinder structure to initialize
 * 
 * @return 1 on success, 0 on parse error
 * 
 * @post cylinder->axis is normalized (length == 1.0 within EPSILON)
 * @post cylinder->radius == cylinder->diameter / 2.0
 * @post cylinder->radius_squared == cylinder->radius * cylinder->radius
 * @post cylinder->half_height == cylinder->height / 2.0
 * @post All values > 0
 * 
 * @note MUST normalize axis vector during parsing
 * @note MUST compute all cached values immediately after parsing
 */
int parse_cylinder(char *line, t_cylinder *cylinder);
```

**Implementation Requirements**:
```c
int parse_cylinder(char *line, t_cylinder *cylinder)
{
    /* Parse basic fields */
    if (sscanf(line, "cy %lf %lf %lf %lf %lf %lf %lf %lf %d %d %d",
               &cylinder->center.x, &cylinder->center.y, &cylinder->center.z,
               &cylinder->axis.x, &cylinder->axis.y, &cylinder->axis.z,
               &cylinder->diameter, &cylinder->height,
               &cylinder->color.r, &cylinder->color.g, &cylinder->color.b) != 11)
        return (0);
    
    /* Validate geometry */
    if (cylinder->diameter <= 0 || cylinder->height <= 0)
        return (error_invalid_geometry());
    
    /* REQUIRED: Normalize axis vector */
    cylinder->axis = vec3_normalize(cylinder->axis);
    
    /* Validate normalization succeeded (zero vector check) */
    if (vec3_length(cylinder->axis) < EPSILON)
        return (error_invalid_axis());
    
    /* REQUIRED: Compute precomputed values immediately */
    cylinder->radius = cylinder->diameter / 2.0;
    cylinder->radius_squared = cylinder->radius * cylinder->radius;
    cylinder->half_height = cylinder->height / 2.0;
    
    return (1);
}
```

---

## Usage Interface

### Sphere Intersection Usage

```c
/**
 * @brief Test ray-sphere intersection using precomputed radius values
 * 
 * @param ray     Ray to test
 * @param sphere  Sphere with precomputed radius/radius_squared
 * @param hit     Hit structure to update
 * 
 * @return 1 if hit, 0 otherwise
 * 
 * @note Uses sphere->radius instead of computing diameter/2
 * @note Uses sphere->radius_squared in quadratic formula
 */
int intersect_sphere(t_ray *ray, t_sphere *sphere, t_hit *hit)
{
    t_vec3  oc;
    double  t;
    
    /* Use precomputed radius directly (no division) */
    oc = vec3_subtract(ray->origin, sphere->center);
    t = calculate_sphere_t(oc, ray, sphere->radius);
    
    /* ... intersection logic ... */
}
```

**Before Refactoring**:
```c
double radius = sphere->diameter / 2.0;  /* Computed every call */
c = vec3_dot(oc, oc) - radius * radius;  /* Multiplication every call */
```

**After Refactoring**:
```c
/* Access precomputed values directly */
c = vec3_dot(oc, oc) - sphere->radius_squared;  /* No division, no multiplication */
```

---

### Cylinder Intersection Usage

```c
/**
 * @brief Calculate cylinder intersection using precomputed values
 * 
 * @param ray  Ray to test
 * @param cyl  Cylinder with precomputed radius/radius_squared/half_height
 * @param calc Calculation structure to populate
 * 
 * @return 1 if intersection found, 0 otherwise
 */
static int calculate_cylinder_intersection(t_ray *ray, t_cylinder *cyl,
                                            t_cyl_calc *calc)
{
    t_vec3  oc;
    
    /* Use precomputed radius directly (no division) */
    oc = vec3_subtract(ray->origin, cyl->center);
    
    /* Quadratic coefficients using cached radius_squared */
    calc->a = vec3_dot(ray->direction, ray->direction) - /* ... */;
    calc->c = vec3_dot(oc, oc) - /* ... */ - cyl->radius_squared;  /* Precomputed */
    
    /* ... rest of intersection logic ... */
}

/**
 * @brief Check cylinder cap intersection using precomputed half_height
 */
int intersect_cylinder_cap(t_ray *ray, t_cylinder *cyl, t_hit *hit, double cap_m)
{
    /* Use precomputed half_height for cap position */
    if (fabs(cap_m) > cyl->half_height)  /* Direct access, no division */
        return (0);
    
    /* ... cap intersection logic ... */
}
```

---

## Invariant Validation

### Debug-Mode Validation

```c
/**
 * @brief Validate sphere invariants (debug builds only)
 * 
 * Checks that precomputed values match their definitions. Should be called
 * after parsing in debug builds to catch computation errors.
 * 
 * @param sphere Sphere to validate
 * @return 1 if valid, 0 if invariants violated
 */
#ifdef DEBUG
int validate_sphere_invariants(const t_sphere *sphere)
{
    double expected_radius = sphere->diameter / 2.0;
    double expected_radius_sq = expected_radius * expected_radius;
    
    if (fabs(sphere->radius - expected_radius) > EPSILON)
    {
        fprintf(stderr, "ERROR: sphere->radius invariant violated\n");
        return (0);
    }
    
    if (fabs(sphere->radius_squared - expected_radius_sq) > EPSILON)
    {
        fprintf(stderr, "ERROR: sphere->radius_squared invariant violated\n");
        return (0);
    }
    
    if (sphere->diameter <= 0 || sphere->radius <= 0 || sphere->radius_squared <= 0)
    {
        fprintf(stderr, "ERROR: sphere has non-positive geometry values\n");
        return (0);
    }
    
    return (1);
}
#endif
```

### Runtime Validation (Optional)

```c
/**
 * @brief Validate cylinder invariants
 */
#ifdef DEBUG
int validate_cylinder_invariants(const t_cylinder *cylinder)
{
    /* Check radius computation */
    if (fabs(cylinder->radius - cylinder->diameter / 2.0) > EPSILON)
        return (error_invariant_violation("cylinder radius"));
    
    /* Check squared radius */
    if (fabs(cylinder->radius_squared - cylinder->radius * cylinder->radius) > EPSILON)
        return (error_invariant_violation("cylinder radius_squared"));
    
    /* Check half height */
    if (fabs(cylinder->half_height - cylinder->height / 2.0) > EPSILON)
        return (error_invariant_violation("cylinder half_height"));
    
    /* Check axis normalization */
    double axis_length = vec3_length(cylinder->axis);
    if (fabs(axis_length - 1.0) > EPSILON)
        return (error_invariant_violation("cylinder axis not normalized"));
    
    /* Check positive values */
    if (cylinder->diameter <= 0 || cylinder->height <= 0 ||
        cylinder->radius <= 0 || cylinder->radius_squared <= 0 ||
        cylinder->half_height <= 0)
        return (error_invariant_violation("cylinder non-positive values"));
    
    return (1);
}
#endif
```

---

## Testing Requirements

### Unit Tests: Parsing

```c
/* Test: Sphere parsing computes correct cached values */
void test_sphere_parsing(void)
{
    t_sphere sphere;
    char line[] = "sp 0 0 0 10.0 255 0 0";
    
    int result = parse_sphere(line, &sphere);
    
    assert(result == 1);
    assert(sphere.diameter == 10.0);
    assert(sphere.radius == 5.0);           /* diameter / 2 */
    assert(sphere.radius_squared == 25.0);  /* 5 * 5 */
    
    printf("✓ Sphere parsing test passed\n");
}

/* Test: Cylinder parsing computes all cached values */
void test_cylinder_parsing(void)
{
    t_cylinder cyl;
    char line[] = "cy 0 0 0 0 1 0 4.0 10.0 0 255 0";
    
    int result = parse_cylinder(line, &cyl);
    
    assert(result == 1);
    assert(cyl.diameter == 4.0);
    assert(cyl.height == 10.0);
    assert(cyl.radius == 2.0);              /* diameter / 2 */
    assert(cyl.radius_squared == 4.0);      /* 2 * 2 */
    assert(cyl.half_height == 5.0);         /* height / 2 */
    assert(fabs(vec3_length(cyl.axis) - 1.0) < EPSILON);  /* normalized */
    
    printf("✓ Cylinder parsing test passed\n");
}

/* Test: Invalid diameter rejected */
void test_invalid_diameter(void)
{
    t_sphere sphere;
    char line[] = "sp 0 0 0 -5.0 255 0 0";  /* Negative diameter */
    
    int result = parse_sphere(line, &sphere);
    
    assert(result == 0);  /* Parse should fail */
    
    printf("✓ Invalid diameter rejection test passed\n");
}
```

### Integration Tests: Rendering

```c
/* Test: Precomputed values produce identical rendering */
void test_rendering_equivalence(void)
{
    /* Render scene with precomputed values */
    t_scene scene = load_scene("test_sphere.rt");
    render_image("output_new.ppm", &scene);
    
    /* Compare with reference image */
    int pixels_different = compare_images("output_new.ppm", "reference.ppm");
    
    assert(pixels_different == 0);  /* Pixel-perfect match */
    
    printf("✓ Rendering equivalence test passed\n");
}
```

### Performance Tests

```c
/* Benchmark: Measure performance improvement */
void benchmark_cached_values(void)
{
    t_scene scene = load_scene("complex_scene.rt");
    
    clock_t start = clock();
    
    /* Render 100 frames */
    for (int i = 0; i < 100; i++)
        render_frame(&scene);
    
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("Time with cached values: %f seconds\n", time_spent);
    /* Expected: 3-5% improvement over non-cached version */
}
```

---

## Migration Guide

### Step-by-Step Migration

**Step 1**: Update structure definitions in `includes/objects.h`
```c
/* Add new fields to t_sphere and t_cylinder */
```

**Step 2**: Update parsing code in `src/parser/parse_objects.c`
```c
/* Add computation of cached values after parsing diameter/height */
sphere->radius = sphere->diameter / 2.0;
sphere->radius_squared = sphere->radius * sphere->radius;
```

**Step 3**: Update intersection functions
```c
/* Replace: radius = sphere->diameter / 2.0; */
/* With:    radius = sphere->radius; */

/* Replace: radius * radius */
/* With:    sphere->radius_squared */
```

**Step 4**: Validation
```c
/* Run tests to verify identical output */
/* Run valgrind to verify no memory issues */
/* Run norminette to verify coding style */
```

---

## Summary

This contract defines how precomputed geometry properties are integrated into object structures. By computing values once during parsing and storing them in structures, the refactoring eliminates redundant arithmetic operations in the render loop while maintaining code clarity and correctness.

**Key Benefits**:
- Eliminates divisions in render loop (hot path)
- Reduces multiplications in intersection tests
- Improves code readability (self-documenting cached values)
- Minimal memory cost (~16-24 bytes per object)
- Zero risk of stale values (immutable after parsing)

**Compliance**:
- ✅ 42 School constraints (no external libraries)
- ✅ Memory safe (no dynamic allocation in refactored code)
- ✅ Norminette compliant
- ✅ Pixel-perfect output maintained
