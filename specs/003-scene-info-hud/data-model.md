# Phase 1: Data Model - Scene Information HUD

**Date**: 2025-12-19  
**Feature**: Scene Information HUD  
**Status**: Complete

## Overview

This document defines the data structures, entities, and their relationships for the HUD feature. All structures follow 42 Norm conventions and integrate with existing miniRT data model.

## Core Entities

### 1. HUD State (`t_hud_state`)

**Purpose**: Manages HUD visibility, pagination, and rendering state.

**Structure Definition**:
```c
typedef struct s_hud_state
{
    int     visible;           /* 0 = hidden, 1 = visible */
    int     current_page;      /* 0-indexed current page for pagination */
    int     objects_per_page;  /* Number of objects to display per page (8) */
    int     total_pages;       /* Total pages calculated from object count */
    int     dirty;             /* Flag: 1 = needs redraw, 0 = clean */
    void    *bg_img;           /* Background overlay image buffer */
    char    *bg_data;          /* Background image pixel data pointer */
}   t_hud_state;
```

**Field Descriptions**:
- `visible`: Controls whether HUD is rendered. Toggled by 'H' key.
- `current_page`: Tracks which page of objects is currently displayed (for pagination).
- `objects_per_page`: Constant value (8), defines page size.
- `total_pages`: Computed from total object count divided by objects_per_page.
- `dirty`: Optimization flag - when 0, skip HUD rendering to save performance.
- `bg_img`: MiniLibX image pointer for semi-transparent background overlay.
- `bg_data`: Direct pixel access pointer from mlx_get_data_addr().

**Lifecycle**:
- Created during `init_window()` call
- Updated on key events (toggle, page navigation, selection changes)
- Destroyed during `close_window()` cleanup

**Validation Rules**:
- `visible` must be 0 or 1
- `current_page` must be >= 0 and < total_pages
- `objects_per_page` is constant (8)
- `dirty` must be 0 or 1
- `bg_img` and `bg_data` must be valid pointers or NULL

**Related Operations**:
- `hud_init()`: Initialize HUD state with defaults
- `hud_toggle()`: Flip visible flag
- `hud_page_next()`: Increment current_page with wraparound
- `hud_page_prev()`: Decrement current_page with wraparound
- `hud_mark_dirty()`: Set dirty flag to trigger redraw
- `hud_cleanup()`: Free background image buffer

---

### 2. Object Selection (`t_selection`)

**Purpose**: Tracks currently selected object for highlighting in HUD.

**Structure Definition** (already exists in window.h):
```c
typedef enum e_obj_type
{
    OBJ_NONE = 0,      /* No object selected */
    OBJ_SPHERE,        /* Sphere selected */
    OBJ_PLANE,         /* Plane selected */
    OBJ_CYLINDER       /* Cylinder selected */
}   t_obj_type;

typedef struct s_selection
{
    t_obj_type  type;   /* Type of selected object */
    int         index;  /* Index within type-specific array */
}   t_selection;
```

**Field Descriptions**:
- `type`: Discriminator indicating which object type is selected.
- `index`: Array index within the selected type (e.g., spheres[index]).

**State Transitions**:
```
OBJ_NONE -> OBJ_SPHERE (Tab pressed, first sphere selected)
OBJ_SPHERE -> OBJ_SPHERE (Tab within sphere array)
OBJ_SPHERE -> OBJ_PLANE (Tab beyond last sphere, first plane selected)
OBJ_PLANE -> OBJ_CYLINDER (Tab beyond last plane, first cylinder selected)
OBJ_CYLINDER -> OBJ_SPHERE (Tab beyond last cylinder, wrap to first sphere)
```

**Navigation Logic**:
```
Global Index = {
    sphere_index                                    if type == OBJ_SPHERE
    sphere_count + plane_index                      if type == OBJ_PLANE
    sphere_count + plane_count + cylinder_index     if type == OBJ_CYLINDER
}
```

**Validation Rules**:
- `type` must be valid enum value (OBJ_NONE to OBJ_CYLINDER)
- `index` must be >= 0
- `index` must be < count for selected type (sphere_count, plane_count, cylinder_count)
- When type == OBJ_NONE, index is irrelevant (typically -1 or 0)

**Related Operations**:
- `hud_select_next()`: Navigate to next object (Tab key)
- `hud_select_prev()`: Navigate to previous object (Shift+Tab key)
- `hud_get_global_index()`: Convert selection to linear index
- `hud_set_selection_from_global()`: Convert linear index to selection

---

### 3. Render Context (`t_render`)

**Purpose**: Central rendering context containing all state needed for window and HUD rendering.

**Structure Definition** (exists in window.h, updated):
```c
typedef struct s_render
{
    void        *mlx;          /* MiniLibX connection pointer */
    void        *win;          /* Window pointer */
    void        *img;          /* Main scene image buffer */
    char        *img_data;     /* Scene image pixel data */
    int         bpp;           /* Bits per pixel */
    int         size_line;     /* Bytes per line in image */
    int         endian;        /* Endianness flag */
    t_scene     *scene;        /* Pointer to scene data */
    t_selection selection;     /* Currently selected object */
    int         dirty;         /* Scene dirty flag (needs re-render) */
    int         low_quality;   /* Low quality mode flag */
    int         shift_pressed; /* NEW: Shift key state for Shift+Tab */
    t_hud_state hud;           /* NEW: HUD state structure */
}   t_render;
```

**New Fields**:
- `shift_pressed`: Tracks Shift key state for Shift+Tab navigation (0 = released, 1 = pressed)
- `hud`: Embedded HUD state structure (not a pointer, allocated inline)

**Field Relationships**:
- `scene` pointer provides access to all scene data (camera, lights, objects)
- `selection` determines which object is highlighted in HUD
- `dirty` flag triggers scene re-render, separate from `hud.dirty`
- `hud.visible` controls HUD rendering independent of scene rendering

**Validation Rules**:
- `mlx`, `win`, `img` must be valid pointers (non-NULL when initialized)
- `scene` must point to valid, parsed scene structure
- `shift_pressed` must be 0 or 1
- `hud` embedded struct follows `t_hud_state` validation rules

**Related Operations**:
- `init_window()`: Allocates t_render, initializes all fields including HUD
- `render_loop()`: Main loop that calls scene and HUD rendering based on dirty flags
- `handle_key()`: Updates selection, shift_pressed, hud state based on input
- `close_window()`: Cleanup of all resources including HUD background image

---

### 4. Scene Data (`t_scene`)

**Purpose**: Contains all parsed data from RT file. No modifications needed for HUD feature.

**Structure Definition** (existing in minirt.h):
```c
typedef struct s_scene
{
    t_ambient       ambient;         /* Ambient lighting */
    t_camera        camera;          /* Camera configuration */
    t_light         light;           /* Point light source */
    t_shadow_config shadow_config;   /* Shadow settings */
    t_sphere        spheres[100];    /* Array of spheres */
    int             sphere_count;    /* Number of spheres */
    t_plane         planes[100];     /* Array of planes */
    int             plane_count;     /* Number of planes */
    t_cylinder      cylinders[100];  /* Array of cylinders */
    int             cylinder_count;  /* Number of cylinders */
    int             has_ambient;     /* Flag: ambient defined */
    int             has_camera;      /* Flag: camera defined */
    int             has_light;       /* Flag: light defined */
}   t_scene;
```

**HUD Usage**:
- Read-only access from HUD rendering functions
- `camera`, `ambient`, `light` displayed in HUD header
- Object arrays (spheres, planes, cylinders) iterated for object list
- Counts used to calculate pagination (total_pages, object range per page)

**No Modifications Required**: HUD is purely a display feature, does not modify scene data.

---

## Data Flow

### HUD Initialization Flow
```
1. main() calls init_window(scene)
2. init_window() allocates t_render
3. init_window() calls hud_init(&render->hud, render->mlx, render->win)
4. hud_init() sets:
   - visible = 1 (default visible)
   - current_page = 0
   - objects_per_page = 8
   - dirty = 1 (force initial render)
5. hud_init() creates background overlay:
   - bg_img = mlx_new_image(mlx, HUD_WIDTH, HUD_HEIGHT)
   - bg_data = mlx_get_data_addr(bg_img, ...)
   - Fill bg_data with alpha-blended dark pixels
6. init_window() initializes selection:
   - type = OBJ_NONE
   - index = -1
7. Returns render context to main()
```

### HUD Render Flow
```
1. render_loop() called by MLX event loop
2. Check if hud.visible == 0 -> skip HUD rendering
3. Check if render->dirty || render->hud.dirty -> render needed
4. Call hud_render(render)
5. hud_render():
   a. Render background overlay (mlx_put_image_to_window)
   b. Calculate text positions (line-by-line, y = base_y + line * LINE_HEIGHT)
   c. Render camera info (mlx_string_put)
   d. Render ambient light info
   e. Render light source info
   f. Render object list header ("Objects (Page X/Y)")
   g. Calculate object range for current page
   h. Iterate objects in page range:
      - Format object info string
      - Check if object matches selection -> use highlight color
      - Call mlx_string_put with formatted string
   i. Render page navigation hint
6. Set hud.dirty = 0
7. Return
```

### Selection Navigation Flow
```
1. User presses Tab key
2. MLX calls handle_key(KEY_TAB, render)
3. handle_key() checks shift_pressed flag
4. If shift_pressed == 0:
   - Call hud_select_next(render)
5. hud_select_next():
   a. Calculate total objects (sphere_count + plane_count + cylinder_count)
   b. Convert current selection to global_index
   c. Increment global_index with wraparound (% total_objects)
   d. Convert global_index back to (type, index)
   e. Update render->selection
   f. Set render->hud.dirty = 1
6. Next render_loop() iteration redraws HUD with new selection highlighted
```

### Page Navigation Flow
```
1. User presses Down arrow key
2. MLX calls handle_key(KEY_DOWN, render)
3. handle_key() calls hud_page_down(render)
4. hud_page_down():
   a. Increment render->hud.current_page
   b. If current_page >= total_pages, wraparound to 0
   c. Set render->hud.dirty = 1
5. Next render_loop() iteration redraws HUD with new page of objects
```

### Toggle Visibility Flow
```
1. User presses 'H' key
2. MLX calls handle_key(KEY_H, render)
3. handle_key() calls hud_toggle(render)
4. hud_toggle():
   a. Flip render->hud.visible (0 <-> 1)
   b. Log toggle event (OBS-002)
5. Next render_loop():
   - If hud.visible == 0, skip HUD rendering entirely
   - Scene continues to render normally
```

---

## Memory Management

### Allocations
| Resource | Allocation Point | Deallocation Point | Size |
|----------|------------------|-------------------|------|
| t_render | init_window() | close_window() | sizeof(t_render) |
| hud.bg_img | hud_init() | hud_cleanup() | HUD_WIDTH × HUD_HEIGHT × 4 bytes |
| Scene data | parse_scene() | cleanup_scene() | sizeof(t_scene) |

### Lifecycle Diagram
```
main()
├─ parse_scene() ──> t_scene allocated (static in this implementation)
├─ init_window()
│  ├─ malloc(t_render)
│  ├─ mlx_new_image() ──> main scene image
│  ├─ hud_init()
│  │  └─ mlx_new_image() ──> HUD background image
│  └─ return t_render*
├─ mlx_loop()
│  └─ [render_loop() repeatedly called]
└─ close_window()
   ├─ hud_cleanup()
   │  └─ mlx_destroy_image(hud.bg_img)
   ├─ mlx_destroy_image(render->img)
   ├─ free(render)
   └─ cleanup_scene()
```

### Memory Safety Rules
- All `mlx_new_image()` calls paired with `mlx_destroy_image()`
- t_render freed after all MLX resources destroyed
- No dynamic allocations within HUD rendering loop (all buffers static or on stack)
- Scene data lifetime exceeds render context lifetime

---

## Validation & Constraints

### Input Validation
- **current_page bounds**: `0 <= current_page < total_pages`
- **selection index bounds**: `0 <= index < count[type]`
- **objects_per_page**: Fixed constant (8), no validation needed
- **visibility flag**: `visible ∈ {0, 1}`

### Performance Constraints
- **HUD render time**: ≤ 2ms per frame (measured via gettimeofday)
- **Background overlay**: Created once, reused (no per-frame allocation)
- **Text rendering**: Maximum 25 mlx_string_put calls per frame (header + 8 objects + footer)
- **Dirty flag optimization**: Skip rendering when dirty == 0 and visible == 1

### 42 Norm Constraints
- All structure definitions in header files
- No typedef'd pointers (use `t_hud_state` not `t_hud_state*`)
- Structure fields aligned, clear naming
- No nested structures beyond 1 level (t_render contains t_hud_state is acceptable)
- Comments follow Doxygen style in implementation

---

## State Diagrams

### HUD Visibility State
```
┌─────────┐  'H' key   ┌─────────┐
│ Visible │ ◄─────────► │ Hidden  │
│ (1)     │  pressed    │ (0)     │
└─────────┘             └─────────┘
    │                       │
    │ Renders HUD           │ Skips HUD
    │ each frame            │ rendering
```

### Selection State Transitions (Example: 2 spheres, 1 plane, 1 cylinder)
```
        Tab
┌─────► OBJ_SPHERE[0] ──Tab──► OBJ_SPHERE[1] ──Tab──►
│           │                                         │
│      Shift+Tab                                  Shift+Tab
│           │                                         │
│           ▼                      Tab                ▼
└───── OBJ_CYLINDER[0] ◄──Tab── OBJ_PLANE[0] ───────►
             │                      ▲
             │                      │
             └──────Shift+Tab───────┘
```

### Pagination State (Example: 10 objects, 8 per page = 2 pages)
```
        Down Arrow
┌─────► Page 0 ──────────► Page 1 ──┐
│       (objects 0-7)     (objects 8-9) │
│           │                  │        │
│      Up Arrow            Up Arrow     │
│           │                  │        │
└───────────┴──────────────────┴────────┘
         (wraparound at boundaries)
```

---

## Integration Points

### With Existing Systems

1. **Window System** (`src/window/window.c`):
   - `handle_key()`: Extended with HUD key handling
   - `render_loop()`: Calls `hud_render()` after scene rendering
   - `init_window()`: Calls `hud_init()`
   - `close_window()`: Calls `hud_cleanup()`

2. **Scene Parser** (`src/parser/`):
   - No changes needed
   - HUD reads scene data via `render->scene` pointer

3. **Render System** (`src/render/`):
   - No changes to core rendering
   - HUD rendering is separate, post-scene overlay

4. **MiniLibX**:
   - Uses `mlx_string_put()` for text
   - Uses `mlx_put_image_to_window()` for background
   - Uses `mlx_new_image()` / `mlx_destroy_image()` for background buffer

### New Module Boundary

```
┌──────────────────────────────────────┐
│         HUD Module (src/hud/)        │
├──────────────────────────────────────┤
│ Public API (includes/hud.h):         │
│  - hud_init()                        │
│  - hud_cleanup()                     │
│  - hud_render()                      │
│  - hud_toggle()                      │
│  - hud_select_next()                 │
│  - hud_select_prev()                 │
│  - hud_page_up()                     │
│  - hud_page_down()                   │
│                                      │
│ Private Implementation:              │
│  - Text formatting functions         │
│  - Background rendering              │
│  - Index conversion helpers          │
└──────────────────────────────────────┘
         ▲                  │
         │ Read             │ Write
         │ t_scene          │ t_hud_state
         │                  │ t_selection
         │                  ▼
┌─────────────────────────────────────┐
│      Window Module (src/window/)    │
│  - Calls HUD functions              │
│  - Manages render context           │
│  - Dispatches keyboard events       │
└─────────────────────────────────────┘
```

---

## Summary

**Key Entities**:
1. `t_hud_state` - HUD-specific state (visibility, pagination, dirty flag, background image)
2. `t_selection` - Object selection tracking (existing, used for highlighting)
3. `t_render` - Extended with HUD state and shift key tracking
4. `t_scene` - Read-only data source (no modifications)

**Data Relationships**:
- `t_render` contains `t_hud_state` (composition)
- `t_render` contains `t_selection` (composition)
- `t_render` points to `t_scene` (reference)
- `t_hud_state` owns background image resource (ownership)

**Memory Footprint**:
- `t_hud_state`: ~40 bytes (6 ints + 2 pointers)
- Background image: ~500KB (300x400 pixels × 4 bytes/pixel)
- Total HUD overhead: <1MB

**Validation Summary**:
- All bounds checked on indices and page numbers
- Flags restricted to valid values (0/1)
- Resource allocation paired with deallocation
- No dynamic allocation in hot paths (render loop)

---

**Data Model Complete**: 2025-12-19  
**Reviewed By**: Implementation Agent  
**Status**: ✅ Ready for Contract Definition
