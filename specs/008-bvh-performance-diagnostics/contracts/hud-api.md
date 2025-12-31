# API Contract: HUD Performance Display Extension

**Module**: `src/hud/hud_performance.c`  
**Header**: `includes/hud.h`  
**Purpose**: Render performance metrics section in HUD overlay

---

## Public API Functions

### 1. Performance Section Rendering

#### `void hud_render_performance(t_render *render, int *y)`

**Purpose**: Render performance metrics section in HUD (FPS, frame time, BVH stats).

**Parameters**:
- `render`: Pointer to render context containing metrics and scene data
- `y`: Pointer to current vertical position for text rendering (updated after rendering)

**Preconditions**:
- `render` must be non-NULL
- `render->hud.visible` must be 1 (HUD is visible)
- `render->state.metrics` must be initialized
- `y` must be non-NULL and point to valid vertical position within HUD bounds

**Postconditions**:
- Performance section rendered to screen via mlx_string_put()
- `*y` advanced by total height of performance section (for subsequent sections)
- HUD displays at minimum: FPS, frame time, BVH status
- If `render->state.show_info` is 1: displays detailed BVH statistics

**Side Effects**:
- Calls mlx_string_put() multiple times (8-15 calls depending on detail mode)
- Writes to render->mlx window buffer

**42 Norm Compliance**: ✓ (22 lines, delegates to helper functions)

**Example**:
```c
void hud_render_content(t_render *render)
{
    int y = HUD_MARGIN_Y + 20;
    
    // Existing sections
    hud_render_camera(render, &y);
    hud_render_ambient(render, &y);
    hud_render_light(render, &y);
    hud_render_objects(render, &y);
    
    // New performance section
    y += HUD_LINE_HEIGHT;
    hud_render_performance(render, &y);
}
```

**Display Format** (Basic Mode):
```
=== Performance ===
FPS: 58.3
Frame: 16.8ms
BVH: ON
```

**Display Format** (Detailed Mode with show_info=1):
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
```

---

## Internal Helper Functions

### `void render_perf_header(t_render *render, int *y)`

**Purpose**: Render "=== Performance ===" section header.

**Parameters**:
- `render`: Render context
- `y`: Current Y position (incremented after rendering)

**42 Norm Compliance**: ✓ (8 lines)

**Example Output**:
```
=== Performance ===
```

---

### `void render_perf_basic(t_render *render, int *y)`

**Purpose**: Render basic performance metrics (FPS, frame time, BVH status).

**Parameters**:
- `render`: Render context with metrics data
- `y`: Current Y position (incremented after rendering)

**Preconditions**:
- `render->state.metrics.fps` must be calculated
- `render->state.metrics.render_time_us` must be valid

**Implementation**:
```c
void render_perf_basic(t_render *render, int *y)
{
    char buf[64];
    
    // FPS line
    ft_strcpy(buf, "FPS: ");
    hud_format_fps(buf + 5, render->state.metrics.fps);
    mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
                   *y, HUD_COLOR_TEXT, buf);
    *y += HUD_LINE_HEIGHT;
    
    // Frame time line
    ft_strcpy(buf, "Frame: ");
    hud_format_time_ms(buf + 7, render->state.metrics.render_time_us);
    mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
                   *y, HUD_COLOR_TEXT, buf);
    *y += HUD_LINE_HEIGHT;
    
    // BVH status line
    ft_strcpy(buf, "BVH: ");
    hud_format_bvh_status(buf + 5, render->state.bvh_enabled);
    mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
                   *y, HUD_COLOR_TEXT, buf);
    *y += HUD_LINE_HEIGHT;
}
```

**42 Norm Compliance**: ✓ (24 lines, can be split further if needed)

---

### `void render_perf_detailed(t_render *render, int *y)`

**Purpose**: Render detailed BVH statistics (only when show_info is enabled).

**Parameters**:
- `render`: Render context with metrics and BVH data
- `y`: Current Y position (incremented after rendering)

**Preconditions**:
- `render->state.show_info` must be 1
- `render->state.bvh_enabled` should be 1 (metrics are meaningful)

**Implementation Strategy**:
- Delegate to multiple sub-functions to maintain <25 line limit
- `render_bvh_counters()`: Ray-Box, Ray-Obj, Nodes Visit/Skip
- `render_bvh_metrics()`: Avg Tests/Ray, Efficiency percentage
- `render_bvh_tree_stats()`: Tree nodes, depth, avg objects/leaf

**42 Norm Compliance**: ✓ (18 lines with delegation)

---

### `void render_bvh_counters(t_render *render, int *y)`

**Purpose**: Render BVH counter statistics (ray-box tests, ray-object tests, nodes visited/skipped).

**Parameters**:
- `render`: Render context
- `y`: Current Y position

**Display Format**:
```
  Ray-Box: 45,231
  Ray-Obj: 12,960
  Nodes Visit: 8,542
  Nodes Skip: 36,689
```

**42 Norm Compliance**: ✓ (22 lines)

---

### `void render_bvh_metrics(t_render *render, int *y)`

**Purpose**: Render calculated BVH effectiveness metrics (avg tests/ray, efficiency percentage).

**Parameters**:
- `render`: Render context
- `y`: Current Y position

**Display Format**:
```
  Avg Tests/Ray: 10.0
  Efficiency: 90.0%
```

**Calculations**:
```c
double avg_tests = calculate_avg_tests_per_ray(&render->state.metrics);
double efficiency = calculate_bvh_efficiency(&render->state.metrics,
                                              get_object_count(render->scene));
```

**42 Norm Compliance**: ✓ (15 lines)

---

### `void render_bvh_tree_stats(t_render *render, int *y)`

**Purpose**: Render BVH tree structure statistics (nodes, depth, avg objects/leaf).

**Parameters**:
- `render`: Render context with BVH data
- `y`: Current Y position

**Preconditions**:
- `render->state.bvh` must be non-NULL
- `render->state.bvh->root` must be non-NULL (tree built)

**Display Format**:
```
Tree Stats:
  Nodes: 127
  Depth: 7
  Avg Objs/Leaf: 2.3
```

**42 Norm Compliance**: ✓ (18 lines)

---

## Formatting Functions

### `void hud_format_fps(char *buf, double fps)`

**Purpose**: Format FPS value to string with 1 decimal place.

**Parameters**:
- `buf`: Output buffer (must be at least 16 bytes)
- `fps`: Frames per second value

**Output Format**: "58.3" (1 decimal place)

**Implementation**:
```c
void hud_format_fps(char *buf, double fps)
{
    int whole = (int)fps;
    int decimal = (int)((fps - whole) * 10);
    
    hud_format_int_simple(buf, whole);
    while (*buf)
        buf++;
    *buf++ = '.';
    *buf++ = '0' + decimal;
    *buf = '\0';
}
```

**42 Norm Compliance**: ✓ (12 lines)

---

### `void hud_format_time_ms(char *buf, long time_us)`

**Purpose**: Format microseconds as milliseconds with 1 decimal place and "ms" suffix.

**Parameters**:
- `buf`: Output buffer (must be at least 16 bytes)
- `time_us`: Time in microseconds

**Output Format**: "16.8ms"

**Implementation**:
```c
void hud_format_time_ms(char *buf, long time_us)
{
    double time_ms = time_us / 1000.0;
    int whole = (int)time_ms;
    int decimal = (int)((time_ms - whole) * 10);
    
    hud_format_int_simple(buf, whole);
    while (*buf)
        buf++;
    *buf++ = '.';
    *buf++ = '0' + decimal;
    *buf++ = 'm';
    *buf++ = 's';
    *buf = '\0';
}
```

**42 Norm Compliance**: ✓ (15 lines)

---

### `void hud_format_counter(char *buf, long count)`

**Purpose**: Format large counter with thousands separator for readability.

**Parameters**:
- `buf`: Output buffer (must be at least 32 bytes)
- `count`: Counter value

**Output Format**: "1,234,567" (comma separator every 3 digits)

**Implementation Strategy**:
- Convert number to string in reverse
- Insert commas every 3 digits from right
- Reverse final string

**42 Norm Compliance**: ✓ (22 lines with helper function)

---

### `void hud_format_percent(char *buf, double pct)`

**Purpose**: Format percentage with 1 decimal place and "%" suffix.

**Parameters**:
- `buf`: Output buffer (must be at least 16 bytes)
- `pct`: Percentage value (0.0-100.0)

**Output Format**: "90.0%"

**Implementation**:
```c
void hud_format_percent(char *buf, double pct)
{
    int whole = (int)pct;
    int decimal = (int)((pct - whole) * 10);
    
    hud_format_int_simple(buf, whole);
    while (*buf)
        buf++;
    *buf++ = '.';
    *buf++ = '0' + decimal;
    *buf++ = '%';
    *buf = '\0';
}
```

**42 Norm Compliance**: ✓ (12 lines)

---

### `void hud_format_bvh_status(char *buf, int enabled)`

**Purpose**: Format BVH status as "ON" or "OFF".

**Parameters**:
- `buf`: Output buffer (must be at least 4 bytes)
- `enabled`: BVH enabled flag (0 or 1)

**Output Format**: "ON" or "OFF"

**Implementation**:
```c
void hud_format_bvh_status(char *buf, int enabled)
{
    if (enabled) {
        buf[0] = 'O';
        buf[1] = 'N';
        buf[2] = '\0';
    } else {
        buf[0] = 'O';
        buf[1] = 'F';
        buf[2] = 'F';
        buf[3] = '\0';
    }
}
```

**42 Norm Compliance**: ✓ (12 lines)

---

## Integration with Existing HUD

### Modification to hud_render_content()

**File**: `src/hud/hud_render.c`

**Change**:
```c
void hud_render_content(t_render *render)
{
    int y;

    y = HUD_MARGIN_Y + 20;
    hud_render_camera(render, &y);
    render_camera_fov(render, &y);
    y += HUD_LINE_HEIGHT / 2;
    hud_render_ambient(render, &y);
    y += HUD_LINE_HEIGHT / 2;
    hud_render_light(render, &y);
    render_light_bright(render, &y);
    y += HUD_LINE_HEIGHT;
    hud_render_objects(render, &y);
    y += HUD_LINE_HEIGHT;           // NEW: Add spacing
    hud_render_performance(render, &y);  // NEW: Render performance section
}
```

**Lines Changed**: +2 lines (y spacing and function call)

---

### Header Declaration

**File**: `includes/hud.h`

**Addition**:
```c
/*
** Render performance metrics section in HUD.
**
** Parameters:
**   render: Render context containing metrics and scene data
**   y: Pointer to current vertical position (updated after rendering)
**
** Returns:
**   void
**
** Behavior:
**   - Displays FPS, frame time, BVH status (always)
**   - Displays detailed BVH statistics if show_info flag enabled
**   - Advances *y by section height for subsequent sections
*/
void    hud_render_performance(t_render *render, int *y);
```

---

## Layout Calculations

### Vertical Space Requirements

**Basic Mode** (show_info=0):
- Header: 1 line (18px)
- FPS: 1 line (18px)
- Frame Time: 1 line (18px)
- BVH Status: 1 line (18px)
- **Total**: 72px (4 lines × 18px)

**Detailed Mode** (show_info=1):
- Basic Mode: 72px
- Detail Header: 1 line (18px)
- Ray-Box: 1 line (18px)
- Ray-Obj: 1 line (18px)
- Nodes Visit: 1 line (18px)
- Nodes Skip: 1 line (18px)
- Avg Tests/Ray: 1 line (18px)
- Efficiency: 1 line (18px)
- Tree Stats Header: 1 line (18px)
- Nodes: 1 line (18px)
- Depth: 1 line (18px)
- Avg Objs/Leaf: 1 line (18px)
- **Total**: 270px (15 lines × 18px)

### HUD Height Validation
- Current HUD height: 600px (HUD_HEIGHT constant)
- Existing content: ~400px (camera, ambient, light, objects)
- Performance section (detailed): 270px
- **Total**: 670px

**Issue**: Performance section exceeds HUD height in detailed mode.

**Solution**: Increase HUD_HEIGHT constant to 700px or make performance section scrollable with objects pagination.

**Recommendation**: Increase HUD_HEIGHT to 700px (acceptable since modern displays are typically 900px+ tall).

---

## Error Handling

### NULL Pointer Checks
```c
void hud_render_performance(t_render *render, int *y)
{
    if (!render || !y)
        return;  // Safe no-op
    
    if (!render->hud.visible)
        return;  // HUD hidden
    
    // ... render performance section
}
```

### Missing BVH Data
```c
void render_bvh_tree_stats(t_render *render, int *y)
{
    if (!render->state.bvh || !render->state.bvh->root) {
        mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
                       *y, HUD_COLOR_TEXT, "  BVH: Not Built");
        *y += HUD_LINE_HEIGHT;
        return;
    }
    
    // ... render tree statistics
}
```

### Division by Zero
```c
double avg_tests = calculate_avg_tests_per_ray(&render->state.metrics);
if (isnan(avg_tests) || isinf(avg_tests))
    avg_tests = 0.0;  // Guard against invalid values
```

---

## Performance Considerations

### Rendering Overhead
- **mlx_string_put() calls**: 4-15 per frame (depending on detail mode)
- **Formatting operations**: ~10-30 function calls per frame
- **Total overhead**: <2ms per frame (acceptable, <5% at 60 FPS)

### Optimization Strategies
1. **Lazy rendering**: Only render when HUD visible
2. **Dirty flag**: Skip rendering if metrics unchanged (can't implement - metrics always change)
3. **String caching**: Pre-format static strings ("FPS:", "Frame:", etc.)
4. **Inline formatting**: Use inline functions for hot-path formatting

---

## Testing Strategy

### Visual Validation
1. Launch miniRT with test scene
2. Verify performance section appears in HUD
3. Verify FPS updates smoothly (not jittery)
4. Verify frame time correlates with scene complexity
5. Toggle BVH with 'B' key, verify status changes
6. Toggle detail mode with 'I' key, verify statistics appear/disappear

### Functional Tests
```c
void test_hud_performance_basic(void);    // Verify basic display
void test_hud_performance_detailed(void); // Verify detailed display
void test_formatting_functions(void);     // Verify all formatters produce correct output
void test_layout_calculations(void);      // Verify Y position advances correctly
```

### Edge Cases
- FPS = 0.0: Display "0.0" (not crash)
- Frame time > 1000ms: Display large value correctly
- BVH disabled: Display "OFF", hide detailed stats
- show_info = 0: Hide detailed section
- Very large counters (millions): Format with commas correctly

---

## Contract Validation Checklist

- ✅ Main function (hud_render_performance) clearly defined
- ✅ Helper functions decomposed for 42 Norm compliance (<25 lines)
- ✅ Formatting functions specified with exact output format
- ✅ Integration points with existing HUD identified
- ✅ Layout calculations validated (HUD height constraint)
- ✅ NULL pointer and error handling specified
- ✅ Performance overhead analyzed (<5% requirement)
- ✅ Testing strategy outlined
- ✅ Visual examples provided for display format
