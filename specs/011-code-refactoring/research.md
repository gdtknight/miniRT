# Research: Code Refactoring for Calculation Optimization

**Feature**: 011-code-refactoring  
**Date**: 2026-01-01  
**Status**: Complete

## Purpose

This research document identifies specific code duplication patterns, redundant calculations, and refactoring opportunities in the miniRT ray tracing engine. All findings are based on codebase analysis and are constrained to 42 School allowed optimization techniques.

---

## 1. Duplicate Vector Calculations in Cylinder Intersection

### Decision
Cache repeated `vec3_dot()` calculations in `intersect_cylinder.c` by storing intermediate results in local variables.

### Current Code Analysis
Located in `src/ray/intersect_cylinder.c`, lines 42-48:

```c
calc->a = vec3_dot(ray->direction, ray->direction) - 
    vec3_dot(ray->direction, cyl->axis) *     // Computed on line 43
    vec3_dot(ray->direction, cyl->axis);      // DUPLICATE - same calculation line 44

calc->b = 2.0 * (vec3_dot(ray->direction, oc) - 
    vec3_dot(ray->direction, cyl->axis) *     // DUPLICATE - computed 3rd time on line 46
    vec3_dot(oc, cyl->axis));                 

calc->c = vec3_dot(oc, oc) - 
    vec3_dot(oc, cyl->axis) *                 // Computed on line 47
    vec3_dot(oc, cyl->axis) -                 // DUPLICATE - same calculation line 48
    radius * radius;
```

**Findings**:
- `vec3_dot(ray->direction, cyl->axis)` computed 3 times (lines 43, 44, 46)
- `vec3_dot(oc, cyl->axis)` computed 2 times (lines 47, 48)
- `vec3_dot(ray->direction, ray->direction)` computed once but could benefit from naming
- `vec3_dot(oc, oc)` computed once

### Rationale
Dot product operations involve 3 multiplications and 2 additions (5 floating-point operations each). Caching these values eliminates 3 redundant dot product calls per cylinder intersection test, saving 15 floating-point operations in a hot path function called millions of times per frame.

### Implementation Approach
```c
// Cache dot products before quadratic calculation
double dir_dot_axis = vec3_dot(ray->direction, cyl->axis);
double oc_dot_axis = vec3_dot(oc, cyl->axis);
double dir_dot_dir = vec3_dot(ray->direction, ray->direction);
double oc_dot_oc = vec3_dot(oc, oc);

calc->a = dir_dot_dir - dir_dot_axis * dir_dot_axis;
calc->b = 2.0 * (vec3_dot(ray->direction, oc) - dir_dot_axis * oc_dot_axis);
calc->c = oc_dot_oc - oc_dot_axis * oc_dot_axis - radius * radius;
```

### Alternatives Considered
- **Precompute at parse time**: Cannot precompute because `oc` (ray origin - cylinder center) depends on each ray
- **SIMD vectorization**: Prohibited by 42 School constraints
- **Leave as-is**: Compiler optimization may help but explicit caching ensures optimization and improves readability

### Performance Impact
Estimated 10-15% performance improvement in cylinder intersection tests (one of the most expensive operations in ray tracing).

---

## 2. Repetitive Intersection Loop Pattern

### Decision
Abstract the common intersection checking pattern into a generic iterator function using function pointers.

### Current Code Analysis
Located in `src/render/trace.c`:

**Pattern 1 - Sphere intersections (lines 24-43)**:
```c
int check_sphere_intersections(t_scene *scene, t_ray *ray, t_hit *hit)
{
    t_hit temp_hit;
    int i = 0;
    int hit_found = 0;
    
    while (i < scene->sphere_count)
    {
        temp_hit.distance = hit->distance;
        if (intersect_sphere(ray, &scene->spheres[i], &temp_hit))
        {
            *hit = temp_hit;
            hit_found = 1;
        }
        i++;
    }
    return (hit_found);
}
```

**Pattern 2 - Plane intersections (lines 50-69)**: Nearly identical structure
**Pattern 3 - Cylinder intersections (lines 76-95)**: Nearly identical structure

**Findings**:
- 40 lines of code performing the same logical operation (iterate array, test intersection, update closest hit)
- Only differences: object array pointer, count, and intersection function
- Pattern repeated exactly 3 times with only type changes

### Rationale
Code duplication violates DRY (Don't Repeat Yourself) principle and makes maintenance error-prone. When fixing bugs or adding features, developers must update three separate functions identically. Abstracting this pattern reduces code volume by ~30 lines and centralizes the intersection testing logic.

### Implementation Approach
Use function pointers to create a generic intersection iterator:

```c
typedef int (*t_intersect_fn)(t_ray *ray, void *object, t_hit *hit);

int check_intersections_generic(void *objects, int count, size_t obj_size,
                                 t_intersect_fn intersect_fn, 
                                 t_ray *ray, t_hit *hit)
{
    t_hit temp_hit;
    int i = 0;
    int hit_found = 0;
    void *current_obj;
    
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

// Usage
check_intersections_generic(scene->spheres, scene->sphere_count, 
                            sizeof(t_sphere), 
                            (t_intersect_fn)intersect_sphere, ray, hit);
```

### Alternatives Considered
- **Keep separate functions**: Easier to understand but high maintenance burden and code duplication
- **Macro-based approach**: Would work but function pointers are more type-safe and debuggable
- **Union-based object array**: More complex data structure changes, harder to maintain

### Constraints
Function pointers are allowed under 42 School constraints (standard C feature, no external libraries required).

### Code Reduction
- Current: ~45 lines (3 functions × 15 lines each)
- After: ~20 lines (1 generic function + 3 simple call sites)
- **Savings: 25 lines, 55% reduction in intersection iteration code**

---

## 3. Precomputed Geometry Properties

### Decision
Add cached geometry fields (`radius`, `radius_squared`, `half_height`) to object structures and compute them during scene parsing.

### Current Code Analysis

**Sphere intersection** (`src/ray/intersections.c`, line 54):
```c
radius = sphere->diameter / 2.0;  // Computed every intersection test
```

**Cylinder intersection** (`src/ray/intersect_cylinder.c`, line 40):
```c
radius = cyl->diameter / 2.0;  // Computed every intersection test
```

**Also occurs in**: `src/lighting/shadow_calc.c` (shadow ray tests)

**Findings**:
- `diameter / 2.0` division performed on every ray-sphere intersection (millions per frame)
- `diameter / 2.0` division performed on every ray-cylinder intersection
- `radius * radius` multiplication for sphere intersection (line 33 of intersections.c)
- Division and multiplication are more expensive than memory access

### Rationale
Diameter values are constant after parsing and never change during rendering. Computing radius and squared radius once during scene loading eliminates repeated arithmetic operations in the critical render loop. Memory cost is minimal (16 bytes per object type) while performance gain is significant in functions called millions of times per frame.

### Implementation Approach

**1. Extend object structures** (`includes/objects.h`):
```c
typedef struct s_sphere
{
    t_vec3  center;
    double  diameter;
    t_color color;
    double  radius;          // NEW: precomputed diameter / 2.0
    double  radius_squared;  // NEW: precomputed radius * radius
} t_sphere;

typedef struct s_cylinder
{
    t_vec3  center;
    t_vec3  axis;
    double  diameter;
    double  height;
    t_color color;
    double  radius;          // NEW: precomputed diameter / 2.0
    double  radius_squared;  // NEW: precomputed radius * radius
    double  half_height;     // NEW: precomputed height / 2.0
} t_cylinder;
```

**2. Compute during parsing** (`src/parser/parse_objects.c`):
```c
// After parsing diameter
sphere->radius = sphere->diameter / 2.0;
sphere->radius_squared = sphere->radius * sphere->radius;

// After parsing cylinder diameter and height
cylinder->radius = cylinder->diameter / 2.0;
cylinder->radius_squared = cylinder->radius * cylinder->radius;
cylinder->half_height = cylinder->height / 2.0;
```

**3. Use precomputed values in intersection tests**:
```c
// Before: radius = sphere->diameter / 2.0;
// After:  radius = sphere->radius;

// Before: c = vec3_dot(oc, oc) - radius * radius;
// After:  c = vec3_dot(oc, oc) - sphere->radius_squared;
```

### Alternatives Considered
- **Compute on-the-fly**: Current approach, wastes CPU cycles
- **Compiler optimization**: May inline division but not guaranteed, explicit caching is clearer
- **Lazy evaluation with caching flag**: Over-engineered for static values

### Memory Impact
- Sphere: +16 bytes (2 doubles) × sphere_count
- Cylinder: +24 bytes (3 doubles) × cylinder_count
- Typical scene: ~10 objects → +160 bytes total (negligible)

### Performance Impact
Eliminates 1 division + 1 multiplication per sphere intersection, 1 division per cylinder intersection. Estimated 3-5% performance improvement in intersection tests.

---

## 4. Screen-to-NDC Coordinate Conversion Duplication

### Decision
Extract screen-to-NDC (Normalized Device Coordinates) conversion into a dedicated inline utility function.

### Current Code Analysis
Located in `src/render/render.c`:

**Occurrence 1** (line 48-49, single pixel render):
```c
u = (2.0 * x / (double)WINDOW_WIDTH) - 1.0;
v = 1.0 - (2.0 * y / (double)WINDOW_HEIGHT);
```

**Occurrence 2** (line 74-75, full scene render):
```c
u = (2.0 * x / (double)WINDOW_WIDTH) - 1.0;
v = 1.0 - (2.0 * y / (double)WINDOW_HEIGHT);
```

**Findings**:
- Exact duplicate code in two separate functions
- Same calculation pattern: screen coordinates → NDC range [-1, 1]
- Pattern may appear in future viewport or camera functions

### Rationale
Coordinate conversion is a well-defined mathematical operation that should be encapsulated. Duplication makes it harder to modify viewport calculations consistently. Extracting to a function improves code organization and makes the conversion logic testable independently.

### Implementation Approach

Create inline helper function in appropriate header or source:

```c
/*
** Convert screen pixel coordinates to Normalized Device Coordinates.
** NDC range: u in [-1, 1] (left to right), v in [-1, 1] (bottom to top)
** Screen origin is top-left (0, 0), NDC origin is center (0, 0).
*/
static inline void screen_to_ndc(int x, int y, double *u, double *v)
{
    *u = (2.0 * x / (double)WINDOW_WIDTH) - 1.0;
    *v = 1.0 - (2.0 * y / (double)WINDOW_HEIGHT);
}

// Usage
double u, v;
screen_to_ndc(x, y, &u, &v);
```

### Alternatives Considered
- **Macro-based conversion**: Less type-safe, harder to debug
- **Leave duplicated**: Small amount of code but violates DRY principle
- **Structure with u/v fields**: Over-engineered for simple conversion

### Benefits
- Centralized conversion logic (single source of truth)
- Easier to modify viewport calculations in future
- Self-documenting code (function name explains intent)
- Reduces visual clutter in render functions

### Performance Impact
Using `static inline` ensures no function call overhead (compiler inlines the code). Zero performance impact while improving code organization.

---

## 5. Best Practices for Refactoring C Code

### Decision
Follow systematic refactoring methodology to maintain correctness and compliance with 42 School constraints.

### Refactoring Methodology

**1. Pre-refactoring Validation**
- Run full test suite and document baseline results
- Run valgrind to establish zero-leak baseline
- Capture reference images from test scenes for pixel-perfect comparison
- Run norminette to ensure clean starting state

**2. Incremental Changes**
- Refactor one pattern at a time (not all at once)
- Commit after each working refactoring step
- Test thoroughly between changes

**3. Validation After Each Change**
- Verify identical visual output (pixel-perfect image comparison)
- Verify zero memory leaks with valgrind
- Verify norminette compliance
- Run performance benchmarks to measure improvement

**4. Code Review Checklist**
- All functions from allowed list only (libc, math.h, mlx_*)
- No pthread, fork, pipe, or external parallelization
- Comments updated to reflect changes
- No accidental behavior changes

### Testing Strategy

**Functional Correctness**:
- Render all test scenes in `scenes/` directory
- Compare output images pixel-by-pixel with baseline
- Verify edge cases: zero-diameter objects, negative values, extreme coordinates

**Performance Validation**:
- Measure frames per second on standard test scenes
- Compare before/after metrics using built-in HUD metrics
- Verify 5-15% improvement target is achieved

**Memory Safety**:
- Run valgrind on all test scenes after refactoring
- Verify zero leaks, zero invalid reads/writes
- Check for use-after-free or double-free errors

**Code Quality**:
- Run norminette on all changed files
- Verify 42 header format maintained
- Check function length limits (25 lines) and line length (80 chars)

### Risk Mitigation

**Backup Strategy**:
- Work on feature branch `011-code-refactoring`
- Tag working states before major changes
- Keep original functions available until new code validated

**Rollback Plan**:
- If refactoring introduces bugs: revert to last working commit
- If performance degrades: analyze with profiling tools, adjust approach
- If memory leaks appear: use valgrind to identify source, fix immediately

---

## 6. Code Maintainability Metrics

### Decision
Measure code quality improvements using objective metrics.

### Metrics to Track

**Lines of Code (LOC)**:
- Before: Count total LOC in target files
- After: Count total LOC after refactoring
- Target: 200+ line reduction (from spec success criteria SC-004)

**Code Duplication**:
- Before: Measure duplicate code blocks with tools or manual analysis
- After: Measure after refactoring
- Target: 30% reduction in duplicate patterns (from spec success criteria SC-002)

**Cyclomatic Complexity**:
- Measure function complexity (number of decision points)
- Target: Reduce complexity in intersection checking functions
- Generic iterator should have lower complexity than 3 separate functions

**Function Count**:
- Before: 3 separate intersection checking functions
- After: 1 generic function + 3 simple wrappers
- Improvement: Reduced function complexity, improved maintainability

### Documentation Requirements

Each refactoring must include:
- Code comments explaining why calculation is cached
- Function documentation in Doxygen format
- Rationale for precomputed values in struct comments

Example:
```c
typedef struct s_sphere
{
    t_vec3  center;
    double  diameter;
    t_color color;
    double  radius;          /* Precomputed diameter/2 to avoid division in render loop */
    double  radius_squared;  /* Precomputed radius² for intersection test optimization */
} t_sphere;
```

---

## Summary

### All Clarifications Resolved

This research document resolves all "NEEDS CLARIFICATION" items from the Technical Context section:

1. ✅ **Duplicate calculations identified**: Specific code locations documented
2. ✅ **Repetitive patterns identified**: Three intersection functions analyzed
3. ✅ **Precomputation opportunities identified**: Geometry properties enumerated
4. ✅ **Best practices established**: Refactoring methodology defined
5. ✅ **Testing strategy defined**: Validation approach documented
6. ✅ **42 School compliance verified**: All techniques use allowed functions only

### Ready for Phase 1

With all research complete, the implementation plan can proceed to:
- Phase 1: Create data model showing structure changes
- Phase 1: Generate contracts/interfaces for new generic functions
- Phase 1: Create quickstart guide for developers
- Phase 2: Generate detailed implementation tasks

### Key Constraints Maintained

- ✅ No pthread, fork, or external parallelization libraries
- ✅ All optimizations use allowed techniques (caching, precomputation, abstraction)
- ✅ Pixel-perfect output maintained (no visual changes)
- ✅ Zero memory leaks (valgrind validation required)
- ✅ Norminette compliance (42 coding style)

### Expected Outcomes

- 5-15% rendering performance improvement (measured in FPS)
- 30% reduction in code duplication (measured in LOC)
- 200+ fewer lines of code (success criteria SC-004)
- Improved maintainability and future extensibility
