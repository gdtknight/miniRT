# Data Model: HUD Improvements

**Feature**: HUD Improvements and Key Guide  
**Date**: 2025-12-30  
**Based on**: research.md findings

## Overview

This document defines the data structures, entities, and state management for the HUD improvements feature. All structures follow 42 School norminette requirements and existing miniRT architecture patterns.

## Core Entities

### 1. Key Guide State (`t_keyguide_state`)

**Purpose**: Manages key guide HUD overlay lifecycle, visibility, and rendering resources.

**Structure Definition**:
```c
typedef struct s_keyguide_state
{
    void    *bg_img;        /* Background image buffer (semi-transparent) */
    char    *bg_data;       /* Pixel data pointer from mlx_get_data_addr */
    int     bpp;            /* Bits per pixel (typically 32) */
    int     size_line;      /* Bytes per line in image buffer */
    int     endian;         /* Endianness (little/big) */
    int     visible;        /* Visibility flag: 1=visible, 0=hidden */
    int     x;              /* Upper-left X coordinate on screen */
    int     y;              /* Upper-left Y coordinate on screen */
}   t_keyguide_state;
```

**Field Descriptions**:
- `bg_img`: MiniLibX image handle for pre-rendered semi-transparent background
- `bg_data`: Direct pixel buffer access for efficient rendering
- `bpp`, `size_line`, `endian`: MiniLibX image metadata (obtained from mlx_get_data_addr)
- `visible`: Controlled by 'H' key toggle (same as main HUD)
- `x`, `y`: Calculated as `(WINDOW_WIDTH - KEYGUIDE_WIDTH - MARGIN_X, MARGIN_Y)`

**Lifecycle**:
1. **Initialize**: `keyguide_init()` allocates bg_img, sets position, visible=1
2. **Render**: `keyguide_render()` draws background + text when visible=1
3. **Cleanup**: `keyguide_cleanup()` destroys bg_img via mlx_destroy_image

**Relationships**:
- Embedded in `t_render` structure alongside `t_hud_state`
- Shares visibility state with main HUD (both toggle together)

---

### 2. Enhanced HUD State (`t_hud_state`)

**Purpose**: Existing HUD state structure with transparency support.

**Modified Structure** (no API changes, internal behavior modified):
```c
typedef struct s_hud_state
{
    void    *bg_img;        /* MODIFIED: Now pre-rendered with alpha blending */
    char    *bg_data;       /* Pixel data for background buffer */
    int     bpp;            /* Bits per pixel */
    int     size_line;      /* Line size in bytes */
    int     endian;         /* Endianness */
    int     visible;        /* Visibility: 1=visible, 0=hidden */
    int     current_page;   /* Pagination state for object list */
    int     dirty;          /* Re-render flag: 1=needs update, 0=clean */
}   t_hud_state;
```

**Behavioral Changes**:
- `bg_img` creation now includes alpha blending (background not fully opaque)
- Transparency constant: `HUD_BG_ALPHA = 0.65` (65% opaque, 35% scene visible)

**No Changes Required**:
- API functions remain identical (backward compatible)
- Existing callers unaffected

---

### 3. Window Configuration Constants

**Purpose**: Define window resolution and derived dimensions.

**New Constants** (`includes/window.h` or `includes/minirt.h`):
```c
/* Window resolution - MacBook M1 13-inch native resolution */
#define WINDOW_WIDTH    1440
#define WINDOW_HEIGHT   900

/* Derived aspect ratio for camera calculations */
#define ASPECT_RATIO    (1440.0 / 900.0)  /* 1.6 (16:10) */
```

**Replaces**:
- Hardcoded `800` and `600` literals throughout codebase
- Aspect ratio calculation `800.0 / 600.0` in camera.c

**Impact**:
- `mlx_new_window()` calls: Use WINDOW_WIDTH, WINDOW_HEIGHT
- `mlx_new_image()` calls: Use WINDOW_WIDTH, WINDOW_HEIGHT
- Bounds checks: `if (x >= WINDOW_WIDTH)` instead of `if (x >= 800)`
- Camera projection: Use ASPECT_RATIO constant

---

### 4. Key Guide Layout Constants

**Purpose**: Define key guide dimensions and positioning.

**New Constants** (`includes/keyguide.h`):
```c
/* Key guide dimensions (max 300x400 per requirements) */
#define KEYGUIDE_WIDTH          300
#define KEYGUIDE_HEIGHT         400
#define KEYGUIDE_MARGIN_X       20
#define KEYGUIDE_MARGIN_Y       20

/* Text layout */
#define KEYGUIDE_LINE_HEIGHT    16
#define KEYGUIDE_SECTION_GAP    8

/* Colors */
#define KEYGUIDE_COLOR_TEXT     0xFFFFFF  /* White */
#define KEYGUIDE_COLOR_HEADING  0x00FFFF  /* Cyan */
#define KEYGUIDE_COLOR_BG       0x000000  /* Black */
#define KEYGUIDE_BG_ALPHA       0.65      /* 65% opacity */
```

**Derived Values**:
```c
/* Calculated at runtime */
keyguide_x = WINDOW_WIDTH - KEYGUIDE_WIDTH - KEYGUIDE_MARGIN_X;
keyguide_y = KEYGUIDE_MARGIN_Y;
```

---

### 5. Selection State (Existing, Modified Behavior)

**Purpose**: Track currently selected object for manipulation and HUD highlight.

**Structure** (no changes to definition):
```c
typedef struct s_selection
{
    t_object_type   type;   /* SPHERE, PLANE, CYLINDER, or NONE */
    int             index;  /* Index within type-specific array */
}   t_selection;
```

**Modified Behavior**:
- Changes to `render->selection` now trigger `hud_mark_dirty(render)`
- Ensures HUD updates immediately when selection changes

**Trigger Points** (add dirty flag calls):
1. `handle_object_selection()` - Bracket key handlers
2. `hud_select_next()` - Already implemented
3. `hud_select_prev()` - Already implemented

---

## Entity Relationships

```
t_render (render context)
├── t_hud_state (main HUD on left)
│   ├── bg_img → MiniLibX image (alpha-blended)
│   ├── visible (controlled by 'H' key)
│   └── dirty (triggers re-render)
├── t_keyguide_state (key guide on right)  ← NEW
│   ├── bg_img → MiniLibX image (alpha-blended)
│   └── visible (synced with main HUD)
├── t_selection (current object)
│   └── [changes trigger dirty flag]
└── t_scene (scene data displayed in HUD)
```

**Shared State**:
- `visible` flag: Both HUDs toggle together via 'H' key
- `dirty` flag: Main HUD checks selection changes
- Alpha blending: Both use same transparency algorithm

---

## State Transitions

### HUD Visibility State Machine

```
[VISIBLE] ---(H key)---> [HIDDEN]
    ^                        |
    |-------(H key)-----------
```

**Implementation**:
```c
void hud_toggle(t_render *render)
{
    render->hud.visible = !render->hud.visible;
    render->keyguide.visible = render->hud.visible;  /* Sync both */
}
```

---

### Selection Update Flow

```
[User Input] → [Keypress Handler] → [Update Selection] → [Mark Dirty]
    ↓
[Render Loop] → [Check Dirty] → [Re-render HUD] → [Clear Dirty]
```

**Example Implementation**:
```c
/* In handle_object_selection() */
void handle_object_selection(t_render *render, int keycode)
{
    if (keycode == KEY_BRACKET_RIGHT)
        select_next_object(render);  /* Updates render->selection */
    else if (keycode == KEY_BRACKET_LEFT)
        select_prev_object(render);  /* Updates render->selection */
    
    hud_mark_dirty(render);  /* NEW: Trigger HUD refresh */
}
```

---

## Validation Rules

### Image Buffer Validation

**Rule**: All image buffers MUST be created and destroyed through MiniLibX API.

**Enforcement**:
```c
/* Create */
bg_img = mlx_new_image(mlx, width, height);
if (!bg_img)
    return (error_cleanup());

/* Destroy */
if (bg_img != NULL)
    mlx_destroy_image(mlx, bg_img);
```

**Rationale**: Prevents memory leaks (valgrind requirement), ensures proper GPU resource cleanup.

---

### Transparency Range Validation

**Rule**: Alpha values MUST be in range [0.0, 1.0].

**Enforcement**:
```c
#define HUD_BG_ALPHA 0.65
#define KEYGUIDE_BG_ALPHA 0.65

/* Static assertion at compile time */
_Static_assert(HUD_BG_ALPHA >= 0.0 && HUD_BG_ALPHA <= 1.0, 
               "Alpha must be [0.0, 1.0]");
```

**Rationale**: Out-of-range values produce undefined rendering behavior.

---

### Resolution Bounds Validation

**Rule**: Pixel coordinates MUST be within window bounds.

**Enforcement**:
```c
if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
    return;  /* Skip out-of-bounds pixels */
```

**Rationale**: Out-of-bounds access causes segfaults or buffer overflows.

---

## Memory Management

### Allocation Strategy

| Resource | Allocator | Deallocator | Lifetime |
|----------|-----------|-------------|----------|
| HUD background | `mlx_new_image()` | `mlx_destroy_image()` | Application lifetime |
| Key guide background | `mlx_new_image()` | `mlx_destroy_image()` | Application lifetime |
| Pixel buffers | MiniLibX (internal) | MiniLibX (internal) | Tied to image lifetime |

**Cleanup Order** (critical for proper teardown):
1. Destroy key guide image: `keyguide_cleanup()`
2. Destroy main HUD image: `hud_cleanup()`
3. Destroy window: `mlx_destroy_window()`
4. Destroy MLX connection: (handled by exit)

---

## Performance Considerations

### Memory Footprint

| Component | Size | Formula | Total |
|-----------|------|---------|-------|
| Main HUD background | 960 KB | 400×600×4 bytes | 960 KB |
| Key guide background | 468 KB | 300×400×4 bytes | 468 KB |
| **Total Additional** | **1.43 MB** | | **1.43 MB** |

**Analysis**: Negligible on modern systems (MacBook M1 has 8GB+ RAM).

---

### Rendering Performance Budget

| Operation | Target Time | Current | Budget Remaining |
|-----------|-------------|---------|------------------|
| HUD background composite | 0.5 ms | N/A (new) | 1.5 ms for text |
| Key guide background composite | 0.3 ms | N/A (new) | 1.7 ms for text |
| Text rendering (both HUDs) | 1.5 ms | ~1.2 ms | 0.3 ms margin |
| **Total HUD render** | **≤2 ms** | **~1.2 ms** | **0.8 ms margin** |

**Validation Strategy**: Profile with gprof or manual timing to verify targets met.

---

## Integration Points

### With Existing Systems

1. **Window System** (`src/window/window.c`):
   - Initialize key guide in `init_window()`
   - Cleanup key guide in `close_window()`

2. **Event Handling** (`src/window/window.c`):
   - Modify `hud_toggle()` to sync both HUDs
   - Add `hud_mark_dirty()` to selection handlers

3. **Render Loop** (`src/render/render.c`):
   - Call `keyguide_render()` after `hud_render()`
   - Maintain render order: scene → main HUD → key guide

4. **Camera System** (`src/render/camera.c`):
   - Update aspect ratio calculation to use ASPECT_RATIO constant

---

## Summary

**New Entities**: 1 (t_keyguide_state)  
**Modified Entities**: 1 (t_hud_state behavior)  
**New Constants**: 10 (window resolution, key guide layout)  
**State Transitions**: 2 (visibility toggle, selection update)  
**Validation Rules**: 3 (buffer management, alpha range, bounds checking)  

All entities follow existing miniRT patterns and 42 School constraints. No complex state machines or external dependencies introduced.
