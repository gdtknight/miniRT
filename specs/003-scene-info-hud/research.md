# Phase 0: Research - Scene Information HUD

**Date**: 2025-12-19  
**Feature**: Scene Information HUD  
**Status**: Complete

## Overview

This document consolidates research findings for implementing a HUD system in miniRT using MiniLibX primitives while maintaining 42 Norm compliance and performance requirements.

## Research Areas

### 1. MiniLibX Text Rendering Capabilities

**Research Task**: Investigate MLX text rendering functions and their performance characteristics.

**Decision**: Use `mlx_string_put()` for HUD text rendering

**Rationale**:
- `mlx_string_put()` is the standard MLX function for rendering text directly to window
- Provides built-in font rendering without additional dependencies
- Simple API: `mlx_string_put(mlx_ptr, win_ptr, x, y, color, string)`
- Sufficient for fixed-position HUD text display
- Performance acceptable for ~20-30 text lines (camera info + lighting + objects list)

**Alternatives Considered**:
- **Custom bitmap fonts**: Rejected - adds complexity, requires font file management, violates "no external libraries" constraint
- **XLib text functions directly**: Rejected - breaks MLX abstraction, platform-specific, not 42-school compliant
- **Pre-rendered text to image buffer**: Considered for optimization if needed, deferred to performance testing phase

**Implementation Notes**:
- Each text line requires one `mlx_string_put()` call
- Color format: 0xRRGGBB (integer)
- Text position calculated from top-left corner with fixed line spacing (e.g., 15-20 pixels)

**References**:
- MiniLibX documentation (42 School intranet)
- Existing miniRT codebase patterns

---

### 2. Semi-Transparent Background Rendering

**Research Task**: Determine best approach for rendering semi-transparent dark background behind HUD text using MLX primitives.

**Decision**: Use pre-filled image buffer with alpha-blending simulation

**Rationale**:
- MiniLibX does not provide native alpha channel support
- Simulate transparency by darkening existing pixel values mathematically
- Pre-create HUD background image buffer once, reuse across frames
- Algorithm: `new_color = (existing_color * (1 - alpha)) + (background_color * alpha)`
- For dark semi-transparent: use alpha ~0.5-0.7 with black background (0x000000)
- Performance: One-time buffer creation, fast pixel blending in rendering loop

**Alternatives Considered**:
- **Solid opaque background**: Rejected - poor visual integration with scene, blocks rendered content unnecessarily
- **mlx_pixel_put() per-pixel drawing**: Rejected - too slow for large background areas (e.g., 300x400 pixel HUD region)
- **XLib XRender extension**: Rejected - breaks MLX abstraction, platform-specific, requires external dependencies
- **No background**: Rejected - insufficient contrast, text readability issues with bright/varied scene backgrounds

**Implementation Approach**:
```c
// Pseudo-code for alpha blending
int blend_color(int scene_color, int bg_color, double alpha)
{
    int r_scene = (scene_color >> 16) & 0xFF;
    int g_scene = (scene_color >> 8) & 0xFF;
    int b_scene = scene_color & 0xFF;
    
    int r_bg = (bg_color >> 16) & 0xFF;
    int g_bg = (bg_color >> 8) & 0xFF;
    int b_bg = bg_color & 0xFF;
    
    int r = (int)(r_scene * (1 - alpha) + r_bg * alpha);
    int g = (int)(g_scene * (1 - alpha) + g_bg * alpha);
    int b = (int)(b_scene * (1 - alpha) + b_bg * alpha);
    
    return (r << 16) | (g << 8) | b;
}
```

**Performance Considerations**:
- Create background overlay image once during HUD initialization
- Render background to main image buffer before text overlay
- Benchmark: expect <1ms for 300x400 pixel region on modern hardware

**References**:
- Alpha compositing theory (Porter-Duff)
- Existing miniRT image buffer usage patterns

---

### 3. Object Selection and Navigation State Management

**Research Task**: Design state management for object selection (Tab/Shift+Tab navigation) within 42 Norm constraints.

**Decision**: Add selection state to existing `t_render` structure with unified object indexing

**Rationale**:
- Existing `t_render` structure already contains scene pointer and window context
- Selection state is rendering-related (needed for highlight rendering)
- Unified index approach: treat all objects as single linear list (spheres, then planes, then cylinders)
- Calculation: `global_index = sphere_idx OR (sphere_count + plane_idx) OR (sphere_count + plane_count + cylinder_idx)`
- Simple increment/decrement with wraparound for Tab/Shift+Tab navigation

**Data Structure Extension**:
```c
// Add to existing t_render in window.h (already has t_selection)
typedef struct s_selection
{
    t_obj_type  type;     // OBJ_NONE, OBJ_SPHERE, OBJ_PLANE, OBJ_CYLINDER
    int         index;    // Index within object type array
}   t_selection;

// Already exists in window.h - no changes needed to structure
```

**Navigation Algorithm**:
```c
// Pseudo-code for Tab navigation
void select_next_object(t_render *render)
{
    int total_objects = render->scene->sphere_count 
                      + render->scene->plane_count 
                      + render->scene->cylinder_count;
    
    if (total_objects == 0)
        return;
    
    // Convert current selection to global index
    int global_idx = get_global_index(render->selection, render->scene);
    
    // Increment with wraparound
    global_idx = (global_idx + 1) % total_objects;
    
    // Convert back to type + index
    render->selection = get_selection_from_global(global_idx, render->scene);
}
```

**42 Norm Compliance**:
- Each function ≤25 lines
- Break navigation logic into helper functions: `get_global_index()`, `get_selection_from_global()`, `select_next()`, `select_previous()`
- No complex nested conditionals

**Alternatives Considered**:
- **Separate indices for each object type**: Rejected - complicates Tab navigation logic (need to know which type to cycle to next)
- **Linked list of objects**: Rejected - requires dynamic allocation, conflicts with existing static array storage
- **Global array of object pointers**: Rejected - adds memory overhead, requires initialization, complicates type identification

**References**:
- Existing `t_selection` structure in window.h
- 42 Norm guidelines (function length, complexity)

---

### 4. Pagination System for Large Object Lists

**Research Task**: Design pagination to handle scenes with >10 objects while maintaining readability.

**Decision**: Fixed objects-per-page with page navigation keys (arrow keys)

**Rationale**:
- Display 8 objects per page (fits comfortably in HUD space)
- Page indicator: "Page X/Y" at bottom of object list
- Navigation: Up/Down arrow keys or PgUp/PgDown for page switching
- Current page index stored in HUD state structure
- Only render objects for current page (reduces mlx_string_put calls)

**Pagination State**:
```c
typedef struct s_hud_state
{
    int     visible;           // 0 = hidden, 1 = visible
    int     current_page;      // 0-indexed current page
    int     objects_per_page;  // Fixed at 8
    int     total_pages;       // Calculated from total object count
}   t_hud_state;
```

**Page Calculation**:
```c
// Objects to display calculation
int total_objects = scene->sphere_count + scene->plane_count + scene->cylinder_count;
int total_pages = (total_objects + HUD_OBJECTS_PER_PAGE - 1) / HUD_OBJECTS_PER_PAGE;

// Object range for current page
int start_idx = hud->current_page * HUD_OBJECTS_PER_PAGE;
int end_idx = min(start_idx + HUD_OBJECTS_PER_PAGE, total_objects);
```

**Display Layout**:
```
[Camera Info]
[Ambient Light]
[Light Source]
--- Objects (Page 1/5) ---
Sphere #1: pos(...) rad(...)
Sphere #2: pos(...) rad(...)
...
[Use ↑↓ for pages, Tab to select]
```

**Performance Impact**:
- Reduces string_put calls from potentially 100+ to maximum 8 per frame
- Page switching only updates HUD state (instant, no scene re-render needed)
- Negligible memory overhead (few integers in HUD state)

**Alternatives Considered**:
- **Scrolling list**: Rejected - requires smooth scrolling animation, complex in MLX, performance overhead
- **Dynamic objects-per-page**: Rejected - adds complexity, inconsistent UX
- **No pagination (truncate list)**: Rejected - violates FR-016 requirement, users lose visibility of all objects

**References**:
- Spec requirements FR-016, FR-017, FR-018
- Standard pagination UI patterns

---

### 5. Performance Optimization for 60fps Target

**Research Task**: Identify performance bottlenecks and optimization strategies for <2ms HUD overhead.

**Decision**: Multi-level optimization strategy with lazy updates

**Rationale**:
- HUD rendering is separate pass after main scene rendering
- Most HUD data is static (doesn't change every frame)
- Only re-render HUD when: (1) selection changes, (2) camera moves, (3) object properties change
- Use dirty flag pattern to track when HUD update is needed

**Optimization Strategies**:

1. **Lazy Text Updates**:
   - Pre-format text strings into buffers
   - Only re-format when underlying data changes
   - Detect changes via dirty flags

2. **Minimal mlx_string_put Calls**:
   - Batch text rendering in single loop
   - Skip rendering when HUD hidden
   - Limit displayed objects per page (pagination)

3. **Background Caching**:
   - Create background overlay once at init
   - Reuse same image buffer across frames
   - Only recreate if HUD size changes (never in this implementation)

4. **Conditional Rendering**:
   ```c
   // Pseudo-code
   if (!hud->visible)
       return;  // Skip entire HUD pass
   
   if (!render->dirty && !hud->dirty)
       return;  // No changes, skip update
   
   render_hud_background(render, hud);
   render_hud_text(render, hud, scene);
   hud->dirty = 0;
   ```

**Performance Budget Allocation**:
- Background overlay: <0.5ms (300x400 pixels, single blit)
- Text rendering: ~1.0ms (20-25 mlx_string_put calls)
- State updates: <0.1ms (simple arithmetic, comparisons)
- Total: ~1.6ms average, <2ms worst case

**Measurement Strategy**:
- Use `gettimeofday()` or `clock_gettime()` to measure HUD render time
- Log metrics at DEBUG level (per spec OBS-003)
- Test with maximum object count (100 spheres + 100 planes + 100 cylinders)

**Alternatives Considered**:
- **Pre-render HUD to texture**: Rejected - requires full re-render on any change, no performance gain for frequently updating data
- **OpenGL overlay**: Rejected - not available in MLX, external dependency
- **Multi-threaded rendering**: Rejected - excessive complexity for 2ms target, thread synchronization overhead

**References**:
- Spec performance requirements SC-003, SC-004
- Existing render loop structure in src/render/render.c

---

### 6. Keyboard Input Integration

**Research Task**: Integrate new keybindings (Tab, Shift+Tab, toggle key, page navigation) without conflicts.

**Decision**: Extend existing `handle_key()` function with HUD-specific key handling

**Rationale**:
- Existing keyboard handling in `src/window/window.c` via `handle_key()`
- MLX provides keycode values for Tab, Shift, arrows
- Check existing keybindings to avoid conflicts
- Route HUD-related keys to HUD module functions

**Key Mapping**:
```c
// Key codes (platform-specific, from MLX)
#define KEY_ESC         53   // Existing: exit program
#define KEY_TAB         48   // NEW: next object
#define KEY_SHIFT_L     257  // NEW: modifier for Shift+Tab
#define KEY_H           4    // NEW: toggle HUD visibility
#define KEY_UP          126  // NEW: previous page
#define KEY_DOWN        125  // NEW: next page
#define KEY_LEFT        123  // Existing: camera control
#define KEY_RIGHT       124  // Existing: camera control
```

**Integration Approach**:
```c
// Extend handle_key() in window.c
int handle_key(int keycode, void *param)
{
    t_render *render = (t_render *)param;
    
    if (keycode == KEY_ESC)
        close_window(param);
    
    // NEW: HUD controls
    if (keycode == KEY_H)
        hud_toggle(render);
    else if (keycode == KEY_TAB)
    {
        if (render->shift_pressed)
            hud_select_previous(render);
        else
            hud_select_next(render);
    }
    else if (keycode == KEY_UP)
        hud_page_up(render);
    else if (keycode == KEY_DOWN)
        hud_page_down(render);
    
    // Existing camera/object controls
    // ...
    
    return (0);
}
```

**Shift Key Detection**:
- Track Shift key state in `t_render` structure: `int shift_pressed`
- Set flag in `handle_key()` when Shift pressed (keycode 257/258)
- Clear flag in `handle_key_release()` when Shift released
- Check flag when Tab pressed to determine direction

**Conflict Check**:
- Tab: No existing binding (safe to use)
- 'H' key: Verify not used for camera/object manipulation (check existing code)
- Arrow keys: Up/Down appear unused, Left/Right used for camera - use modifier if needed
- Fallback: Use PgUp/PgDown if arrow conflicts found

**42 Norm Compliance**:
- Keep handle_key() under 25 lines by delegating to HUD functions
- Each HUD key handler is separate function (hud_toggle, hud_select_next, etc.)

**Alternatives Considered**:
- **Separate key handler for HUD**: Rejected - adds complexity, requires event handler multiplexing
- **Modal input (enter HUD mode)**: Rejected - poor UX, users expect direct key access
- **Mouse-based selection**: Out of scope per spec, keyboard-only required

**References**:
- MLX key event documentation
- Existing handle_key() implementation in src/window/window.c
- Spec requirements FR-011, FR-013

---

### 7. Text Formatting for Numerical Values

**Research Task**: Determine consistent number formatting for coordinates, ratios, colors.

**Decision**: Use custom formatting functions with fixed precision (2 decimal places)

**Rationale**:
- Standard printf-family functions don't meet 42 Norm (forbidden functions)
- Implement custom `ft_ftoa()` (float to ASCII) with precision control
- Format: `"%.2f"` equivalent (2 decimal places)
- Buffer size: 32 bytes per formatted number (sufficient for "-999.99\0")

**Formatting Functions**:
```c
// Format float to string with 2 decimal places
// Example: 123.456789 -> "123.46"
void ft_format_float(char *buffer, double value);

// Format vec3 to string "(x, y, z)"
// Example: {1.5, 2.3, 3.7} -> "(1.50, 2.30, 3.70)"
void ft_format_vec3(char *buffer, t_vec3 vec);

// Format color to string "R:255 G:128 B:64"
void ft_format_color(char *buffer, t_color color);
```

**Display Format Examples**:
```
Camera: pos(0.00, 5.00, -10.00) dir(0.00, 0.00, 1.00) fov:70.00
Ambient: ratio:0.20 R:255 G:255 B:255
Light: pos(10.00, 10.00, 0.00) bright:0.80 R:255 G:255 B:255
Sphere #1: pos(0.00, 0.00, 5.00) rad:2.50 R:255 G:0 B:0
```

**42 Norm Implementation**:
- Avoid using `snprintf()` (if not available or to follow strict Norm)
- Implement integer-to-string conversion manually
- Handle negative numbers, decimal point placement
- Keep functions under 25 lines by breaking into sub-functions

**Alternatives Considered**:
- **Variable precision (3+ decimals)**: Rejected - clutters HUD, excessive detail not useful for visual verification
- **Integer-only (round values)**: Rejected - loses precision for small values (e.g., ratios 0.0-1.0)
- **Scientific notation**: Rejected - less readable for humans, unnecessary for miniRT value ranges
- **Using system printf**: Considered if 42 Norm allows (check allowed functions list)

**References**:
- 42 Norm allowed functions list
- Spec requirement FR-015 (2-3 decimal places)

---

## Summary of Key Decisions

| Area | Decision | Primary Rationale |
|------|----------|-------------------|
| Text Rendering | mlx_string_put() | Simple, sufficient performance, no external deps |
| Background | Alpha-blended image buffer | Simulates transparency, one-time creation |
| Selection State | Extend t_render with unified indexing | Fits existing structure, simple navigation |
| Pagination | Fixed 8 objects/page | Maintains readability, reduces render calls |
| Performance | Lazy updates with dirty flags | Meets <2ms target, avoids redundant work |
| Key Handling | Extend existing handle_key() | Minimal changes, no conflicts |
| Formatting | Custom ft_format_*() functions | 42 Norm compliance, controlled precision |

## Next Steps (Phase 1)

1. Define data model for HUD state structure
2. Design API contracts (function signatures in hud.h)
3. Create quickstart guide for HUD usage
4. Validate design against constitution and spec requirements

---

**Research Complete**: 2025-12-19  
**Reviewed By**: Implementation Agent  
**Status**: ✅ Ready for Phase 1 (Design & Contracts)
