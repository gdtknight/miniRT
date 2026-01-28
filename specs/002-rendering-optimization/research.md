# Research: Advanced Rendering Performance Optimization

**Feature**: 002-rendering-optimization
**Date**: 2025-12-19
**Purpose**: Resolve technical unknowns and establish implementation patterns

## Research Tasks

### 1. Parallel Ray Tracing Feasibility

**Question**: Can we use pthread for parallel ray tracing in a 42 School project?

**Research Findings**:

**Decision**: pthread is NOT allowed in miniRT

**Rationale**:
- The miniRT subject explicitly enumerates allowed functions: `open`,
  `close`, `read`, `write`, `printf`, `malloc`, `free`, `perror`,
  `strerror`, `exit`, math library, MinilibX, and `gettimeofday()`
- `pthread_create`, `pthread_join`, and related functions are NOT in
  the allowed list
- Using forbidden functions results in a "Forbidden function" evaluation
  flag and automatic zero grade
- This eliminates User Story 1 (Parallel Ray Tracing) entirely

**Impact**:
- US1 is SKIPPED; focus shifts to US2 (BVH) as primary optimization
- US4 (Progressive Tiles) loses threading infrastructure but tile
  structure remains useful for rendering organization
- Single-threaded optimization becomes critical: algorithmic
  improvements (BVH) provide the largest performance gains

**Alternatives Considered**:
- `fork()`: Not in allowed list, also impractical for shared-memory
  ray tracing
- OpenMP: Requires compiler support and is not an allowed function
- Manual coroutines: Too complex, no real concurrency benefit

---

### 2. BVH Construction Algorithm Selection

**Question**: Which BVH construction method best fits the miniRT constraints?

**Research Findings**:

**Decision**: Use Surface Area Heuristic (SAH) for BVH construction

**Rationale**:
- SAH produces high-quality trees that minimize expected traversal cost
- The cost function `C = C_trav + (SA_L/SA_P) * N_L * C_isect +
  (SA_R/SA_P) * N_R * C_isect` optimizes split planes based on
  surface area ratios
- Construction time is acceptable since scenes are static during
  rendering (build once, traverse many times)
- Well-documented algorithm with known implementation patterns

**Implementation Pattern**:
```c
// Evaluate SAH cost for a candidate split
double sah_cost(t_aabb parent, t_aabb left, t_aabb right,
    int n_left, int n_right)
{
    double sa_parent = aabb_surface_area(parent);
    double sa_left = aabb_surface_area(left);
    double sa_right = aabb_surface_area(right);
    return (1.0 + (sa_left / sa_parent) * n_left
        + (sa_right / sa_parent) * n_right);
}
```

**Alternatives Considered**:
- Median split: Simpler but produces lower quality trees; O(n) vs
  O(n log n) traversal in worst case
- Object median: Better than spatial median but still suboptimal
  for non-uniform distributions
- LBVH (Linear BVH): Fast construction but lower traversal quality;
  overkill for static scenes with <1000 objects

---

### 3. AABB Intersection Algorithm

**Question**: Which ray-AABB intersection test is optimal for miniRT?

**Research Findings**:

**Decision**: Use the slab method (Kay-Kajiya algorithm)

**Rationale**:
- Branchless implementation possible (minimizes pipeline stalls)
- Only requires 6 multiplications and 6 comparisons
- Handles edge cases (ray parallel to slab) naturally with IEEE 754
  infinity arithmetic
- Standard algorithm used in production ray tracers

**Implementation Pattern**:
```c
int aabb_intersect(t_aabb box, t_ray ray, double *t_min, double *t_max)
{
    // For each axis: compute entry/exit t values
    // t_min = max(tx_min, ty_min, tz_min)
    // t_max = min(tx_max, ty_max, tz_max)
    // Hit if t_min <= t_max && t_max >= 0
}
```

**Alternatives Considered**:
- Pluecker coordinates: More complex, marginal benefit for AABB
- Separating axis theorem: More general but slower for AABB case

---

### 4. Bounding Box Computation for Primitives

**Question**: How to compute tight AABBs for each supported primitive?

**Research Findings**:

**Sphere**: Trivial - center +/- radius on each axis
```c
aabb.min = center - (radius, radius, radius)
aabb.max = center + (radius, radius, radius)
```

**Cylinder**: Must account for orientation, radius, and half-height
```c
// Project cylinder endpoints onto each axis
// Expand by radius perpendicular to axis
// Take component-wise min/max
```

**Plane**: Infinite extent - use scene-scale bounding box
```c
// Use large value (e.g., 1,000,000) on unbounded axes
// Planes effectively always intersect any reasonable AABB test
```

**Decision**: Implement per-primitive AABB functions in `bounds.c`

---

### 5. Adaptive Quality Rendering

**Question**: How to implement quality switching without threads?

**Research Findings**:

**Decision**: Use interaction state tracking with timer-based upgrade

**Rationale**:
- `gettimeofday()` provides sufficient precision for 1-second delay
- MiniLibX event loop (`mlx_loop_hook`) provides per-frame callback
  for checking timer state
- Binary quality mode (LOW/HIGH) is simplest to implement
- LOW mode renders 2x2 pixel blocks (4x fewer rays)

**Implementation Pattern**:
```c
// In render loop:
if (quality == QUALITY_LOW)
    render_pixel_block(x, y, 2, 2);  // One ray per 2x2 block
else
    render_pixel(x, y);              // One ray per pixel
```

**Key Insight**: Quality switching integrates naturally with the
debounced rendering system (spec 018) - preview mode uses LOW quality,
final render uses HIGH quality.

---

### 6. Progressive Tile Rendering Without Threads

**Question**: How to show rendering progress without parallel execution?

**Research Findings**:

**Decision**: Tile-based rendering with per-frame tile completion

**Rationale**:
- Without threads, progressive rendering means rendering one tile
  per frame (or a few tiles) and displaying immediately
- MiniLibX `mlx_put_image_to_window` can update display after each
  tile without waiting for full frame
- Tile order (spiral or random) improves perceived progress vs
  sequential top-to-bottom

**Limitation**: Without threads, "progressive" means the user sees
tiles appear sequentially rather than concurrently. The primary
benefit is visual feedback, not actual speed improvement.

---

### 7. Performance Metrics Collection

**Question**: How to measure and display rendering performance?

**Research Findings**:

**Decision**: Use `gettimeofday()` for frame timing with rolling average

**Rationale**:
- Microsecond precision is sufficient for frame time measurement
- Rolling average over 60 frames smooths FPS display
- Per-frame overhead is negligible (~500ns for two gettimeofday calls)
- Metrics can be displayed via HUD overlay without affecting render
  performance significantly

**Implementation Pattern**:
```c
typedef struct s_frame_timing {
    long render_time_us;
    long frame_times_us[60];  // Rolling history
    double fps;
}
```

---

## Research Summary

| Question | Decision | Confidence |
|----------|----------|------------|
| pthread allowed? | NO - forbidden | High |
| BVH algorithm | SAH construction | High |
| AABB intersection | Slab method | High |
| Primitive bounds | Per-type AABB functions | High |
| Quality switching | Timer-based binary mode | Medium |
| Progressive tiles | Per-frame tile rendering | Medium |
| Performance metrics | gettimeofday + rolling avg | High |

## Key Risks Identified

1. **No threading**: Single-threaded constraint means BVH is the
   primary optimization lever; expected 10-100x improvement for
   complex scenes
2. **Shadow rays not BVH-accelerated**: Shadow intersection still
   uses brute-force O(n); potential bottleneck with soft shadows
3. **Progressive rendering limited**: Without threads, tiles render
   sequentially; UX benefit is visual feedback only
