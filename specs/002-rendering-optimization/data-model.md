# Data Model: Advanced Rendering Performance Optimization

**Feature**: 002-rendering-optimization
**Date**: 2025-12-19
**Status**: Implemented

## Entity Overview

The rendering optimization feature introduces four primary entity groups:

1. **BVH Spatial Acceleration** - Hierarchical spatial structure for
   O(log n) ray intersection
2. **Performance Metrics** - Real-time rendering statistics collection
3. **Render Quality State** - Adaptive quality mode management
4. **Progressive Rendering** - Tile-based incremental rendering

## Entity Definitions

### 1. AABB (`t_aabb`)

**Purpose**: Axis-Aligned Bounding Box for spatial bounds testing.

**Fields**:
```c
typedef struct s_aabb
{
    t_vec3  min;    // Minimum corner (x_min, y_min, z_min)
    t_vec3  max;    // Maximum corner (x_max, y_max, z_max)
}   t_aabb;
```

**Validation Rules**:
- `min.x <= max.x`, `min.y <= max.y`, `min.z <= max.z`
- For planes: min/max use large sentinel values (~1,000,000)

**Operations**:
- `aabb_intersect()`: Ray-AABB intersection test (slab method)
- `aabb_surface_area()`: Surface area for SAH cost evaluation
- `aabb_merge()`: Union of two AABBs (parent bounds)
- `aabb_for_object()`: Compute tight AABB for sphere/plane/cylinder

---

### 2. BVH Node (`t_bvh_node`)

**Purpose**: Binary tree node for hierarchical spatial partitioning.

**Fields**:
```c
typedef struct s_bvh_node
{
    t_aabb          bounds;         // Bounding box of this subtree
    t_bvh_node      *left;          // Left child (NULL for leaf)
    t_bvh_node      *right;         // Right child (NULL for leaf)
    t_object_ref    *objects;       // Object list (leaf nodes only)
    int             object_count;   // Number of objects in leaf
    int             depth;          // Tree depth level
}   t_bvh_node;
```

**Invariants**:
- Internal nodes: `left != NULL && right != NULL && objects == NULL`
- Leaf nodes: `left == NULL && right == NULL && objects != NULL`
- `bounds` encloses all objects in subtree
- `depth` increments by 1 per level (root = 0)

**Memory Management**:
- Nodes allocated recursively during `bvh_build_recursive()`
- Freed recursively via `bvh_lifecycle.c` cleanup
- Object reference arrays allocated per leaf node

---

### 3. BVH Root (`t_bvh`)

**Purpose**: Top-level BVH container with metadata.

**Fields**:
```c
typedef struct s_bvh
{
    t_bvh_node  *root;          // Root node of BVH tree
    int         enabled;        // 1 if BVH active, 0 for brute-force
    int         total_nodes;    // Total node count
    int         max_depth;      // Maximum tree depth
    int         visualize;      // Debug visualization flag
}   t_bvh;
```

**State Transitions**:
- `NULL` -> built: `scene_build_bvh()` called after parsing
- enabled/disabled: Toggle with keyboard ('b' key)
- Rebuild: On scene geometry changes

---

### 4. Frame Timing (`t_frame_timing`)

**Purpose**: Track per-frame rendering performance.

**Fields**:
```c
typedef struct s_frame_timing
{
    long    render_time_us;                     // Current frame (us)
    long    frame_times_us[FRAME_HISTORY_SIZE]; // Rolling history
    double  fps;                                // Computed FPS
}   t_frame_timing;
```

**Behavior**:
- `FRAME_HISTORY_SIZE` = 60 (1 second at 60 FPS)
- FPS computed as rolling average over history buffer
- Updated once per frame via `metrics_end_frame()`

---

### 5. Ray Metrics (`t_ray_metrics`)

**Purpose**: Track ray casting statistics per frame.

**Fields**:
```c
typedef struct s_ray_metrics
{
    long    rays_traced;        // Total primary + shadow rays
    long    intersect_tests;    // Object intersection tests
}   t_ray_metrics;
```

**Reset**: Cleared at start of each frame

---

### 6. BVH Metrics (`t_bvh_metrics`)

**Purpose**: Track BVH traversal efficiency per frame.

**Fields**:
```c
typedef struct s_bvh_metrics
{
    long    nodes_visited;      // BVH nodes traversed
    long    tests_skipped;      // Intersection tests avoided
    long    box_tests;          // AABB intersection tests
}   t_bvh_metrics;
```

**Derived Metric**: BVH efficiency = `tests_skipped / (tests_skipped
+ intersect_tests)` (percentage of work avoided)

---

### 7. Metrics Container (`t_metrics`)

**Purpose**: Aggregate all performance metrics.

**Fields**:
```c
typedef struct s_metrics
{
    t_frame_timing  timing;
    t_ray_metrics   ray;
    t_bvh_metrics   bvh;
    int             quality_mode;   // Current quality level
}   t_metrics;
```

**Integration**: Embedded in `t_scene` structure

---

### 8. Quality Mode (`t_quality_mode`)

**Purpose**: Enumerate rendering quality levels.

**Definition**:
```c
typedef enum e_quality_mode
{
    QUALITY_LOW,    // 2x2 pixel blocks, simplified shadows
    QUALITY_HIGH    // Per-pixel rendering, full quality
}   t_quality_mode;
```

---

### 9. Interaction State (`t_interaction_state`)

**Purpose**: Track user interaction for adaptive quality switching.

**Fields**:
```c
typedef struct s_interaction_state
{
    int             is_interacting;     // Currently interacting flag
    struct timeval  last_interaction;   // Timestamp of last input
    int             interaction_count;  // Input event counter
}   t_interaction_state;
```

**Behavior**:
- `is_interacting` set to 1 on keyboard input
- `last_interaction` updated via `gettimeofday()`
- After 1 second of no interaction, quality upgrades to HIGH

---

### 10. Progressive State (`t_progressive_state`)

**Purpose**: Track tile-based progressive rendering progress.

**Fields**:
```c
typedef struct s_progressive_state
{
    int     current_tile;       // Index of next tile to render
    int     total_tiles;        // Total tile count for window
    int     tiles_completed;    // Tiles finished in current frame
    int     tile_size;          // Pixels per tile side (default 32)
    int     enabled;            // Progressive mode active flag
}   t_progressive_state;
```

**Tile Calculation**:
- `total_tiles = ceil(width/tile_size) * ceil(height/tile_size)`
- Default: `(800/32) * (600/32) = 25 * 19 = 475 tiles`

---

### 11. Render State (`t_render_state`)

**Purpose**: Aggregate render state for quality and progressive control.

**Fields**:
```c
typedef struct s_render_state
{
    t_quality_mode      quality;        // Current quality
    t_quality_mode      target_quality; // Target after upgrade
    t_interaction_state interaction;    // User interaction tracking
    t_progressive_state progressive;   // Tile progress tracking
    int                 adaptive_enabled; // Adaptive mode flag
    int                 show_info;      // Display info overlay
}   t_render_state;
```

---

## Entity Relationships

```
t_scene
├── t_bvh
│   └── t_bvh_node (tree)
│       ├── t_aabb (bounds)
│       └── t_object_ref[] (leaf objects)
├── t_metrics
│   ├── t_frame_timing
│   ├── t_ray_metrics
│   └── t_bvh_metrics
└── (render_state accessed via t_render)

t_render
└── t_render_state (via render_state.h)
    ├── t_quality_mode
    ├── t_interaction_state
    └── t_progressive_state
```

## Memory Lifecycle

| Entity | Allocation | Deallocation |
|--------|-----------|--------------|
| `t_bvh` | `scene_build_bvh()` after parse | `bvh_free()` in cleanup |
| `t_bvh_node` | Recursive during build | Recursive free |
| `t_object_ref[]` | Per leaf node | Freed with leaf node |
| `t_metrics` | Stack (embedded in scene) | No heap allocation |
| `t_render_state` | Stack (embedded in render) | No heap allocation |
