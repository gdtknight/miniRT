# API Contract: Metrics Module

**Module**: `src/render/metrics.c`  
**Header**: `includes/metrics.h`  
**Purpose**: Core performance metrics collection and calculation operations

---

## Public API Functions

### 1. Timer Operations

#### `void timer_start(struct timeval *tv)`

**Purpose**: Record current timestamp for frame timing.

**Parameters**:
- `tv`: Pointer to timeval structure to store timestamp

**Preconditions**:
- `tv` must be non-NULL
- `tv` must point to valid memory

**Postconditions**:
- `tv` contains current system time
- Safe to call `timer_elapsed_us()` or `timer_elapsed_ms()` with this timestamp

**Side Effects**: None (pure timing operation)

**42 Norm Compliance**: ✓ (5 lines)

**Example**:
```c
struct timeval start;
timer_start(&start);
// ... do work ...
long elapsed = timer_elapsed_us(&start);
```

---

#### `long timer_elapsed_us(struct timeval *start)`

**Purpose**: Calculate microseconds elapsed since start timestamp.

**Parameters**:
- `start`: Pointer to timeval from previous `timer_start()` call

**Returns**: Elapsed time in microseconds (long)

**Preconditions**:
- `start` must be non-NULL
- `start` must contain valid timestamp from `timer_start()`

**Postconditions**:
- Returns non-negative microsecond count
- Returns 0 if current time < start time (clock skew)

**Side Effects**: None (read-only operation)

**42 Norm Compliance**: ✓ (8 lines)

**Example**:
```c
struct timeval start;
timer_start(&start);
render_frame();
long frame_time = timer_elapsed_us(&start);  // e.g., 16800
```

---

#### `double timer_elapsed_ms(struct timeval *start)`

**Purpose**: Calculate milliseconds elapsed since start timestamp.

**Parameters**:
- `start`: Pointer to timeval from previous `timer_start()` call

**Returns**: Elapsed time in milliseconds (double, with fractional precision)

**Preconditions**:
- `start` must be non-NULL
- `start` must contain valid timestamp from `timer_start()`

**Postconditions**:
- Returns non-negative millisecond count with 3 decimal places precision
- Returns 0.0 if current time < start time (clock skew)

**Side Effects**: None (read-only operation)

**42 Norm Compliance**: ✓ (6 lines)

**Example**:
```c
struct timeval start;
timer_start(&start);
build_bvh();
double build_time = timer_elapsed_ms(&start);  // e.g., 12.345
printf("BVH build: %.2fms\n", build_time);
```

---

### 2. Metrics Lifecycle

#### `void metrics_init(t_metrics *metrics)`

**Purpose**: Initialize metrics structure with zero values.

**Parameters**:
- `metrics`: Pointer to metrics structure to initialize

**Preconditions**:
- `metrics` must be non-NULL
- `metrics` must point to allocated memory (stack or heap)

**Postconditions**:
- All counters set to 0
- `frame_index` set to 0
- `frame_times_us[]` array zeroed
- `fps` set to 0.0
- `quality_mode` set to 0

**Side Effects**: Writes to metrics structure only

**42 Norm Compliance**: ✓ (15 lines with loop unrolling or memset)

**Example**:
```c
t_metrics metrics;
metrics_init(&metrics);
// Ready for first frame
```

---

#### `void metrics_start_frame(t_metrics *metrics)`

**Purpose**: Reset per-frame counters and start frame timer.

**Parameters**:
- `metrics`: Pointer to initialized metrics structure

**Preconditions**:
- `metrics` must be non-NULL
- `metrics_init()` must have been called previously

**Postconditions**:
- `rays_traced` = 0
- `intersect_tests` = 0
- `bvh_nodes_visited` = 0
- `bvh_tests_skipped` = 0
- `bvh_box_tests` = 0
- `start_time` contains current timestamp

**Side Effects**: Resets counters, affects timing measurements

**42 Norm Compliance**: ✓ (8 lines)

**Example**:
```c
while (running) {
    metrics_start_frame(&metrics);
    render_frame();
    metrics_end_frame(&metrics);
}
```

---

#### `void metrics_end_frame(t_metrics *metrics)`

**Purpose**: Calculate frame time, update FPS rolling average.

**Parameters**:
- `metrics`: Pointer to metrics structure with started frame

**Preconditions**:
- `metrics` must be non-NULL
- `metrics_start_frame()` must have been called this frame

**Postconditions**:
- `render_time_us` contains frame duration
- `frame_times_us[frame_index]` updated with current frame time
- `frame_index` incremented (wraps at 60)
- `fps` recalculated from rolling average
- `frame_count` incremented

**Side Effects**: Updates multiple fields, advances circular buffer index

**42 Norm Compliance**: ✓ (18 lines, delegates to helper functions)

**Example**:
```c
metrics_start_frame(&metrics);
render_scene(scene);
metrics_end_frame(&metrics);
printf("Frame: %.2fms, FPS: %.1f\n", 
       metrics.render_time_us / 1000.0, metrics.fps);
```

---

### 3. Counter Operations

#### `void metrics_add_ray(t_metrics *metrics)`

**Purpose**: Increment ray counter (called once per traced ray).

**Parameters**:
- `metrics`: Pointer to active metrics structure

**Preconditions**:
- `metrics` must be non-NULL
- `metrics_start_frame()` called this frame

**Postconditions**:
- `rays_traced` incremented by 1

**Side Effects**: Increments counter only

**42 Norm Compliance**: ✓ (3 lines, inline candidate)

**Performance**: <1ns per call (single increment)

**Example**:
```c
for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
        t_ray ray = camera_get_ray(x, y);
        metrics_add_ray(&metrics);
        trace_ray(ray, scene, &metrics);
    }
}
```

---

#### `void metrics_add_intersect_test(t_metrics *metrics)`

**Purpose**: Increment object intersection test counter.

**Parameters**:
- `metrics`: Pointer to active metrics structure

**Preconditions**:
- `metrics` must be non-NULL
- `metrics_start_frame()` called this frame

**Postconditions**:
- `intersect_tests` incremented by 1

**Side Effects**: Increments counter only

**42 Norm Compliance**: ✓ (3 lines, inline candidate)

**Performance**: <1ns per call (single increment)

**Example**:
```c
int intersect_sphere(t_ray ray, t_sphere sphere, t_hit_record *hit,
                     t_metrics *metrics)
{
    metrics_add_intersect_test(metrics);
    // ... sphere intersection math
}
```

---

#### `void metrics_add_bvh_node_visit(t_metrics *metrics)`

**Purpose**: Increment BVH node traversal counter.

**Parameters**:
- `metrics`: Pointer to active metrics structure

**Preconditions**:
- `metrics` must be non-NULL
- `metrics_start_frame()` called this frame
- BVH is enabled

**Postconditions**:
- `bvh_nodes_visited` incremented by 1

**Side Effects**: Increments counter only

**42 Norm Compliance**: ✓ (3 lines, inline candidate)

**Performance**: <1ns per call (single increment)

**Example**:
```c
int bvh_node_intersect(t_bvh_node *node, t_ray ray, t_hit_record *hit,
                       t_scene *scene, t_metrics *metrics)
{
    metrics_add_bvh_node_visit(metrics);
    // ... traversal logic
}
```

---

#### `void metrics_add_bvh_skip(t_metrics *metrics)`

**Purpose**: Increment BVH branch culling counter.

**Parameters**:
- `metrics`: Pointer to active metrics structure

**Preconditions**:
- `metrics` must be non-NULL
- `metrics_start_frame()` called this frame
- BVH is enabled

**Postconditions**:
- `bvh_tests_skipped` incremented by 1

**Side Effects**: Increments counter only

**42 Norm Compliance**: ✓ (3 lines, inline candidate)

**Performance**: <1ns per call (single increment)

**Example**:
```c
if (!aabb_intersect(node->bounds, ray, &t_min, &t_max)) {
    metrics_add_bvh_skip(metrics);  // Branch pruned
    return (0);
}
```

---

#### `void metrics_add_bvh_box_test(t_metrics *metrics)`

**Purpose**: Increment ray-AABB intersection test counter.

**Parameters**:
- `metrics`: Pointer to active metrics structure

**Preconditions**:
- `metrics` must be non-NULL
- `metrics_start_frame()` called this frame
- BVH is enabled

**Postconditions**:
- `bvh_box_tests` incremented by 1

**Side Effects**: Increments counter only

**42 Norm Compliance**: ✓ (3 lines, inline candidate)

**Performance**: <1ns per call (single increment)

**Example**:
```c
int aabb_intersect(t_aabb box, t_ray ray, double *t_min, double *t_max,
                   t_metrics *metrics)
{
    metrics_add_bvh_box_test(metrics);
    // ... AABB intersection math
}
```

---

### 4. Diagnostic Operations

#### `void metrics_log_render(t_metrics *metrics)`

**Purpose**: Output current metrics to terminal (optional, for debugging).

**Parameters**:
- `metrics`: Pointer to metrics structure with completed frame

**Preconditions**:
- `metrics` must be non-NULL
- `metrics_end_frame()` called this frame

**Postconditions**:
- Terminal displays frame statistics
- Metrics structure unchanged

**Side Effects**: Writes to stdout via printf()

**42 Norm Compliance**: ✓ (20 lines with multiple printf calls)

**Example Output**:
```
Frame #1234:
  Time: 16.8ms | FPS: 58.3
  Rays: 1,296,000 | Tests: 12,960,000
  BVH Nodes: 8,542 | Skipped: 36,689
  Efficiency: 90.0%
```

**Example**:
```c
if (verbose_logging) {
    metrics_end_frame(&metrics);
    metrics_log_render(&metrics);
}
```

---

## Internal Helper Functions

### `double calculate_fps(t_metrics *metrics)`

**Purpose**: Calculate FPS from rolling frame time average.

**Parameters**:
- `metrics`: Pointer to metrics with frame_times_us[] buffer

**Returns**: Calculated frames per second (double)

**Formula**: `FPS = 1,000,000 / avg(frame_times_us)`

**Edge Cases**:
- Returns 0.0 if sum == 0 (guards against division by zero)
- First 60 frames: Uses partial average (sum / valid_frame_count)

**42 Norm Compliance**: ✓ (12 lines)

---

### `double calculate_bvh_efficiency(t_metrics *metrics, int object_count)`

**Purpose**: Calculate percentage reduction in intersection tests.

**Parameters**:
- `metrics`: Pointer to metrics with test counters
- `object_count`: Total objects in scene

**Returns**: Efficiency percentage 0.0-100.0 (double)

**Formula**: `(1 - actual_tests / naive_tests) × 100`

**Edge Cases**:
- Returns 0.0 if object_count == 0
- Returns 0.0 if rays_traced == 0
- Clamps negative values to 0.0 (shouldn't occur)

**42 Norm Compliance**: ✓ (10 lines)

---

### `double calculate_avg_tests_per_ray(t_metrics *metrics)`

**Purpose**: Calculate average object tests per ray.

**Parameters**:
- `metrics`: Pointer to metrics with test counters

**Returns**: Average tests per ray (double)

**Formula**: `intersect_tests / rays_traced`

**Edge Cases**:
- Returns 0.0 if rays_traced == 0

**42 Norm Compliance**: ✓ (5 lines)

---

## Integration Points

### Render Loop Integration
```c
void render_frame(t_render *render)
{
    metrics_start_frame(&render->state.metrics);
    
    // Render pixels
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            t_ray ray = camera_get_ray(x, y);
            trace_ray(ray, render->scene, &render->state.metrics);
        }
    }
    
    metrics_end_frame(&render->state.metrics);
}
```

### BVH Integration
```c
int bvh_intersect(t_bvh *bvh, t_ray ray, t_hit_record *hit,
                  t_scene *scene, t_metrics *metrics)
{
    if (!bvh || !bvh->enabled)
        return (intersect_scene_naive(ray, scene, hit, metrics));
    
    return (bvh_node_intersect(bvh->root, ray, hit, scene, metrics));
}
```

---

## Error Handling

### NULL Pointer Checks
All public functions must check for NULL pointers and handle gracefully:
```c
void metrics_add_ray(t_metrics *metrics)
{
    if (!metrics)
        return;  // Safe no-op
    metrics->rays_traced++;
}
```

### Counter Overflow Protection
```c
void metrics_add_ray(t_metrics *metrics)
{
    if (!metrics)
        return;
    if (metrics->rays_traced < LONG_MAX)
        metrics->rays_traced++;
}
```

### Timer Validation
```c
long timer_elapsed_us(struct timeval *start)
{
    struct timeval now;
    long elapsed;
    
    if (!start)
        return (0);
    
    gettimeofday(&now, NULL);
    elapsed = (now.tv_sec - start->tv_sec) * 1000000L
              + (now.tv_usec - start->tv_usec);
    
    if (elapsed < 0)  // Clock skew
        return (0);
    
    return (elapsed);
}
```

---

## Performance Guarantees

### Counter Operations
- **Latency**: <1ns per increment (single instruction)
- **Overhead**: <5% total frame time (measured)
- **Cache**: Hot-path counters grouped for cache locality

### Timer Operations
- **Precision**: Microsecond (1μs)
- **Accuracy**: System-dependent, typically <10μs error
- **Overhead**: ~100ns per gettimeofday() call

### FPS Calculation
- **Frequency**: Once per frame (not per ray)
- **Latency**: ~1μs (60 iterations, simple arithmetic)
- **Smoothing**: 60-frame rolling average (~1 second at 60 FPS)

---

## Testing Strategy

### Unit Tests
```c
void test_metrics_init(void);           // Verify zero initialization
void test_timer_precision(void);        // Verify microsecond accuracy
void test_fps_calculation(void);        // Verify rolling average math
void test_efficiency_calculation(void); // Verify percentage formula
void test_counter_overflow(void);       // Verify LONG_MAX handling
```

### Integration Tests
```bash
./test_metrics_overhead.sh    # Verify <5% overhead requirement
./test_metrics_accuracy.sh    # Compare calculated vs expected values
```

### Manual Validation
1. Render simple scene (5 objects), verify rays_traced = width × height
2. Toggle BVH on/off, verify efficiency changes dramatically
3. Run for 1000+ frames, verify no counter overflow or FPS drift

---

## Contract Validation Checklist

- ✅ All functions have clear preconditions and postconditions
- ✅ NULL pointer handling specified
- ✅ Edge cases documented (division by zero, overflow)
- ✅ Performance guarantees stated (<5% overhead)
- ✅ 42 Norm compliance verified for all functions
- ✅ Integration points with render loop and BVH defined
- ✅ Error handling strategy established
- ✅ Testing strategy outlined
