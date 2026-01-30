# Quickstart: Complex Scene Performance Optimization

**Feature**: 011-performance-optimization  
**Goal**: Get BVH acceleration and performance metrics running in 30 minutes  
**Prerequisites**: Working miniRT raytracer with basic scene rendering

---

## Quick Navigation

- [5-Minute Demo](#5-minute-demo) - See the improvement immediately
- [Developer Setup](#developer-setup) - Integrate into your code
- [Testing & Validation](#testing--validation) - Verify correctness
- [Common Issues](#common-issues) - Troubleshooting

---

## 5-Minute Demo

Test BVH acceleration on existing code **without modifications**:

```bash
# 1. Ensure you're on the feature branch
git checkout 011-performance-optimization

# 2. Build with optimization feature
make clean && make

# 3. Run complex scene (baseline - should be <1 FPS)
time ./miniRT scenes/valid/valid_scene_complex.rt

# 4. After implementation, run again (should be 3-5 FPS)
./miniRT scenes/valid/valid_scene_complex.rt

# 5. Toggle performance metrics with 'I' key during render
# You should see:
#   FPS: 4.2 (avg: 3.8)
#   Frame: 237ms
#   Rays: 1,228,800
#   Intersections: 3,421 (99.7% reduction)
```

**Expected improvement**: 3-5× faster rendering, 60%+ fewer intersection tests

---

## Developer Setup

### Step 1: Add Headers (5 minutes)

Create `includes/bvh.h`:
```c
#ifndef BVH_H
# define BVH_H

# include "miniRT.h"
# include <stdbool.h>

// AABB structure
typedef struct s_aabb {
    t_vec3  min;
    t_vec3  max;
} t_aabb;

// BVH node structure
typedef struct s_bvh_node {
    t_aabb              bbox;
    struct s_bvh_node   *left;
    struct s_bvh_node   *right;
    t_object            **objects;
    int                 object_count;
    bool                is_leaf;
    bool                is_dirty;
    int                 depth;
} t_bvh_node;

// BVH allocator for memory tracking
typedef struct s_bvh_allocator {
    size_t  total_allocated;
    size_t  max_budget;
    int     node_count;
    bool    limit_exceeded;
} t_bvh_allocator;

// BVH build configuration
typedef struct s_bvh_build_info {
    int     max_depth;
    int     min_objects_leaf;
    int     sah_bins;
    float   traversal_cost;
    float   intersection_cost;
} t_bvh_build_info;

// Core BVH functions
t_bvh_node  *bvh_build(t_object **objects, int count, 
                       t_bvh_allocator *alloc, t_bvh_build_info *info);
bool        bvh_intersect(t_bvh_node *root, t_ray *ray, 
                         t_hit_record *hit, t_perf_metrics *metrics);
void        bvh_destroy(t_bvh_node *root);

// AABB operations
t_aabb      aabb_from_object(t_object *obj);
bool        aabb_intersect_ray(t_aabb box, t_ray *ray, float t_min, float t_max);
t_aabb      aabb_union(t_aabb a, t_aabb b);
float       aabb_surface_area(t_aabb box);

#endif
```

Create `includes/optimization.h`:
```c
#ifndef OPTIMIZATION_H
# define OPTIMIZATION_H

# include "miniRT.h"
# include <stdint.h>
# include <stdbool.h>

// Performance metrics structure
typedef struct s_perf_metrics {
    uint64_t    frame_times[60];
    uint64_t    bvh_traverse_times[60];
    int         buffer_index;
    uint64_t    frame_start_time;
    uint64_t    bvh_traverse_start;
    int         ray_count;
    int         intersection_count;
    int         bvh_node_visits;
    int         frame_count;
    bool        display_enabled;
} t_perf_metrics;

// Metrics functions
t_perf_metrics  metrics_init(void);
void            metrics_frame_start(t_perf_metrics *m);
void            metrics_frame_end(t_perf_metrics *m);
void            metrics_bvh_start(t_perf_metrics *m);
void            metrics_bvh_end(t_perf_metrics *m);
void            metrics_add_ray(t_perf_metrics *m);
void            metrics_add_intersection(t_perf_metrics *m, int count);
void            metrics_toggle_display(t_perf_metrics *m);
void            metrics_render_hud(t_perf_metrics *m, void *img, void *mlx);

// Computed metrics
float           metrics_get_fps_current(t_perf_metrics *m);
float           metrics_get_fps_average(t_perf_metrics *m);
float           metrics_get_frame_ms_current(t_perf_metrics *m);

#endif
```

### Step 2: Update Scene Structure (2 minutes)

Modify `includes/miniRT.h` to add BVH and metrics:
```c
typedef struct s_scene {
    // ... existing fields ...
    t_bvh_node      *bvh_root;      // NEW: BVH acceleration structure
    t_perf_metrics  metrics;        // NEW: Performance tracking
    bool            use_bvh;        // NEW: BVH enabled flag
} t_scene;
```

### Step 3: Integrate into Scene Loading (10 minutes)

Modify `src/scene/scene_load.c` (or equivalent):
```c
void scene_finalize_loading(t_scene *scene)
{
    // ... existing scene loading code ...
    
    // Initialize BVH
    t_bvh_allocator allocator = {
        .total_allocated = 0,
        .max_budget = 512 * 1024 * 1024,  // 512 MB
        .node_count = 0,
        .limit_exceeded = false
    };
    
    t_bvh_build_info build_info = {
        .max_depth = 64,
        .min_objects_leaf = 4,
        .sah_bins = 16,
        .traversal_cost = 1.0,
        .intersection_cost = 10.0
    };
    
    scene->bvh_root = bvh_build(
        scene->objects,
        scene->object_count,
        &allocator,
        &build_info
    );
    
    if (!scene->bvh_root)
    {
        ft_putstr_fd("Warning: BVH construction failed, using O(n) fallback\n", 2);
        scene->use_bvh = false;
    }
    else
    {
        scene->use_bvh = true;
        printf("BVH built: %d nodes, %.2f MB\n",
               allocator.node_count,
               allocator.total_allocated / (1024.0 * 1024.0));
    }
    
    // Initialize metrics
    scene->metrics = metrics_init();
}
```

### Step 4: Modify Ray Tracing Loop (10 minutes)

Replace existing ray-object intersection loop with BVH traversal:

**Before** (brute-force):
```c
bool find_closest_hit(t_scene *scene, t_ray *ray, t_hit_record *hit)
{
    hit->t = INFINITY;
    bool hit_anything = false;
    
    for (int i = 0; i < scene->object_count; i++)
    {
        if (intersect_object(scene->objects[i], ray, hit))
            hit_anything = true;
    }
    
    return hit_anything;
}
```

**After** (with BVH):
```c
bool find_closest_hit(t_scene *scene, t_ray *ray, t_hit_record *hit)
{
    metrics_add_ray(&scene->metrics);
    
    if (scene->use_bvh)
    {
        metrics_bvh_start(&scene->metrics);
        bool result = bvh_intersect(scene->bvh_root, ray, hit, &scene->metrics);
        metrics_bvh_end(&scene->metrics);
        return result;
    }
    else
    {
        // Fallback to O(n) brute-force
        hit->t = INFINITY;
        bool hit_anything = false;
        
        for (int i = 0; i < scene->object_count; i++)
        {
            metrics_add_intersection(&scene->metrics, 1);
            if (intersect_object(scene->objects[i], ray, hit))
                hit_anything = true;
        }
        
        return hit_anything;
    }
}
```

### Step 5: Add Metrics HUD (5 minutes)

Integrate into main render loop:
```c
void render_frame(t_scene *scene)
{
    metrics_frame_start(&scene->metrics);
    
    // ... existing pixel rendering loop ...
    for (int y = 0; y < scene->height; y++)
    {
        for (int x = 0; x < scene->width; x++)
        {
            t_ray ray = create_camera_ray(&scene->camera, x, y);
            t_hit_record hit;
            
            if (find_closest_hit(scene, &ray, &hit))
            {
                t_color color = compute_lighting(scene, &hit);
                put_pixel(scene->image, x, y, color);
            }
        }
    }
    
    metrics_frame_end(&scene->metrics);
    
    // Render HUD overlay
    if (metrics_is_display_enabled(&scene->metrics))
        metrics_render_hud(&scene->metrics, scene->image, scene->mlx);
    
    mlx_put_image_to_window(scene->mlx, scene->win, scene->image, 0, 0);
}
```

### Step 6: Add Key Handler (3 minutes)

Enable 'I' key to toggle metrics:
```c
int key_hook(int keycode, t_scene *scene)
{
    if (keycode == KEY_I)  // 'I' key (typically 34 on Linux, 34 on macOS)
    {
        metrics_toggle_display(&scene->metrics);
        render_frame(scene);  // Redraw immediately
        return 0;
    }
    
    // ... other key handlers (W/A/S/D/Q/E for camera movement) ...
}
```

### Step 7: Cleanup on Exit (2 minutes)

Free BVH memory:
```c
void cleanup_scene(t_scene *scene)
{
    // ... existing cleanup ...
    
    if (scene->bvh_root)
        bvh_destroy(scene->bvh_root);
    
    metrics_destroy(&scene->metrics);
}
```

---

## Testing & Validation

### Functional Tests

#### 1. Visual Correctness Test
```bash
# Generate reference image without BVH (brute-force)
./miniRT scenes/valid/valid_scene_complex.rt --no-bvh --output reference.ppm

# Generate image with BVH
./miniRT scenes/valid/valid_scene_complex.rt --output optimized.ppm

# Compare pixel-by-pixel (should be identical)
diff reference.ppm optimized.ppm
# Expected: No differences
```

#### 2. Performance Test
```bash
# Run with metrics enabled
./miniRT scenes/valid/valid_scene_complex.rt

# Press 'I' to show metrics
# Verify:
#   - FPS: 3-5 (target range)
#   - Intersections: <1000 (was ~15000 without BVH)
#   - BVH traverse time: 30-40% of frame time
```

#### 3. Memory Test
```bash
# Check for memory leaks with valgrind
valgrind --leak-check=full --show-leak-kinds=all ./miniRT scenes/valid/valid_scene_complex.rt

# Expected: 0 bytes leaked
```

#### 4. Stress Test
```bash
# Test with large scene (50+ objects)
./miniRT scenes/benchmark_bvh.rt

# Verify:
#   - BVH construction completes <5 seconds
#   - Memory usage <512 MB
#   - No crashes or errors
```

### Unit Tests (Optional but Recommended)

Create `tests/test_bvh.c`:
```c
void test_aabb_construction(void)
{
    t_sphere sphere = {.center = {0, 0, 0}, .radius = 1.0};
    t_aabb box = aabb_from_sphere(&sphere);
    
    assert(box.min.x == -1.0 && box.max.x == 1.0);
    assert(box.min.y == -1.0 && box.max.y == 1.0);
    assert(box.min.z == -1.0 && box.max.z == 1.0);
}

void test_aabb_intersection(void)
{
    t_aabb box = {{-1, -1, -1}, {1, 1, 1}};
    t_ray ray = {.origin = {-2, 0, 0}, .direction = {1, 0, 0}};
    
    assert(aabb_intersect_ray(box, &ray, 0.0, 10.0) == true);
    
    ray.origin = (t_vec3){0, 2, 0};
    ray.direction = (t_vec3){1, 0, 0};
    assert(aabb_intersect_ray(box, &ray, 0.0, 10.0) == false);
}

int main(void)
{
    test_aabb_construction();
    test_aabb_intersection();
    // ... more tests ...
    
    printf("All tests passed!\n");
    return 0;
}
```

---

## Common Issues

### Issue 1: BVH Construction Fails
**Symptom**: "BVH construction failed, using O(n) fallback" message

**Cause**: Memory limit exceeded or allocation failure

**Solution**:
```c
// Check allocator status
if (allocator.limit_exceeded)
    printf("Memory limit exceeded: %zu MB used\n", 
           allocator.total_allocated / (1024 * 1024));

// Reduce BVH depth or increase leaf size
build_info.max_depth = 32;  // Was 64
build_info.min_objects_leaf = 8;  // Was 4
```

---

### Issue 2: Visual Artifacts (Black Pixels, Wrong Colors)
**Symptom**: Rendered image differs from reference

**Cause**: BVH traversal bug or incorrect AABB construction

**Solution**:
```bash
# Enable debug output in BVH traversal
DEBUG_BVH=1 ./miniRT scenes/valid/valid_scene_complex.rt

# Check AABB bounds visually
./miniRT scenes/valid/valid_scene_complex.rt --debug-aabb
```

**Common bugs**:
- AABB doesn't fully contain object (too tight)
- Ray-AABB intersection test has sign error
- BVH tree structure inconsistent (internal node has objects)

---

### Issue 3: Slower Than Brute-Force
**Symptom**: BVH rendering is slower than O(n) approach

**Cause**: Degenerate BVH tree (unbalanced) or excessive traversal cost

**Solution**:
```c
// Print BVH statistics
void print_bvh_stats(t_bvh_node *root)
{
    int max_depth = compute_max_depth(root);
    int leaf_count = count_leaves(root);
    float avg_objects_per_leaf = compute_avg_objects(root);
    
    printf("BVH Stats:\n");
    printf("  Max depth: %d (optimal: log2(n) = %.1f)\n", 
           max_depth, log2(scene->object_count));
    printf("  Leaf count: %d\n", leaf_count);
    printf("  Avg objects per leaf: %.2f\n", avg_objects_per_leaf);
}
```

**Expected values**:
- Max depth ≈ 2 × log₂(n) (e.g., depth 8-10 for 15 objects)
- Avg objects per leaf: 2-4
- If max depth > 30: tree is degenerate, check SAH implementation

---

### Issue 4: Metrics Not Displaying
**Symptom**: Press 'I' key but HUD doesn't appear

**Solution**:
```c
// Verify key code
printf("Key pressed: %d\n", keycode);
// Linux: 'I' is typically 34
// macOS: 'I' is typically 34

// Check metrics display flag
if (metrics_is_display_enabled(&scene->metrics))
    printf("Metrics display is ON\n");
else
    printf("Metrics display is OFF\n");

// Verify mlx_string_put is working
mlx_string_put(mlx, win, 10, 10, 0xFFFFFF, "Test");
```

---

### Issue 5: Segmentation Fault During Traversal
**Symptom**: Crash in `bvh_intersect`

**Cause**: Corrupted BVH tree structure or stack overflow

**Solution**:
```c
// Add validation during build
void validate_bvh_node(t_bvh_node *node)
{
    if (!node)
        return;
    
    assert(node->bbox.min.x <= node->bbox.max.x);
    assert(node->bbox.min.y <= node->bbox.max.y);
    assert(node->bbox.min.z <= node->bbox.max.z);
    
    if (node->is_leaf)
    {
        assert(node->objects != NULL);
        assert(node->object_count > 0);
        assert(node->left == NULL && node->right == NULL);
    }
    else
    {
        assert(node->left != NULL && node->right != NULL);
        assert(node->objects == NULL);
        assert(node->object_count == 0);
        validate_bvh_node(node->left);
        validate_bvh_node(node->right);
    }
}

// Call after build
validate_bvh_node(scene->bvh_root);
```

---

## Performance Tuning

### Optimize SAH Bin Count
```c
// Fewer bins = faster build, worse quality
build_info.sah_bins = 8;   // Fast build, 10% slower traversal

// More bins = slower build, better quality
build_info.sah_bins = 32;  // Slow build, 5% faster traversal

// Default (good balance)
build_info.sah_bins = 16;
```

### Optimize Leaf Size
```c
// Smaller leaves = deeper tree, more nodes
build_info.min_objects_leaf = 1;  // Max depth, slowest build

// Larger leaves = shallower tree, fewer nodes
build_info.min_objects_leaf = 8;  // Faster build, more leaf tests

// Default (good balance)
build_info.min_objects_leaf = 4;
```

### Optimize Traversal Order
```c
// In bvh_traversal.c, visit near child first
float dist_left = ray_distance_to_aabb(ray, node->left->bbox);
float dist_right = ray_distance_to_aabb(ray, node->right->bbox);

if (dist_left < dist_right)
{
    visit(node->left);
    visit(node->right);
}
else
{
    visit(node->right);
    visit(node->left);
}
```

---

## Success Checklist

- ✅ BVH builds successfully (no fallback message)
- ✅ FPS improves from <1 to 3-5 on test_complex_scene.rt
- ✅ Rendered images identical to reference (pixel-perfect)
- ✅ Memory usage <512 MB (check with valgrind --tool=massif)
- ✅ No memory leaks (valgrind reports 0 bytes leaked)
- ✅ Metrics HUD displays correctly (press 'I')
- ✅ Intersection tests reduced by 60%+ (shown in HUD)

---

## Next Steps

After basic integration:
1. **Add cylinder optimizations** (see `contracts/bvh_api.md`)
2. **Implement selective BVH rebuild** for dynamic scenes
3. **Profile with gprof** to identify remaining hotspots
4. **Add more test scenes** to validate various object counts

---

## Resources

- **Full API reference**: See `contracts/bvh_api.md` and `contracts/metrics_api.md`
- **Data models**: See `data-model.md` for structure details
- **Research findings**: See `research.md` for algorithm justifications
- **42 School constraints**: All implementations comply with allowed functions only

---

**Estimated total integration time**: 30-40 minutes for basic setup, 2-3 hours for full feature with optimizations
