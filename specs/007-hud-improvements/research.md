# Research: HUD Improvements Implementation

**Feature**: HUD Improvements and Key Guide  
**Date**: 2025-12-30  
**Status**: Complete

## Overview

This document consolidates research findings for implementing semi-transparent HUD overlays, key guide system, real-time selection updates, and resolution changes in miniRT ray tracer. All decisions documented with rationale and alternatives considered.

## Research Topics

### 1. Alpha Blending in MiniLibX Without Native Support

**Decision**: Implement manual alpha blending via direct pixel buffer manipulation using fixed alpha constant.

**Rationale**:
- MiniLibX does not provide native alpha channel or blending functions
- Direct pixel access via `mlx_get_data_addr()` allows manual RGBA manipulation
- Fixed alpha constant (0.65 already defined as HUD_BG_ALPHA) is sufficient for this use case
- Pre-rendering background buffer with blended pixels enables efficient overlay composition

**Implementation Approach**:
```c
// Pseudo-code for alpha blending
background_color = 0x000000;  // Black
alpha = 0.65;                  // HUD_BG_ALPHA constant
for each pixel in HUD rectangle:
    scene_color = get_pixel_from_scene(x, y);
    blended_r = (1 - alpha) * scene_r + alpha * bg_r;
    blended_g = (1 - alpha) * scene_g + alpha * bg_g;
    blended_b = (1 - alpha) * scene_b + alpha * bg_b;
    set_pixel(x, y, rgb_to_int(blended_r, blended_g, blended_b));
```

**Alternatives Considered**:
- **Native alpha channel**: Rejected - MiniLibX does not support 32-bit ARGB images
- **External graphics library**: Rejected - Violates 42 School constraints (only MLX allowed)
- **Dynamic alpha adjustment**: Rejected - Adds complexity without significant user benefit; fixed alpha provides consistent experience
- **No transparency**: Rejected - Defeats user story requirement for simultaneous scene viewing

**Performance Impact**:
- Background buffer pre-rendered during init: ~0.5ms one-time cost
- Real-time blending during render: Need to composite background then text
- Target: Keep total HUD render ≤2ms (existing constraint)

**Validation**:
- Test with bright scenes (white background) to verify text readability
- Test with dark scenes to verify transparency is visible
- Measure frame time impact with HUD visible vs hidden

---

### 2. Key Guide Layout and Content Design

**Decision**: Compact key guide (300x400px max) in upper-right corner, grouped by function category, semi-transparent background matching main HUD.

**Rationale**:
- Upper-right placement avoids conflict with left-side main HUD
- 300x400px constraint ensures guide remains unobtrusive (<10% screen area)
- Functional grouping improves scanability over alphabetical listing
- Semi-transparent background maintains consistency with main HUD design

**Content Structure**:
```
┌─────────────────────────┐
│ CONTROLS                │
│                         │
│ Navigation:             │
│   ESC - Exit            │
│   H - Toggle HUD        │
│                         │
│ Camera:                 │
│   WASD - Move           │
│   R/F - Pitch up/down   │
│                         │
│ Objects:                │
│   TAB - Next object     │
│   [] - Select object    │
│   Numpad 1-8 - Move     │
│                         │
│ Rendering:              │
│   B - Toggle BVH        │
│   Q - Adaptive samples  │
│   I - Show info         │
└─────────────────────────┘
```

**Alternatives Considered**:
- **Bottom-right placement**: Rejected - Common UI convention uses top for help text
- **Overlay center**: Rejected - Too intrusive, blocks primary viewing area
- **Left side below main HUD**: Rejected - Creates single large overlay blocking significant scene area
- **Comprehensive listing**: Rejected - Exceeds 300x400px constraint, includes rarely-used keys (light controls)
- **Alphabetical order**: Rejected - Less intuitive than functional grouping

**Layout Constants**:
```c
#define KEYGUIDE_WIDTH 300
#define KEYGUIDE_HEIGHT 400
#define KEYGUIDE_MARGIN_X 20
#define KEYGUIDE_MARGIN_Y 20
#define KEYGUIDE_LINE_HEIGHT 16
#define KEYGUIDE_COLOR_TEXT 0xFFFFFF
#define KEYGUIDE_COLOR_HEADING 0x00FFFF
#define KEYGUIDE_BG_ALPHA 0.65
```

**Positioning**:
```c
// Upper-right corner with margin
keyguide_x = WINDOW_WIDTH - KEYGUIDE_WIDTH - KEYGUIDE_MARGIN_X;
keyguide_y = KEYGUIDE_MARGIN_Y;
```

**Validation**:
- New users should identify essential controls within 5 seconds
- Guide should remain readable with transparency enabled
- Total screen area occupied: 120K / 1.3M pixels (~9.2%) ✓

---

### 3. Aspect Ratio and Resolution Change Implications

**Decision**: Update resolution to 1440x900 (16:10 aspect ratio) with systematic constant replacement throughout codebase.

**Rationale**:
- 1440x900 is native resolution for 13-inch MacBook M1 displays (target platform)
- 16:10 aspect ratio maintains compatibility with existing scene designs
- Simple constant update approach avoids complex runtime resizing logic
- Improves viewing comfort (occupies ~80% of screen vs 40% at 800x600)

**Technical Changes Required**:

1. **Window initialization** (`src/window/window.c`):
   ```c
   // OLD: mlx_new_window(mlx, 800, 600, "miniRT")
   // NEW: mlx_new_window(mlx, 1440, 900, "miniRT")
   render->win = mlx_new_window(render->mlx, 1440, 900, "miniRT");
   render->img = mlx_new_image(render->mlx, 1440, 900);
   ```

2. **Camera aspect ratio** (`src/render/camera.c`):
   ```c
   // OLD: aspect_ratio = 800.0 / 600.0;  // 1.333... (4:3)
   // NEW: aspect_ratio = 1440.0 / 900.0; // 1.6 (16:10)
   calc->aspect_ratio = 1440.0 / 900.0;
   ```

3. **Pixel bounds checking** (`src/render/render.c`):
   ```c
   // OLD: if (x < 0 || x >= 800 || y < 0 || y >= 600)
   // NEW: if (x < 0 || x >= 1440 || y < 0 || y >= 900)
   if (x < 0 || x >= 1440 || y < 0 || y >= 900)
       return;
   ```

4. **Documentation/comments**:
   - Update all references to "800x600" in comments and docs
   - Note resolution change in CHANGES.md

**Alternatives Considered**:
- **1920x1080 (16:9 Full HD)**: Rejected - Too large for comfortable viewing on 13-inch displays, forces fullscreen mode
- **1280x720 (16:9 HD)**: Rejected - Non-native resolution for MacBook M1, causes scaling artifacts
- **Dynamic resizing**: Rejected - Adds significant complexity, not required by spec, violates YAGNI principle
- **Maintain 800x600**: Rejected - Doesn't address user comfort requirements on modern displays

**Validation Strategy**:
- Test on MacBook M1 13-inch (1440x900 effective resolution)
- Test on larger displays to ensure window fits comfortably
- Verify HUD elements scale/position correctly at new resolution
- Check camera projection maintains proper scene geometry (no distortion)

**HUD Positioning Impact**:
- Main HUD: Left side, margins maintained (relative positioning unchanged)
- Key guide: Upper-right, positioned relative to new WINDOW_WIDTH
- Both HUDs maintain fixed pixel dimensions (do not scale with resolution)

---

### 4. Real-Time Selection Update Strategy

**Decision**: Call `hud_mark_dirty(render)` in existing selection handlers to trigger immediate HUD refresh.

**Rationale**:
- Dirty flag pattern already implemented in HUD system
- Selection handlers already exist: `handle_object_selection()` in window.c
- Minimal code change required (add single function call)
- Maintains existing event-driven rendering architecture

**Implementation Locations**:

1. **Bracket key selection** (`src/window/window.c:handle_object_selection()`):
   ```c
   // After updating render->selection
   render->selection.type = new_type;
   render->selection.index = new_index;
   hud_mark_dirty(render);  // ADD THIS LINE
   ```

2. **Tab key selection** (`src/hud/hud_navigation.c`):
   ```c
   // hud_select_next() and hud_select_prev() already call hud_mark_dirty()
   // No changes needed - already implemented correctly
   ```

**Alternatives Considered**:
- **Polling in render loop**: Rejected - Inefficient, compares selection every frame unnecessarily
- **Immediate re-render**: Rejected - Could cause flicker if called multiple times rapidly
- **Debounced updates**: Rejected - Adds latency, violates 16ms requirement
- **Event queue system**: Rejected - Overengineering for simple state change

**Performance Considerations**:
- Dirty flag check: ~0.1μs (negligible)
- HUD re-render only when selection changes: ~1-2ms
- No impact on main rendering loop performance

**Validation**:
- Press ']' key, verify selected object highlights immediately in HUD
- Rapidly press selection keys, verify no lag or missed updates
- Measure time from key press to HUD update: must be ≤16ms (1 frame at 60fps)

---

## Summary of Decisions

| Topic | Decision | Rationale |
|-------|----------|-----------|
| Alpha Blending | Manual pixel manipulation with fixed alpha=0.65 | MiniLibX limitation, simple and performant |
| Key Guide Layout | Upper-right, 300x400px, functional grouping | Unobtrusive, scannable, consistent design |
| Resolution Change | 1440x900 with systematic constant updates | Native MacBook M1 resolution, comfort improvement |
| Selection Updates | Dirty flag in selection handlers | Minimal change, leverages existing pattern |

## Open Questions and Risks

**Resolved**:
- ✅ How to implement transparency without native alpha? → Manual blending
- ✅ Where to position key guide? → Upper-right corner
- ✅ What resolution to target? → 1440x900
- ✅ How to trigger HUD updates? → Dirty flag pattern

**Remaining**:
- None. All technical unknowns resolved.

**Risks**:
- **Text readability with transparency**: Mitigated by fixed alpha=0.65 providing sufficient contrast
- **Performance impact of blending**: Mitigated by pre-rendered background buffer
- **Resolution compatibility**: Mitigated by testing on target hardware (MacBook M1)

## Next Steps (Phase 1)

1. Define data structures (data-model.md)
2. Document API contracts (contracts/hud_api.h)
3. Create implementation quickstart (quickstart.md)
4. Update agent context with new decisions
