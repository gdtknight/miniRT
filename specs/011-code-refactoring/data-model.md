# Data Model: Code Refactoring for Calculation Optimization

**Feature**: 011-code-refactoring  
**Date**: 2026-01-01  
**Status**: Complete

## Overview

This document defines the data structure changes required to support calculation caching and precomputed geometry properties. The refactoring focuses on extending existing object structures while maintaining backward compatibility with the scene file parsing system.

---

## 1. Extended Object Structures

### 1.1 Sphere Structure (Enhanced)

**File**: `includes/objects.h`

```c
/* Sphere object with precomputed geometry properties for optimized intersection testing */
typedef struct s_sphere
{
    t_vec3  center;          /* Center point in 3D space */
    double  diameter;        /* Diameter as specified in scene file */
    t_color color;           /* RGB color [0-255] */
    double  radius;          /* Precomputed: diameter / 2.0 */
    double  radius_squared;  /* Precomputed: radius * radius for intersection tests */
}   t_sphere;
```

**Changes from Original**:
- **Added**: `radius` field (8 bytes)
- **Added**: `radius_squared` field (8 bytes)
- **Total size increase**: 16 bytes per sphere

**Rationale**:
- Eliminates division operation (`diameter / 2.0`) in every intersection test
- Eliminates multiplication operation (`radius * radius`) in quadratic formula
- Computed once during parsing, accessed millions of times during rendering

**Validation Rules**:
- `radius > 0` (must be positive)
- `radius == diameter / 2.0` (mathematical invariant)
- `radius_squared == radius * radius` (mathematical invariant)

**State Transitions**:
1. **Parsing**: Scene file → Parse diameter → Compute radius and radius_squared
2. **Rendering**: Access cached `radius` and `radius_squared` fields directly
3. **No Runtime Updates**: Values are immutable after parsing (static scene)

---

### 1.2 Cylinder Structure (Enhanced)

**File**: `includes/objects.h`

```c
/* Finite cylinder object with precomputed geometry properties */
typedef struct s_cylinder
{
    t_vec3  center;          /* Center point of cylinder */
    t_vec3  axis;            /* Normalized axis direction vector */
    double  diameter;        /* Diameter as specified in scene file */
    double  height;          /* Height as specified in scene file */
    t_color color;           /* RGB color [0-255] */
    double  radius;          /* Precomputed: diameter / 2.0 */
    double  radius_squared;  /* Precomputed: radius * radius */
    double  half_height;     /* Precomputed: height / 2.0 for cap calculations */
}   t_cylinder;
```

**Changes from Original**:
- **Added**: `radius` field (8 bytes)
- **Added**: `radius_squared` field (8 bytes)
- **Added**: `half_height` field (8 bytes)
- **Total size increase**: 24 bytes per cylinder

**Rationale**:
- Eliminates `diameter / 2.0` division in cylinder body intersection
- Eliminates `radius * radius` multiplication in quadratic formula
- Eliminates `height / 2.0` division in cylinder cap calculations
- Cylinder intersection is the most computationally expensive primitive

**Validation Rules**:
- `radius > 0` (must be positive)
- `height > 0` (must be positive)
- `radius == diameter / 2.0` (mathematical invariant)
- `radius_squared == radius * radius` (mathematical invariant)
- `half_height == height / 2.0` (mathematical invariant)
- `vec3_length(axis) == 1.0` (axis must be normalized)

**State Transitions**:
1. **Parsing**: Scene file → Parse diameter/height → Compute cached values
2. **Rendering**: Access cached fields directly in intersection tests
3. **No Runtime Updates**: Values are immutable (static scene)

---

### 1.3 Plane Structure (Unchanged)

**File**: `includes/objects.h`

```c
/* Infinite plane defined by a point, normal vector, and color */
typedef struct s_plane
{
    t_vec3  point;   /* Any point on the plane */
    t_vec3  normal;  /* Normalized normal vector perpendicular to plane */
    t_color color;   /* RGB color [0-255] */
}   t_plane;
```

**No Changes Required**:
- Plane intersection already optimal (no redundant calculations)
- No repeated arithmetic operations to cache
- Structure remains unchanged

---

## 2. Cached Calculation Structures

### 2.1 Cylinder Intersection Cache

**Purpose**: Store intermediate dot product calculations to eliminate redundancy in cylinder intersection tests.

**Implementation**: Local variables in `calculate_cylinder_intersection()` function (not a new struct type).

```c
/* Cached dot products for cylinder intersection (local variables) */
double dir_dot_axis;   /* vec3_dot(ray->direction, cyl->axis) - used 3× */
double oc_dot_axis;    /* vec3_dot(oc, cyl->axis) - used 2× */
double dir_dot_dir;    /* vec3_dot(ray->direction, ray->direction) - used 1× */
double oc_dot_oc;      /* vec3_dot(oc, oc) - used 1× */
```

**Lifetime**: Function scope only (stack-allocated, automatic cleanup)

**Relationships**:
- Used in quadratic formula coefficients (a, b, c)
- Each cached value used multiple times within same function
- No cross-function sharing (values specific to one intersection test)

---

### 2.2 Normalized Device Coordinates (NDC)

**Purpose**: Structure to hold screen-to-NDC coordinate conversion results.

**Implementation**: Passed as output parameters (not a dedicated struct for this refactoring).

```c
/* Screen to NDC conversion - inline function parameters */
void screen_to_ndc(int x, int y, double *u, double *v);

/* Where:
 * u: Horizontal NDC coordinate in range [-1, 1] (left to right)
 * v: Vertical NDC coordinate in range [-1, 1] (bottom to top)
 */
```

**Calculation Formula**:
- `u = (2.0 * x / WINDOW_WIDTH) - 1.0`
- `v = 1.0 - (2.0 * y / WINDOW_HEIGHT)`

**Usage Context**:
- Called from render functions to convert pixel coordinates
- Results used to compute ray direction through pixel
- No persistent storage required

---

## 3. Generic Intersection Function Interface

### 3.1 Function Pointer Type

**Purpose**: Enable generic intersection iteration over different object types.

**File**: `includes/ray.h` or create `includes/intersection.h`

```c
/*
** Generic intersection function pointer type.
** 
** Parameters:
**   ray     - Ray to test for intersection
**   object  - Pointer to object (sphere, plane, or cylinder)
**   hit     - Hit information to update if intersection found
** 
** Returns:
**   1 if intersection found and closer than current hit->distance
**   0 otherwise
*/
typedef int (*t_intersect_fn)(t_ray *ray, void *object, t_hit *hit);
```

**Concrete Implementations**:
- `intersect_sphere(t_ray *ray, t_sphere *sphere, t_hit *hit) → int`
- `intersect_plane(t_ray *ray, t_plane *plane, t_hit *hit) → int`
- `intersect_cylinder(t_ray *ray, t_cylinder *cylinder, t_hit *hit) → int`

**Type Safety**:
- Existing intersection functions already match this signature
- Cast to `t_intersect_fn` when passing to generic iterator
- No runtime type checking needed (caller ensures correct object type)

---

### 3.2 Generic Intersection Iterator

**Purpose**: Abstract the repetitive "loop over objects and find closest hit" pattern.

```c
/*
** Generic intersection checking for object arrays.
** Iterates through object array and updates hit with closest intersection.
** 
** Parameters:
**   objects      - Pointer to object array (spheres, planes, or cylinders)
**   count        - Number of objects in array
**   obj_size     - Size of each object (sizeof(t_sphere), etc.)
**   intersect_fn - Function pointer to appropriate intersection test
**   ray          - Ray to test for intersections
**   hit          - Hit information, updated with closest intersection
** 
** Returns:
**   1 if any intersection found
**   0 if no intersections found
*/
int check_intersections_generic(void *objects, int count, size_t obj_size,
                                 t_intersect_fn intersect_fn, 
                                 t_ray *ray, t_hit *hit);
```

**Implementation Location**: `src/render/trace.c` (replaces 3 separate functions)

**Internal Logic**:
```
1. Initialize: hit_found = 0
2. For each object in array:
   a. Calculate pointer: current_obj = objects + (i * obj_size)
   b. Create temp_hit with current hit->distance
   c. Call intersect_fn(ray, current_obj, &temp_hit)
   d. If intersection found and closer:
      - Update *hit = temp_hit
      - Set hit_found = 1
3. Return hit_found
```

**Usage Example**:
```c
/* Check sphere intersections using generic iterator */
hit_found = check_intersections_generic(
    scene->spheres,                    /* objects */
    scene->sphere_count,               /* count */
    sizeof(t_sphere),                  /* obj_size */
    (t_intersect_fn)intersect_sphere,  /* intersect_fn */
    ray,                               /* ray */
    &hit                               /* hit */
);
```

---

## 4. Entity Relationships

### 4.1 Object Hierarchy

```
t_scene (root)
├── t_camera (1)
├── t_ambient_light (1)
├── t_light[] (array of point lights)
├── t_sphere[] (array of spheres)
│   └── Each sphere has precomputed: radius, radius_squared
├── t_plane[] (array of planes)
│   └── No cached values needed
└── t_cylinder[] (array of cylinders)
    └── Each cylinder has precomputed: radius, radius_squared, half_height
```

**Relationships**:
- One-to-many: Scene contains multiple objects of each type
- No cross-references: Objects are independent (no object-to-object pointers)
- Immutable after parsing: No runtime object creation or modification

---

### 4.2 Render Pipeline Data Flow

```
Scene File (*.rt)
    ↓ [Parser]
Object Structures (with precomputed values)
    ↓ [Render Loop]
Ray Generation (using cached NDC conversion)
    ↓ [Intersection Testing]
Generic Iterator (using function pointers)
    ↓ [Specific Intersection Functions]
Cached Dot Products (local variables)
    ↓ [Hit Detection]
Closest Hit Information
    ↓ [Lighting Calculation]
Final Pixel Color
```

**Key Data Dependencies**:
1. Precomputed geometry values set during parsing
2. Generic iterator delegates to specific intersection functions
3. Intersection functions use cached calculations internally
4. No runtime recomputation of static values

---

## 5. Memory Layout Considerations

### 5.1 Structure Sizes

| Structure     | Original Size | New Size | Increase | Alignment |
|---------------|---------------|----------|----------|-----------|
| `t_sphere`    | 32 bytes      | 48 bytes | +16 bytes| 8-byte    |
| `t_cylinder`  | 56 bytes      | 80 bytes | +24 bytes| 8-byte    |
| `t_plane`     | 32 bytes      | 32 bytes | 0 bytes  | 8-byte    |

**Alignment Notes**:
- All structures remain 8-byte aligned (no padding changes)
- Double fields naturally aligned on 8-byte boundaries
- No cache line issues (structures fit within single cache line)

---

### 5.2 Memory Impact Per Scene

**Example Scene**: 10 spheres, 10 cylinders, 5 planes

**Before**:
- Spheres: 10 × 32 = 320 bytes
- Cylinders: 10 × 56 = 560 bytes
- Planes: 5 × 32 = 160 bytes
- **Total**: 1,040 bytes

**After**:
- Spheres: 10 × 48 = 480 bytes (+160 bytes)
- Cylinders: 10 × 80 = 800 bytes (+240 bytes)
- Planes: 5 × 32 = 160 bytes (unchanged)
- **Total**: 1,440 bytes (+400 bytes, +38% increase)

**Impact Analysis**:
- Absolute increase: ~400 bytes for typical scene
- Negligible on modern systems (KB vs MB/GB memory)
- Trade-off: Small memory cost for significant CPU savings
- Benefit: Eliminates millions of arithmetic operations per frame

---

### 5.3 Cache Efficiency

**Cache Line Size**: 64 bytes (typical modern CPU)

**Sphere Access Pattern**:
- Original: 32 bytes (fits in 1 cache line)
- Enhanced: 48 bytes (fits in 1 cache line)
- **Result**: No additional cache misses

**Cylinder Access Pattern**:
- Original: 56 bytes (spans 1-2 cache lines depending on alignment)
- Enhanced: 80 bytes (spans 2 cache lines)
- **Result**: Possible 1 additional cache miss, but offset by eliminating arithmetic operations

**Overall**: Memory access is still the bottleneck, not computation, so adding cached values improves performance despite slightly larger structures.

---

## 6. Validation Rules Summary

### 6.1 Parsing Stage Validation

**Sphere Validation**:
```c
/* After parsing sphere from file */
if (sphere->diameter <= 0)
    return (error_invalid_diameter());

/* Compute cached values */
sphere->radius = sphere->diameter / 2.0;
sphere->radius_squared = sphere->radius * sphere->radius;

/* Validate cached values (debug assertions) */
assert(sphere->radius > 0);
assert(sphere->radius_squared > 0);
```

**Cylinder Validation**:
```c
/* After parsing cylinder from file */
if (cylinder->diameter <= 0 || cylinder->height <= 0)
    return (error_invalid_geometry());

/* Ensure axis is normalized */
cylinder->axis = vec3_normalize(cylinder->axis);

/* Compute cached values */
cylinder->radius = cylinder->diameter / 2.0;
cylinder->radius_squared = cylinder->radius * cylinder->radius;
cylinder->half_height = cylinder->height / 2.0;

/* Validate cached values (debug assertions) */
assert(cylinder->radius > 0);
assert(cylinder->radius_squared > 0);
assert(cylinder->half_height > 0);
assert(fabs(vec3_length(cylinder->axis) - 1.0) < EPSILON);
```

---

### 6.2 Runtime Invariants

**Immutability Guarantee**:
- All precomputed values set once during parsing
- No function modifies object structures during rendering
- Values remain constant throughout program execution

**Consistency Checks** (for debugging):
```c
/* Optional runtime validation (can be disabled in release builds) */
#ifdef DEBUG
    assert(fabs(sphere->radius - sphere->diameter / 2.0) < EPSILON);
    assert(fabs(sphere->radius_squared - sphere->radius * sphere->radius) < EPSILON);
#endif
```

---

## 7. Migration Impact

### 7.1 Affected Files

**Structure Definitions**:
- `includes/objects.h` - Add cached fields to t_sphere and t_cylinder

**Parsing Logic**:
- `src/parser/parse_objects.c` - Compute cached values after parsing diameter/height

**Intersection Functions**:
- `src/ray/intersections.c` - Use cached radius/radius_squared for spheres
- `src/ray/intersect_cylinder.c` - Use cached values, add dot product caching
- `src/lighting/shadow_calc.c` - Use cached radius values in shadow ray tests

**Rendering Pipeline**:
- `src/render/trace.c` - Replace 3 functions with generic iterator
- `src/render/render.c` - Use screen_to_ndc() helper function

---

### 7.2 Backward Compatibility

**Scene File Format**: NO CHANGES
- Scene files (*.rt) remain unchanged
- Parser still reads diameter, height, etc. from files
- Cached values computed internally after parsing

**API Stability**:
- Existing function signatures unchanged
- Intersection functions still accept same parameters
- Generic iterator is new addition, doesn't break existing code

**Testing Strategy**:
- Verify identical rendering output (pixel-perfect comparison)
- Run all existing test scenes
- No changes to expected behavior

---

## Summary

### Key Entities Added

1. **Cached Sphere Geometry**: `radius`, `radius_squared` fields
2. **Cached Cylinder Geometry**: `radius`, `radius_squared`, `half_height` fields
3. **Cached Dot Products**: Local variables in cylinder intersection
4. **Generic Intersection Iterator**: Function pointer-based abstraction
5. **Screen-to-NDC Helper**: Inline coordinate conversion function

### Validation Summary

- ✅ All cached values computed once during parsing
- ✅ Mathematical invariants maintained (radius = diameter/2, etc.)
- ✅ No runtime modification of object structures
- ✅ Scene file format unchanged
- ✅ Backward compatible with existing codebase

### Memory Impact

- Typical scene: +400 bytes (+38% in object structures)
- Negligible absolute cost on modern systems
- Significant performance benefit (eliminates millions of operations)

### Ready for Contract Generation

All data structures defined and validated. Ready to proceed to Phase 1 contract generation and quickstart documentation.
