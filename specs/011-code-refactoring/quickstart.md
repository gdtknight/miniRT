# Quickstart Guide: Code Refactoring Implementation

**Feature**: 011-code-refactoring  
**For**: Developers implementing calculation optimization refactoring  
**Date**: 2026-01-01

## Overview

This guide provides step-by-step instructions for implementing the code refactoring optimizations in miniRT. Follow these steps in order to ensure correct implementation and maintain 42 School compliance.

---

## Prerequisites

### Before You Start

✅ **Read these documents first**:
1. `spec.md` - Understand the requirements and acceptance criteria
2. `research.md` - Review identified optimization opportunities
3. `data-model.md` - Understand structure changes
4. `contracts/` - Review API contracts for new interfaces

✅ **Verify your environment**:
```bash
# Ensure project builds cleanly
make clean && make

# Run existing tests
./test_miniRT.sh

# Check for memory leaks
valgrind --leak-check=full ./miniRT scenes/test.rt

# Verify norminette compliance
norminette src/ includes/
```

✅ **Create feature branch**:
```bash
git checkout -b 011-code-refactoring
git push -u origin 011-code-refactoring
```

---

## Implementation Steps

### Phase 1: Precomputed Geometry Properties (Est. 2-3 hours)

This phase adds cached radius and height values to object structures.

#### Step 1.1: Update Object Structures

**File**: `includes/objects.h`

```c
/* Modify t_sphere structure */
typedef struct s_sphere
{
    t_vec3  center;
    double  diameter;
    t_color color;
    double  radius;          /* ADD THIS */
    double  radius_squared;  /* ADD THIS */
}   t_sphere;

/* Modify t_cylinder structure */
typedef struct s_cylinder
{
    t_vec3  center;
    t_vec3  axis;
    double  diameter;
    double  height;
    t_color color;
    double  radius;          /* ADD THIS */
    double  radius_squared;  /* ADD THIS */
    double  half_height;     /* ADD THIS */
}   t_cylinder;
```

**Commit checkpoint**: `git commit -m "Add precomputed geometry fields to object structures"`

---

#### Step 1.2: Update Parsing Code

**File**: `src/parser/parse_objects.c`

Find the sphere parsing function and add computation after diameter parsing:

```c
int parse_sphere(char *line, t_sphere *sphere)
{
    /* Existing parsing code ... */
    
    /* Validate diameter */
    if (sphere->diameter <= 0)
        return (error_invalid_diameter());
    
    /* ADD: Compute precomputed values */
    sphere->radius = sphere->diameter / 2.0;
    sphere->radius_squared = sphere->radius * sphere->radius;
    
    return (1);
}
```

Find the cylinder parsing function and add:

```c
int parse_cylinder(char *line, t_cylinder *cylinder)
{
    /* Existing parsing code ... */
    
    /* Validate geometry */
    if (cylinder->diameter <= 0 || cylinder->height <= 0)
        return (error_invalid_geometry());
    
    /* Normalize axis (if not already done) */
    cylinder->axis = vec3_normalize(cylinder->axis);
    
    /* ADD: Compute precomputed values */
    cylinder->radius = cylinder->diameter / 2.0;
    cylinder->radius_squared = cylinder->radius * cylinder->radius;
    cylinder->half_height = cylinder->height / 2.0;
    
    return (1);
}
```

**Test**:
```bash
make clean && make
./miniRT scenes/test.rt
# Verify scene loads without errors
```

**Commit checkpoint**: `git commit -m "Compute precomputed geometry values during parsing"`

---

#### Step 1.3: Update Sphere Intersection

**File**: `src/ray/intersections.c`

Find the `intersect_sphere()` function and replace radius calculation:

```c
int intersect_sphere(t_ray *ray, t_sphere *sphere, t_hit *hit)
{
    t_vec3  oc;
    double  t;
    
    /* BEFORE: double radius = sphere->diameter / 2.0; */
    /* AFTER: Use precomputed value directly */
    
    oc = vec3_subtract(ray->origin, sphere->center);
    t = calculate_sphere_t(oc, ray, sphere->radius);  /* Use cached radius */
    
    /* ... rest of function ... */
}
```

Update `calculate_sphere_t()` if it uses `radius * radius`:

```c
static double calculate_sphere_t(t_vec3 oc, t_ray *ray, t_sphere *sphere)
{
    /* ... existing code ... */
    
    /* BEFORE: c = vec3_dot(oc, oc) - radius * radius; */
    /* AFTER: Use precomputed squared radius */
    c = vec3_dot(oc, oc) - sphere->radius_squared;
    
    /* ... rest of function ... */
}
```

**Test**:
```bash
make clean && make
./miniRT scenes/sphere_test.rt
# Verify spheres render identically to before
```

**Commit checkpoint**: `git commit -m "Use precomputed radius in sphere intersection"`

---

#### Step 1.4: Update Cylinder Intersection

**File**: `src/ray/intersect_cylinder.c`

Replace radius calculation in `calculate_cylinder_intersection()`:

```c
static int calculate_cylinder_intersection(t_ray *ray, t_cylinder *cyl,
                                            t_cyl_calc *calc)
{
    t_vec3  oc;
    
    /* BEFORE: double radius = cyl->diameter / 2.0; */
    /* AFTER: Use precomputed value */
    
    oc = vec3_subtract(ray->origin, cyl->center);
    
    /* Update quadratic coefficient c */
    /* BEFORE: calc->c = ... - radius * radius; */
    /* AFTER: */
    calc->c = vec3_dot(oc, oc) - vec3_dot(oc, cyl->axis) * 
              vec3_dot(oc, cyl->axis) - cyl->radius_squared;
    
    /* ... rest of function ... */
}
```

Update cylinder cap intersection to use `half_height`:

```c
int intersect_cylinder_cap(t_ray *ray, t_cylinder *cyl, t_hit *hit, double cap_m)
{
    /* BEFORE: if (fabs(cap_m) > cyl->height / 2.0) */
    /* AFTER: Use precomputed half_height */
    if (fabs(cap_m) > cyl->half_height)
        return (0);
    
    /* ... rest of function ... */
}
```

**Test**:
```bash
make clean && make
./miniRT scenes/cylinder_test.rt
# Verify cylinders render correctly
```

**Commit checkpoint**: `git commit -m "Use precomputed radius and half_height in cylinder intersection"`

---

### Phase 2: Cache Duplicate Dot Products (Est. 1-2 hours)

This phase eliminates redundant `vec3_dot()` calculations in cylinder intersection.

#### Step 2.1: Add Local Variable Caching

**File**: `src/ray/intersect_cylinder.c`

Modify `calculate_cylinder_intersection()` to cache dot products:

```c
static int calculate_cylinder_intersection(t_ray *ray, t_cylinder *cyl,
                                            t_cyl_calc *calc)
{
    t_vec3  oc;
    double  dir_dot_axis;
    double  oc_dot_axis;
    double  dir_dot_dir;
    double  oc_dot_oc;
    
    oc = vec3_subtract(ray->origin, cyl->center);
    
    /* Cache all dot products BEFORE using them */
    dir_dot_axis = vec3_dot(ray->direction, cyl->axis);
    oc_dot_axis = vec3_dot(oc, cyl->axis);
    dir_dot_dir = vec3_dot(ray->direction, ray->direction);
    oc_dot_oc = vec3_dot(oc, oc);
    
    /* Use cached values in quadratic coefficients */
    calc->a = dir_dot_dir - dir_dot_axis * dir_dot_axis;
    calc->b = 2.0 * (vec3_dot(ray->direction, oc) - dir_dot_axis * oc_dot_axis);
    calc->c = oc_dot_oc - oc_dot_axis * oc_dot_axis - cyl->radius_squared;
    
    /* ... rest of function unchanged ... */
}
```

**Before/After Comparison**:
```c
/* BEFORE: vec3_dot called 6 times */
calc->a = vec3_dot(ray->direction, ray->direction) - 
          vec3_dot(ray->direction, cyl->axis) * 
          vec3_dot(ray->direction, cyl->axis);  // duplicate!

/* AFTER: vec3_dot called 4 times, cached values reused */
dir_dot_axis = vec3_dot(ray->direction, cyl->axis);  // computed once
calc->a = dir_dot_dir - dir_dot_axis * dir_dot_axis;  // reused
```

**Test**:
```bash
make clean && make
./miniRT scenes/cylinder_complex.rt
# Verify cylinders still render correctly
```

**Commit checkpoint**: `git commit -m "Cache duplicate dot products in cylinder intersection"`

---

### Phase 3: Generic Intersection Iterator (Est. 2-3 hours)

This phase abstracts the repetitive intersection checking pattern.

#### Step 3.1: Add Function Pointer Type

**File**: `includes/ray.h`

Add the function pointer typedef:

```c
/*
** Generic intersection function pointer type.
** Used to abstract intersection testing across different object types.
*/
typedef int (*t_intersect_fn)(t_ray *ray, void *object, t_hit *hit);
```

**Commit checkpoint**: `git commit -m "Add generic intersection function pointer type"`

---

#### Step 3.2: Implement Generic Iterator

**File**: `src/render/trace.c`

Add the new generic function BEFORE the existing check_*_intersections functions:

```c
/*
** Generic intersection checking for object arrays.
** Iterates through objects and updates hit with closest intersection.
*/
int check_intersections_generic(void *objects, int count, size_t obj_size,
                                 t_intersect_fn intersect_fn, 
                                 t_ray *ray, t_hit *hit)
{
    t_hit   temp_hit;
    int     i;
    int     hit_found;
    void    *current_obj;
    
    if (objects == NULL && count > 0)
        return (0);
    hit_found = 0;
    i = 0;
    while (i < count)
    {
        current_obj = (char *)objects + (i * obj_size);
        temp_hit.distance = hit->distance;
        if (intersect_fn(ray, current_obj, &temp_hit))
        {
            *hit = temp_hit;
            hit_found = 1;
        }
        i++;
    }
    return (hit_found);
}
```

**Test** (before removing old functions):
```bash
make clean && make
# Should compile without errors
```

**Commit checkpoint**: `git commit -m "Implement generic intersection iterator"`

---

#### Step 3.3: Update Call Sites

**File**: `src/render/trace.c`

Find the main ray tracing function (probably `trace_ray()` or similar) and update the calls:

```c
/* BEFORE */
if (check_sphere_intersections(scene, ray, &hit))
    hit_found = 1;
if (check_plane_intersections(scene, ray, &hit))
    hit_found = 1;
if (check_cylinder_intersections(scene, ray, &hit))
    hit_found = 1;

/* AFTER */
if (check_intersections_generic(scene->spheres, scene->sphere_count,
                                sizeof(t_sphere),
                                (t_intersect_fn)intersect_sphere,
                                ray, &hit))
    hit_found = 1;

if (check_intersections_generic(scene->planes, scene->plane_count,
                                sizeof(t_plane),
                                (t_intersect_fn)intersect_plane,
                                ray, &hit))
    hit_found = 1;

if (check_intersections_generic(scene->cylinders, scene->cylinder_count,
                                sizeof(t_cylinder),
                                (t_intersect_fn)intersect_cylinder,
                                ray, &hit))
    hit_found = 1;
```

**Test**:
```bash
make clean && make
./miniRT scenes/mixed_objects.rt
# Verify all object types render correctly
```

**Commit checkpoint**: `git commit -m "Update call sites to use generic iterator"`

---

#### Step 3.4: Remove Old Functions

**File**: `src/render/trace.c`

Delete the three old functions:
- `check_sphere_intersections()`
- `check_plane_intersections()`
- `check_cylinder_intersections()`

**Test**:
```bash
make clean && make
# Should compile cleanly (no unused function warnings)
./miniRT scenes/all_tests.rt
# Verify everything still works
```

**Commit checkpoint**: `git commit -m "Remove deprecated intersection checking functions"`

---

### Phase 4: Screen-to-NDC Helper (Est. 30 minutes)

This phase extracts coordinate conversion into a helper function.

#### Step 4.1: Add Helper Function

**File**: `src/render/render.c` (add at top after includes)

```c
/*
** Convert screen pixel coordinates to Normalized Device Coordinates.
** NDC range: u in [-1, 1] (left to right), v in [-1, 1] (bottom to top).
*/
static inline void screen_to_ndc(int x, int y, double *u, double *v)
{
    *u = (2.0 * x / (double)WINDOW_WIDTH) - 1.0;
    *v = 1.0 - (2.0 * y / (double)WINDOW_HEIGHT);
}
```

#### Step 4.2: Replace Duplicate Code

Find all occurrences of the coordinate conversion and replace:

```c
/* BEFORE */
u = (2.0 * x / (double)WINDOW_WIDTH) - 1.0;
v = 1.0 - (2.0 * y / (double)WINDOW_HEIGHT);

/* AFTER */
screen_to_ndc(x, y, &u, &v);
```

**Test**:
```bash
make clean && make
./miniRT scenes/test.rt
# Verify rendering unchanged
```

**Commit checkpoint**: `git commit -m "Extract screen-to-NDC conversion into helper function"`

---

## Validation

### Final Testing Checklist

Run through this checklist before submitting:

```bash
# ✅ 1. Build without warnings or errors
make clean && make

# ✅ 2. Run norminette on all changed files
norminette src/ray/intersections.c
norminette src/ray/intersect_cylinder.c
norminette src/render/trace.c
norminette src/render/render.c
norminette src/parser/parse_objects.c
norminette includes/objects.h
norminette includes/ray.h

# ✅ 3. Test all scene types
./miniRT scenes/sphere_test.rt
./miniRT scenes/plane_test.rt
./miniRT scenes/cylinder_test.rt
./miniRT scenes/mixed_objects.rt

# ✅ 4. Memory leak check
valgrind --leak-check=full --show-leak-kinds=all ./miniRT scenes/test.rt

# ✅ 5. Performance test (compare before/after FPS)
./miniRT scenes/complex_scene.rt  # Note FPS in HUD

# ✅ 6. Visual comparison
# Render same scene before and after, compare pixel-perfect output
```

### Success Criteria Verification

From `spec.md`, verify these outcomes:

- ✅ **SC-001**: 10% reduction in redundant calculations (measure with profiler or count eliminated operations)
- ✅ **SC-002**: 30% reduction in duplicate code (count lines removed: ~25 lines from trace.c)
- ✅ **SC-003**: Identical visual output (pixel-perfect comparison)
- ✅ **SC-004**: 200+ fewer lines (count LOC before/after)
- ✅ **SC-005**: 5-15% rendering performance improvement (FPS comparison)
- ✅ **SC-006**: All tests pass without modification
- ✅ **SC-007**: Improved complexity metrics (fewer duplicate functions)

---

## Common Issues & Solutions

### Issue 1: Segmentation Fault in Generic Iterator

**Symptom**: Crash when calling `check_intersections_generic()`

**Cause**: Incorrect `obj_size` parameter or NULL pointer

**Solution**:
```c
/* Verify you're using sizeof() correctly */
check_intersections_generic(
    scene->spheres,
    scene->sphere_count,
    sizeof(t_sphere),  /* NOT sizeof(t_sphere*) */
    (t_intersect_fn)intersect_sphere,
    ray,
    &hit
);

/* Check for NULL arrays */
if (scene->sphere_count > 0 && scene->spheres != NULL)
    check_intersections_generic(...);
```

---

### Issue 2: Norminette Errors

**Symptom**: Function too long or line too long

**Solution**:
```c
/* Break long function calls into multiple lines */
int hit_found = check_intersections_generic(
    scene->spheres,
    scene->sphere_count,
    sizeof(t_sphere),
    (t_intersect_fn)intersect_sphere,
    ray,
    &hit
);

/* Ensure functions are < 25 lines */
/* Move helper logic into separate static functions if needed */
```

---

### Issue 3: Different Rendering Output

**Symptom**: Image looks different after refactoring

**Cause**: Incorrect cached value computation or dot product caching error

**Solution**:
```c
/* Add debug prints to verify cached values */
printf("DEBUG: sphere radius = %f, expected = %f\n", 
       sphere->radius, sphere->diameter / 2.0);

/* Verify dot product caching is correct */
double manual = vec3_dot(ray->direction, cyl->axis);
printf("DEBUG: cached = %f, manual = %f\n", dir_dot_axis, manual);
```

---

### Issue 4: Memory Leaks

**Symptom**: Valgrind reports leaks

**Cause**: Unlikely from this refactoring (no dynamic allocation added)

**Solution**:
```bash
# Use valgrind to identify leak location
valgrind --leak-check=full --track-origins=yes ./miniRT scenes/test.rt

# Check if leak existed before refactoring
git stash
valgrind --leak-check=full ./miniRT scenes/test.rt
git stash pop
```

---

## Performance Benchmarking

### Measure Performance Improvement

```bash
# 1. Checkout main branch and build
git checkout main
make clean && make

# 2. Run benchmark and note FPS
./miniRT scenes/benchmark.rt
# Note the FPS displayed in HUD: e.g., "25.3 FPS"

# 3. Checkout feature branch
git checkout 011-code-refactoring

# 4. Run same benchmark
./miniRT scenes/benchmark.rt
# Note the improved FPS: e.g., "27.8 FPS"

# 5. Calculate improvement
# (27.8 - 25.3) / 25.3 = 9.9% improvement ✅ meets 5-15% target
```

---

## Code Review Checklist

Before requesting review:

- [ ] All commits have clear, descriptive messages
- [ ] No debug prints left in code
- [ ] All norminette checks pass
- [ ] All memory leaks fixed
- [ ] Performance target achieved (5-15% improvement)
- [ ] Visual output identical (pixel-perfect)
- [ ] Code comments updated
- [ ] No prohibited functions used (42 School compliance)
- [ ] Branch pushed to remote
- [ ] Ready to create pull request

---

## Next Steps

After completing this implementation:

1. **Create Pull Request**:
   ```bash
   git push origin 011-code-refactoring
   # Create PR on GitHub from 011-code-refactoring to main
   ```

2. **Request Code Review**:
   - Link to PR in team chat
   - Highlight performance improvements achieved
   - Note any deviations from plan (if any)

3. **Address Review Comments**:
   - Make requested changes
   - Push updates to feature branch
   - Re-test after changes

4. **Merge After Approval**:
   ```bash
   # After approval, merge via GitHub UI
   # or via command line:
   git checkout main
   git pull origin main
   git merge --no-ff 011-code-refactoring
   git push origin main
   ```

5. **Update Documentation** (if needed):
   - Update Korean documentation in `docs/` if implementation differs from plan
   - Update README if user-facing changes exist

---

## Summary

This quickstart guide walked you through implementing all four refactoring optimizations:

1. ✅ **Precomputed Geometry** - Cached radius, squared radius, half height
2. ✅ **Cached Dot Products** - Eliminated redundant calculations in cylinder intersection
3. ✅ **Generic Iterator** - Abstracted repetitive intersection checking pattern
4. ✅ **Screen-to-NDC Helper** - Extracted coordinate conversion logic

**Estimated Total Time**: 6-9 hours for complete implementation and testing

**Expected Outcomes**:
- 5-15% rendering performance improvement
- 200+ fewer lines of code
- 30% reduction in code duplication
- Identical visual output (pixel-perfect)
- Zero memory leaks
- Full 42 School compliance

**Questions?** Refer back to:
- `research.md` for detailed rationale
- `data-model.md` for structure details
- `contracts/` for API specifications
- `spec.md` for requirements
