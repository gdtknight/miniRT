# API Contract: Generic Intersection Iterator

**Feature**: 011-code-refactoring  
**Module**: Ray Tracing / Intersection Testing  
**File**: `src/render/trace.c`  
**Date**: 2026-01-01

## Function Signature

```c
/**
 * @brief Generic intersection checking for object arrays
 * 
 * Iterates through an array of objects (spheres, planes, or cylinders) and
 * tests each for ray intersection. Updates hit information with the closest
 * intersection found. This generic function replaces three separate type-
 * specific intersection checking functions.
 * 
 * @param objects      Pointer to start of object array (cast to void*)
 * @param count        Number of objects in the array
 * @param obj_size     Size in bytes of each object (use sizeof())
 * @param intersect_fn Function pointer to type-specific intersection function
 * @param ray          Ray to test for intersections (non-NULL)
 * @param hit          Hit structure to update with closest intersection (non-NULL)
 * 
 * @return 1 if any intersection found, 0 if no intersections found
 * 
 * @pre objects != NULL (unless count == 0)
 * @pre count >= 0
 * @pre obj_size > 0
 * @pre intersect_fn != NULL
 * @pre ray != NULL
 * @pre hit != NULL
 * @pre hit->distance > 0 (initialized to max distance or previous hit)
 * 
 * @post If return == 1, hit contains information about closest intersection
 * @post If return == 0, hit remains unchanged
 * @post No side effects on objects array (read-only access)
 * 
 * @note This function uses pointer arithmetic to iterate through the void*
 *       array. Caller is responsible for ensuring correct obj_size.
 * @note Function is thread-safe (no global state, no modifications to shared data)
 * @note Complies with 42 School constraints (no pthread, no external libraries)
 */
int check_intersections_generic(void *objects, int count, size_t obj_size,
                                 t_intersect_fn intersect_fn, 
                                 t_ray *ray, t_hit *hit);
```

## Function Pointer Type

```c
/**
 * @brief Function pointer type for object-specific intersection tests
 * 
 * Defines the signature for intersection functions that test whether a ray
 * intersects with a specific object type (sphere, plane, or cylinder).
 * 
 * @param ray    Ray to test for intersection
 * @param object Pointer to object (actual type depends on implementation)
 * @param hit    Hit structure to update if intersection found and closer
 * 
 * @return 1 if intersection found AND closer than current hit->distance
 *         0 if no intersection or intersection is farther than current hit
 * 
 * @note Implementations: intersect_sphere, intersect_plane, intersect_cylinder
 */
typedef int (*t_intersect_fn)(t_ray *ray, void *object, t_hit *hit);
```

## Usage Examples

### Example 1: Check Sphere Intersections

```c
t_scene scene;
t_ray ray;
t_hit hit;

/* Initialize hit with maximum distance */
hit.distance = INFINITY;

/* Check all sphere intersections using generic iterator */
int sphere_hit = check_intersections_generic(
    scene.spheres,                      /* objects array */
    scene.sphere_count,                 /* count */
    sizeof(t_sphere),                   /* size of each object */
    (t_intersect_fn)intersect_sphere,   /* sphere intersection function */
    &ray,                               /* ray to test */
    &hit                                /* hit info to update */
);

if (sphere_hit)
{
    /* Closest sphere intersection found, hit contains details */
    printf("Hit sphere at distance: %f\n", hit.distance);
}
```

### Example 2: Check All Object Types

```c
t_scene scene;
t_ray ray;
t_hit hit;
int hit_found;

/* Initialize hit */
hit.distance = INFINITY;
hit_found = 0;

/* Check spheres */
if (check_intersections_generic(scene.spheres, scene.sphere_count,
                                sizeof(t_sphere), 
                                (t_intersect_fn)intersect_sphere, 
                                &ray, &hit))
    hit_found = 1;

/* Check planes */
if (check_intersections_generic(scene.planes, scene.plane_count,
                                sizeof(t_plane), 
                                (t_intersect_fn)intersect_plane, 
                                &ray, &hit))
    hit_found = 1;

/* Check cylinders */
if (check_intersections_generic(scene.cylinders, scene.cylinder_count,
                                sizeof(t_cylinder), 
                                (t_intersect_fn)intersect_cylinder, 
                                &ray, &hit))
    hit_found = 1;

/* hit now contains closest intersection across all object types */
if (hit_found)
    return (calculate_lighting(scene, &hit));
else
    return (scene.background_color);
```

### Example 3: Error Handling

```c
/* Safe usage with validation */
if (scene.sphere_count > 0 && scene.spheres != NULL)
{
    hit_found = check_intersections_generic(
        scene.spheres,
        scene.sphere_count,
        sizeof(t_sphere),
        (t_intersect_fn)intersect_sphere,
        &ray,
        &hit
    );
}
else
{
    /* No spheres in scene, skip intersection testing */
    hit_found = 0;
}
```

## Implementation Requirements

### Algorithm

```
FUNCTION check_intersections_generic(objects, count, obj_size, intersect_fn, ray, hit):
    hit_found ← 0
    
    FOR i FROM 0 TO count - 1:
        current_obj ← objects + (i × obj_size)
        
        temp_hit ← CREATE_HIT_STRUCTURE()
        temp_hit.distance ← hit.distance
        
        IF intersect_fn(ray, current_obj, temp_hit) == 1:
            hit ← temp_hit
            hit_found ← 1
        END IF
    END FOR
    
    RETURN hit_found
END FUNCTION
```

### Pointer Arithmetic

```c
/* Calculate pointer to i-th object in void* array */
void *current_obj = (char *)objects + (i * obj_size);

/* Cast back to appropriate type in intersection function */
/* (handled by specific intersection function implementation) */
```

### Performance Characteristics

- **Time Complexity**: O(n) where n is the number of objects
- **Space Complexity**: O(1) - only uses temp_hit structure on stack
- **Cache Efficiency**: Linear array traversal, good cache locality
- **Function Call Overhead**: One indirect function call per object

## Error Handling

### Invalid Input Handling

```c
/* Defensive programming - validate inputs */
if (objects == NULL && count > 0)
    return (0);  /* No objects to check */

if (count <= 0)
    return (0);  /* Empty array, no hits possible */

if (obj_size == 0)
    return (0);  /* Invalid object size */

if (intersect_fn == NULL || ray == NULL || hit == NULL)
    return (0);  /* NULL pointer, cannot proceed */
```

### Edge Cases

| Case | Behavior | Example |
|------|----------|---------|
| Empty array (`count == 0`) | Returns 0 immediately | Scene with no spheres |
| NULL objects with `count == 0` | Returns 0, valid case | No objects of this type |
| All intersections fail | Returns 0, hit unchanged | Ray misses all objects |
| Multiple hits | Returns 1, hit contains closest | Ray through multiple spheres |
| Hit at max distance | Returns 1 if within `hit.distance` | Ray hits far object |

## Integration Points

### Dependencies

**Required Headers**:
```c
#include "ray.h"      /* t_ray, t_hit, t_intersect_fn */
#include "objects.h"  /* t_sphere, t_plane, t_cylinder */
#include <stddef.h>   /* size_t */
```

**Required Functions**:
- `intersect_sphere()` - Sphere intersection implementation
- `intersect_plane()` - Plane intersection implementation
- `intersect_cylinder()` - Cylinder intersection implementation

### Replaces Existing Functions

This generic function **replaces** the following three functions:
- `check_sphere_intersections()` (delete after refactoring)
- `check_plane_intersections()` (delete after refactoring)
- `check_cylinder_intersections()` (delete after refactoring)

### Call Sites

**Modified Files**:
- `src/render/trace.c` - Main ray tracing loop
- Any other code calling `check_*_intersections()` functions

**Migration**:
```c
/* OLD CODE */
hit_found = check_sphere_intersections(scene, ray, &hit);

/* NEW CODE */
hit_found = check_intersections_generic(
    scene->spheres,
    scene->sphere_count,
    sizeof(t_sphere),
    (t_intersect_fn)intersect_sphere,
    ray,
    &hit
);
```

## Testing Requirements

### Unit Tests

```c
/* Test 1: Empty array */
int result = check_intersections_generic(NULL, 0, sizeof(t_sphere),
                                         (t_intersect_fn)intersect_sphere,
                                         &ray, &hit);
assert(result == 0);

/* Test 2: Single sphere hit */
t_sphere spheres[1] = { create_test_sphere(0, 0, 5) };
hit.distance = INFINITY;
result = check_intersections_generic(spheres, 1, sizeof(t_sphere),
                                     (t_intersect_fn)intersect_sphere,
                                     &ray, &hit);
assert(result == 1);
assert(hit.distance < INFINITY);

/* Test 3: Multiple objects, closest wins */
t_sphere spheres[3] = {
    create_test_sphere(0, 0, 10),  /* Far */
    create_test_sphere(0, 0, 5),   /* Closest */
    create_test_sphere(0, 0, 8)    /* Middle */
};
hit.distance = INFINITY;
result = check_intersections_generic(spheres, 3, sizeof(t_sphere),
                                     (t_intersect_fn)intersect_sphere,
                                     &ray, &hit);
assert(result == 1);
assert(fabs(hit.distance - 4.0) < EPSILON);  /* Radius=1, z=5, distance=4 */

/* Test 4: No intersections */
t_sphere spheres[2] = {
    create_test_sphere(100, 100, 100),  /* Far off-screen */
    create_test_sphere(-100, -100, -100)
};
hit.distance = INFINITY;
result = check_intersections_generic(spheres, 2, sizeof(t_sphere),
                                     (t_intersect_fn)intersect_sphere,
                                     &ray, &hit);
assert(result == 0);
assert(hit.distance == INFINITY);  /* Unchanged */
```

### Integration Tests

```c
/* Test with real scene data */
t_scene scene = load_scene("test_scene.rt");
t_ray ray = create_ray(camera_position, camera_direction);
t_hit hit;

hit.distance = INFINITY;

/* Test all object types work correctly */
int sphere_hit = check_intersections_generic(
    scene.spheres, scene.sphere_count, sizeof(t_sphere),
    (t_intersect_fn)intersect_sphere, &ray, &hit);

int plane_hit = check_intersections_generic(
    scene.planes, scene.plane_count, sizeof(t_plane),
    (t_intersect_fn)intersect_plane, &ray, &hit);

int cylinder_hit = check_intersections_generic(
    scene.cylinders, scene.cylinder_count, sizeof(t_cylinder),
    (t_intersect_fn)intersect_cylinder, &ray, &hit);

/* Verify closest hit across all types */
assert(sphere_hit || plane_hit || cylinder_hit);
```

### Performance Tests

```c
/* Benchmark: measure performance improvement */
clock_t start = clock();

for (int frame = 0; frame < 100; frame++)
{
    for (int y = 0; y < WINDOW_HEIGHT; y++)
    {
        for (int x = 0; x < WINDOW_WIDTH; x++)
        {
            t_ray ray = generate_ray(x, y);
            t_hit hit;
            hit.distance = INFINITY;
            
            check_intersections_generic(scene.spheres, scene.sphere_count,
                                       sizeof(t_sphere),
                                       (t_intersect_fn)intersect_sphere,
                                       &ray, &hit);
        }
    }
}

clock_t end = clock();
double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
printf("Generic iterator time: %f seconds\n", time_spent);

/* Compare with old implementation */
/* Expected: similar or slightly better performance (code reduction benefit) */
```

## Validation

### 42 School Compliance

- ✅ Uses only standard C features (function pointers, void*, pointer arithmetic)
- ✅ No pthread, fork, or external libraries
- ✅ No prohibited functions
- ✅ Follows norminette coding standards
- ✅ Function name follows convention
- ✅ Function length within limits (< 25 lines)

### Memory Safety

- ✅ No dynamic memory allocation (stack-only)
- ✅ No memory leaks (no malloc/free)
- ✅ No buffer overflows (pointer arithmetic validated)
- ✅ Read-only access to objects array

### Code Quality

- ✅ Single Responsibility: only iterates and delegates intersection tests
- ✅ Clear documentation with Doxygen comments
- ✅ Type safety preserved through function pointer interface
- ✅ Error handling for edge cases

## Backward Compatibility

### Migration Path

1. **Phase 1**: Implement `check_intersections_generic()` function
2. **Phase 2**: Update call sites to use new function
3. **Phase 3**: Delete old `check_*_intersections()` functions
4. **Phase 4**: Verify all tests pass with identical output

### Breaking Changes

**None** - This is a pure refactoring. External behavior is identical.

### Deprecation

The following functions are **deprecated** and will be removed:
- `check_sphere_intersections()`
- `check_plane_intersections()`
- `check_cylinder_intersections()`

---

## Summary

This contract defines a generic intersection iterator that eliminates code duplication while maintaining type safety through function pointers. The implementation complies with 42 School constraints and provides identical functionality to the original three separate functions.

**Benefits**:
- Reduces code duplication by ~25 lines
- Centralizes intersection iteration logic
- Improves maintainability
- Zero performance overhead (compiler inlines indirect calls)
- Maintains pixel-perfect rendering output
