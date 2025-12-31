# Data Model: BVH Performance Verification and Debugging Tools

**Feature Branch**: `008-bvh-performance-diagnostics`  
**Created**: 2025-12-31  
**Status**: Complete

## Overview

This document defines all data structures, types, and relationships for BVH performance metrics collection and display. Entities are designed to integrate with existing miniRT structures (t_metrics, t_render_state, t_hud_state) while maintaining norminette compliance.

---

## Core Entities

### 1. Frame Metrics (t_metrics)

**Purpose**: Real-time performance data collected during each render frame.

**Structure** (extends existing metrics.h):
```c
typedef struct s_metrics
{
    /* Frame timing */
    struct timeval  start_time;          // Frame start timestamp
    struct timeval  end_time;            // Frame end timestamp
    long            render_time_us;      // Current frame render time (microseconds)
    
    /* FPS calculation */
    long            frame_times_us[60];  // Rolling buffer for FPS smoothing
    int             frame_index;         // Circular buffer position (0-59)
    long            frame_count;         // Total frames rendered (for statistics)
    double          fps;                 // Calculated frames per second
    
    /* Ray tracing counters */
    long            rays_traced;         // Total rays cast this frame
    long            intersect_tests;     // Ray-object intersection tests performed
    
    /* BVH traversal counters */
    long            bvh_nodes_visited;   // BVH nodes traversed
    long            bvh_tests_skipped;   // BVH branches pruned (spatial culling)
    long            bvh_box_tests;       // Ray-bounding box tests
    
    /* Render quality state */
    int             quality_mode;        // 0=low, 1=high (for adaptive rendering)
}   t_metrics;
```

**Fields**:
- **start_time**: Frame timing anchor, set by `metrics_start_frame()`
- **render_time_us**: Frame duration in microseconds, calculated by `metrics_end_frame()`
- **frame_times_us[]**: Circular buffer storing last 60 frame times for FPS averaging
- **frame_index**: Current write position in circular buffer (wraps at 60)
- **frame_count**: Monotonic counter, never resets (for long-term statistics)
- **fps**: Smoothed frames-per-second calculated from rolling average
- **rays_traced**: Incremented once per ray cast (typically width × height)
- **intersect_tests**: Incremented for every ray-object intersection test
- **bvh_nodes_visited**: Incremented when BVH node is traversed
- **bvh_tests_skipped**: Incremented when BVH subtree is culled
- **bvh_box_tests**: Incremented for every ray-AABB intersection test

**Lifecycle**:
1. **Initialization**: `metrics_init()` - zeros all fields, sets frame_index=0
2. **Frame Start**: `metrics_start_frame()` - resets counters, starts timer
3. **Rendering**: Counter increments via inline functions
4. **Frame End**: `metrics_end_frame()` - stops timer, calculates FPS
5. **Display**: HUD reads metrics during `hud_render_performance()`

**Validation Rules**:
- All counters must be non-negative
- frame_index must be in range [0, 59]
- render_time_us should be > 0 after metrics_end_frame()
- rays_traced should equal width × height for full-frame renders

**Relationships**:
- Embedded in `t_render_state` structure
- Accessed by render loop, BVH traversal, and HUD display
- Reset per-frame except frame_count and fps

---

### 2. BVH Statistics (t_bvh)

**Purpose**: Tree structure information collected during BVH construction.

**Structure** (extends existing spatial.h):
```c
typedef struct s_bvh
{
    t_bvh_node  *root;                  // Tree root node
    int         enabled;                // BVH activation flag (toggled by 'B' key)
    
    /* Tree structure metrics */
    int         total_nodes;            // Count of all nodes (internal + leaf)
    int         max_depth;              // Maximum tree depth (root = 0)
    int         leaf_count;             // Number of leaf nodes
    long        leaf_object_sum;        // Sum of objects in all leaves (for average)
    double      avg_leaf_objects;       // Average objects per leaf node
    
    /* Build performance */
    long        build_time_us;          // Tree construction time (microseconds)
    int         object_count;           // Total objects in scene
}   t_bvh;
```

**Fields**:
- **root**: Pointer to root BVH node (existing)
- **enabled**: Flag for BVH on/off state (existing, toggled by 'B' key)
- **total_nodes**: Incremented during `bvh_build_recursive()` for each node created
- **max_depth**: Updated during recursion when depth > current max
- **leaf_count**: Incremented when leaf node created
- **leaf_object_sum**: Accumulated object counts from leaf nodes
- **avg_leaf_objects**: Calculated as `leaf_object_sum / leaf_count` after build
- **build_time_us**: Measured by timer around `bvh_build_recursive()` call
- **object_count**: Stored during build for efficiency calculations

**Lifecycle**:
1. **Creation**: `bvh_create()` - allocates structure, zeros fields
2. **Build**: `bvh_build()` - constructs tree, collects statistics
3. **Output**: Terminal prints statistics immediately after build
4. **Runtime**: Statistics remain constant until scene changes
5. **Cleanup**: `bvh_destroy()` - frees tree and statistics structure

**Validation Rules**:
- total_nodes should be > 0 after build (at least root node)
- max_depth should be roughly log2(object_count) for balanced tree
- avg_leaf_objects should be in range [1, MAX_LEAF_OBJECTS] (typically 1-8)
- build_time_us should correlate with object_count (linear to O(n log n))

**Relationships**:
- Owned by `t_render_state` structure
- Read by HUD when show_info flag is enabled
- Independent of per-frame metrics (built once per scene)

---

### 3. Performance Display State

**Purpose**: Manage visibility and layout of performance section in HUD.

**Structure** (extends existing hud.h):
```c
/* No new structure needed - reuses existing t_hud_state and t_render_state fields */

// Existing fields used for performance display:
// t_render_state.show_info     -> Controls detailed BVH stats visibility ('I' key)
// t_render_state.bvh_enabled   -> BVH on/off state for display ('B' key)
// t_hud_state.visible          -> Master HUD visibility ('H' key)
```

**Design Decision**: No dedicated performance state structure needed. Existing state flags provide necessary control:
- **show_info**: Toggles detailed BVH statistics (ray counts, node visits, efficiency)
- **bvh_enabled**: Determines which metrics to display (BVH stats only shown when enabled)
- **visible**: Master switch for entire HUD including performance section

**Display Modes**:
1. **HUD Hidden** (visible=0): No performance display
2. **Basic Mode** (visible=1, show_info=0): FPS, frame time, BVH status only
3. **Detailed Mode** (visible=1, show_info=1): All metrics including BVH statistics

---

## Derived Metrics

### 1. Frames Per Second (FPS)

**Calculation**:
```c
double calculate_fps(t_metrics *m)
{
    long    sum;
    int     i;
    
    sum = 0;
    i = 0;
    while (i < 60)
    {
        sum += m->frame_times_us[i];
        i++;
    }
    return (1000000.0 / (sum / 60.0));
}
```

**Formula**: `FPS = 1,000,000 / avg(frame_times_us)`

**Properties**:
- Smoothed over 60 frames (~1 second at 60 FPS)
- Updates every frame with new rolling average
- Displayed with 1 decimal place precision (e.g., "58.3")

**Edge Cases**:
- First 60 frames: Use partial average until buffer fills
- Zero frame time: Guard against division by zero (return 0.0)

---

### 2. BVH Efficiency Percentage

**Calculation**:
```c
double calculate_bvh_efficiency(t_metrics *m, int object_count)
{
    long    naive_tests;
    double  efficiency;
    
    if (object_count == 0 || m->rays_traced == 0)
        return (0.0);
    
    naive_tests = m->rays_traced * object_count;
    efficiency = 1.0 - ((double)m->intersect_tests / naive_tests);
    
    return (efficiency * 100.0);  // Convert to percentage
}
```

**Formula**: `Efficiency = (1 - actual_tests / naive_tests) × 100%`

**Properties**:
- Measures spatial culling effectiveness
- Higher is better (90% = 10x reduction in tests)
- Displayed with 1 decimal place (e.g., "90.0%")

**Edge Cases**:
- No objects: Return 0.0%
- No rays traced: Return 0.0%
- actual_tests > naive_tests: Cap at 0.0% (shouldn't happen in practice)

---

### 3. Average Tests Per Ray

**Calculation**:
```c
double calculate_avg_tests_per_ray(t_metrics *m)
{
    if (m->rays_traced == 0)
        return (0.0);
    
    return ((double)m->intersect_tests / m->rays_traced);
}
```

**Formula**: `Avg Tests/Ray = intersect_tests / rays_traced`

**Properties**:
- Indicates how many objects each ray tested on average
- Lower is better (BVH should reduce this significantly)
- Displayed with 1 decimal place (e.g., "10.0")

**Comparison**:
- Without BVH: Should equal object_count (every ray tests every object)
- With BVH: Should be << object_count (spatial culling in effect)

---

## Data Flow Diagram

```
Scene Load
    │
    ├─> bvh_build()
    │       │
    │       ├─> Collect tree statistics
    │       ├─> Calculate avg_leaf_objects
    │       ├─> Measure build_time_us
    │       └─> printf() statistics to terminal
    │
    └─> metrics_init()
            │
            └─> Zero all counters, prepare for first frame

Frame Start
    │
    └─> metrics_start_frame()
            │
            ├─> Reset per-frame counters (rays, tests, nodes)
            └─> timer_start(&start_time)

Rendering Loop (for each ray)
    │
    ├─> metrics_add_ray()                  [Increment rays_traced]
    │
    └─> BVH Traversal (if enabled)
            │
            ├─> metrics_add_bvh_node_visit()    [Increment nodes_visited]
            ├─> metrics_add_bvh_box_test()      [Increment box_tests]
            ├─> metrics_add_bvh_skip()          [Increment tests_skipped]
            └─> metrics_add_intersect_test()    [Increment intersect_tests]

Frame End
    │
    └─> metrics_end_frame()
            │
            ├─> Calculate render_time_us
            ├─> Update frame_times_us[] buffer
            ├─> Calculate fps rolling average
            └─> Increment frame_count

HUD Render
    │
    └─> hud_render_performance()
            │
            ├─> Read metrics.fps, render_time_us
            ├─> Display FPS and frame time
            ├─> Display BVH status (enabled/disabled)
            │
            └─> If show_info enabled:
                    │
                    ├─> Display rays_traced, intersect_tests
                    ├─> Display bvh_nodes_visited, tests_skipped
                    ├─> Calculate and display avg_tests_per_ray
                    ├─> Calculate and display bvh_efficiency
                    └─> Display BVH tree stats (nodes, depth, avg_leaf_objects)
```

---

## Counter Increment Points

### Ray Tracing (trace.c)
```c
void trace_ray(t_ray ray, t_scene *scene, t_metrics *metrics, ...)
{
    metrics_add_ray(metrics);  // Count this ray
    
    // BVH or naive intersection
    if (scene->bvh && scene->bvh->enabled)
        hit = bvh_intersect(scene->bvh, ray, &record, scene);
    else
        hit = intersect_scene_naive(ray, scene, &record, metrics);
    
    // ... continue with shading
}
```

### BVH Traversal (bvh_traverse.c)
```c
int bvh_node_intersect(t_bvh_node *node, t_ray ray, t_hit_record *hit,
                       t_scene *scene, t_metrics *metrics)
{
    metrics_add_bvh_node_visit(metrics);  // Visited this node
    
    if (!aabb_intersect(node->bounds, ray, &t_min, &t_max))
    {
        metrics_add_bvh_skip(metrics);    // Culled this subtree
        return (0);
    }
    
    metrics_add_bvh_box_test(metrics);    // Performed AABB test
    
    // ... continue traversal or test objects
}
```

### Object Intersection (objects/*.c)
```c
int intersect_sphere(t_ray ray, t_sphere sphere, t_hit_record *hit,
                     t_metrics *metrics)
{
    metrics_add_intersect_test(metrics);  // Count this test
    
    // ... sphere intersection math
}
```

---

## State Transitions

### BVH Toggle ('B' Key)
```
Current State: BVH ON
User presses 'B'
    │
    ├─> render_state.bvh_enabled = 0
    ├─> HUD displays "BVH: OFF"
    ├─> Next frame uses naive intersection
    └─> Metrics show different performance (slower, more tests)

User presses 'B' again
    │
    ├─> render_state.bvh_enabled = 1
    ├─> HUD displays "BVH: ON"
    ├─> Next frame uses BVH intersection
    └─> Metrics show improved performance (faster, fewer tests)
```

### Detail Toggle ('I' Key)
```
Current State: Basic Performance Display
User presses 'I'
    │
    ├─> render_state.show_info = 1
    ├─> HUD shows additional metrics:
    │       ├─> Ray-Box tests
    │       ├─> Ray-Object tests
    │       ├─> Nodes visited/skipped
    │       ├─> Avg tests per ray
    │       ├─> Efficiency percentage
    │       └─> BVH tree statistics
    └─> Next frame displays full diagnostic info

User presses 'I' again
    │
    ├─> render_state.show_info = 0
    ├─> HUD shows only basic metrics:
    │       ├─> FPS
    │       ├─> Frame time
    │       └─> BVH status
    └─> Next frame displays minimal info
```

---

## Memory Layout Considerations

### Cache-Friendly Structure Organization

**Hot Path Fields** (accessed every frame):
```c
struct s_metrics {
    // Group frequently-incremented counters together (cache line 1)
    long rays_traced;
    long intersect_tests;
    long bvh_nodes_visited;
    long bvh_tests_skipped;
    
    // Timing fields (cache line 2)
    struct timeval start_time;
    long render_time_us;
    
    // Less frequently accessed (cache line 3+)
    long frame_times_us[60];
    double fps;
    long frame_count;
    int frame_index;
    int quality_mode;
};
```

**Rationale**: Grouping hot-path counters together improves cache locality during rendering loop, reducing memory access latency.

---

## Validation and Error Handling

### Counter Overflow Prevention
```c
// Guard against long overflow (unlikely but possible)
#define MAX_COUNTER_VALUE 9223372036854775807L  // LONG_MAX

void metrics_add_ray(t_metrics *m)
{
    if (m->rays_traced < MAX_COUNTER_VALUE)
        m->rays_traced++;
}
```

### Division by Zero Guards
```c
double calculate_fps(t_metrics *m)
{
    long sum = /* calculate sum */;
    if (sum == 0)
        return (0.0);  // Guard against division by zero
    return (1000000.0 / (sum / 60.0));
}
```

### Sanity Checks
```c
void metrics_validate(t_metrics *m)
{
    // Ensure frame_index stays in bounds
    if (m->frame_index < 0 || m->frame_index >= 60)
        m->frame_index = 0;
    
    // Ensure counters are non-negative
    if (m->rays_traced < 0)
        m->rays_traced = 0;
    
    // Efficiency should be in [0, 100]
    // (handled in display logic, not storage)
}
```

---

## Display Format Specifications

### Performance Section Format

**Basic Mode** (show_info=0):
```
=== Performance ===
FPS: 58.3
Frame: 16.8ms
BVH: ON
```

**Detailed Mode** (show_info=1):
```
=== Performance ===
FPS: 58.3
Frame: 16.8ms
BVH: ON

[I] BVH Details:
  Ray-Box: 45,231
  Ray-Obj: 12,960
  Nodes Visit: 8,542
  Nodes Skip: 36,689
  Avg Tests/Ray: 10.0
  Efficiency: 90.0%

Tree Stats:
  Nodes: 127
  Depth: 7
  Avg Objs/Leaf: 2.3
```

### Formatting Functions

```c
void hud_format_fps(char *buf, double fps);
    // Output: "58.3" (1 decimal place)

void hud_format_time_ms(char *buf, long time_us);
    // Output: "16.8ms" (microseconds converted to milliseconds)

void hud_format_counter(char *buf, long count);
    // Output: "45,231" (thousands separator for readability)

void hud_format_percent(char *buf, double pct);
    // Output: "90.0%" (1 decimal place)

void hud_format_bvh_status(char *buf, int enabled);
    // Output: "ON" or "OFF"
```

---

## Summary

**Core Entities**:
1. ✅ `t_metrics` - Per-frame performance counters and timing
2. ✅ `t_bvh` - Tree structure statistics and build metrics
3. ✅ Display state - Reuses existing HUD flags

**Derived Metrics**:
1. ✅ FPS (rolling 60-frame average)
2. ✅ BVH Efficiency (percentage reduction in tests)
3. ✅ Avg Tests Per Ray (spatial culling indicator)

**Data Flow**: Scene load → Build BVH → Frame loop (increment counters) → Calculate metrics → Display in HUD

**Ready for Contract Design**: All entities defined, relationships clear, validation rules established.
