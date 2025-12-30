# Quick Start Guide: Scene Information HUD

**Feature**: Scene Information HUD  
**Version**: 1.0.0  
**Date**: 2025-12-19

## Overview

The Scene Information HUD displays parsed RT file information overlaid on the rendered scene. It provides real-time visibility into camera settings, lighting, and object properties with keyboard-based navigation.

## Features at a Glance

- 📊 **Scene Info Display**: Camera, ambient light, light source, and all objects
- ⌨️ **Keyboard Navigation**: Tab/Shift+Tab to cycle through objects
- 📄 **Pagination**: Automatic pagination for scenes with many objects
- 🎨 **Object Highlighting**: Selected objects are highlighted in the list
- 👁️ **Toggle Visibility**: Show/hide HUD with a single keypress
- ⚡ **High Performance**: <2ms rendering overhead

## Quick Start (5 minutes)

### 1. Build the Project

```bash
cd /path/to/miniRT
make
```

### 2. Run with a Scene File

```bash
./miniRT scenes/example.rt
```

### 3. HUD Controls

| Key | Action |
|-----|--------|
| `H` | Toggle HUD visibility on/off |
| `Tab` | Select next object (cycle forward) |
| `Shift+Tab` | Select previous object (cycle backward) |
| `↓` (Down Arrow) | Next page of objects |
| `↑` (Up Arrow) | Previous page of objects |
| `ESC` | Exit program |

## HUD Layout

```
┌────────────────────────────────────┐
│ Camera: pos(0.00, 5.00, -10.00)    │
│         dir(0.00, 0.00, 1.00)      │
│         fov:70.00                  │
│                                    │
│ Ambient: ratio:0.20 R:255 G:255... │
│                                    │
│ Light: pos(10.00, 10.00, 0.00)     │
│        bright:0.80 R:255 G:255...  │
│                                    │
│ --- Objects (Page 1/2) ---         │
│ Sphere #1: pos(...) rad:2.50...    │ <- Selected (green)
│ Sphere #2: pos(...) rad:1.00...    │
│ Plane #1: pos(...) normal(...)     │
│ ...                                │
│                                    │
│ [Tab to select, ↑↓ for pages]     │
└────────────────────────────────────┘
```

## Common Use Cases

### Verify RT File Parsing

**Problem**: Did my RT file parse correctly?

**Solution**: Load the scene and check the HUD. All camera settings, lighting, and objects are displayed exactly as parsed.

```bash
./miniRT my_scene.rt
# HUD shows all parsed values immediately
# Verify camera FOV, light position, object counts match your expectations
```

### Debug Object Positioning

**Problem**: Why isn't my sphere visible?

**Solution**: Use Tab to select the sphere in the HUD and check its position coordinates.

```bash
./miniRT problem_scene.rt
# Press Tab until the problematic sphere is highlighted
# Check its position in the HUD
# Compare with camera position/direction
```

### Navigate Large Scenes

**Problem**: I have 50+ objects, hard to track them all.

**Solution**: HUD automatically paginates. Use ↑↓ arrows to browse pages.

```bash
./miniRT large_scene.rt
# HUD shows "Objects (Page 1/7)"
# Press Down arrow to go to page 2, 3, etc.
# Each page shows 8 objects for readability
```

### Take Clean Screenshots

**Problem**: I want to screenshot the rendered scene without UI.

**Solution**: Press `H` to hide the HUD temporarily.

```bash
./miniRT beautiful_scene.rt
# HUD visible by default
# Press H to hide HUD
# Take screenshot
# Press H again to show HUD
```

## Example Workflow

### Scenario: Creating and Debugging a New Scene

1. **Create RT file** (`my_scene.rt`):
```
A 0.2 255,255,255
C 0,5,-10 0,0,1 70
L 10,10,0 0.8 255,255,255
sp 0,0,5 2.5 255,0,0
pl 0,-5,0 0,1,0 100,100,100
cy 3,0,5 0,1,0 1.5 5.0 0,255,0
```

2. **Run miniRT**:
```bash
./miniRT my_scene.rt
```

3. **Verify in HUD**:
   - Camera at (0, 5, -10) looking toward (0, 0, 1) ✓
   - Ambient light ratio 0.2 ✓
   - Light source at (10, 10, 0) brightness 0.8 ✓
   - 1 sphere, 1 plane, 1 cylinder ✓

4. **Adjust camera** (if you have camera controls):
   - Move camera forward
   - HUD updates position in real-time
   - See new coordinates immediately

5. **Select objects**:
   - Press Tab: Sphere #1 highlighted in green
   - Press Tab: Plane #1 highlighted
   - Press Tab: Cylinder #1 highlighted
   - Press Tab: Wraps back to Sphere #1

## Tips & Best Practices

### Performance

- **HUD rendering is fast** (<2ms overhead), but if you notice slowdown:
  - Hide HUD with `H` during intensive operations
  - Reduce scene complexity (fewer objects)

### Readability

- **Dark backgrounds**: HUD has semi-transparent dark background for contrast
- **Bright backgrounds**: Text remains readable thanks to alpha blending
- **Too much information**: Use pagination (↑↓) to browse, or hide HUD (`H`)

### Navigation

- **Tab navigation follows order**: Spheres → Planes → Cylinders → (wrap)
- **Selection wraps around**: Last object → first object when pressing Tab
- **Shift+Tab reverses**: Navigate backward through the list

### Pagination

- **Automatic**: Activates when >8 objects in scene
- **Page indicators**: "Page X/Y" shown at top of object list
- **Empty pages**: If last page has fewer objects, HUD adjusts automatically

## Troubleshooting

### HUD Not Visible

**Symptom**: Scene renders but no HUD appears.

**Solutions**:
1. Press `H` - maybe HUD was toggled off
2. Check terminal for errors - HUD init failure would log error
3. Verify MiniLibX installation - HUD requires mlx_string_put support

### Text Overlapping or Unreadable

**Symptom**: HUD text is garbled or overlaps.

**Solutions**:
1. Increase window size if very small (minimum 800x600 recommended)
2. Check for conflicting text rendering elsewhere in code
3. Verify HUD constants (LINE_HEIGHT, MARGIN) in hud.h

### Selection Not Highlighting

**Symptom**: Tab key works but no green highlight appears.

**Solutions**:
1. Verify object count > 0 (empty scene has nothing to select)
2. Check terminal logs for selection errors
3. Ensure HUD_COLOR_HIGHLIGHT is distinct from HUD_COLOR_TEXT

### Page Navigation Not Working

**Symptom**: ↑↓ keys don't change pages.

**Solutions**:
1. Check if scene has >8 objects (pagination only activates when needed)
2. Verify "Page X/Y" indicator shows multiple pages
3. Check for key binding conflicts with existing controls

### Performance Issues

**Symptom**: Frame rate drops when HUD is visible.

**Solutions**:
1. Hide HUD with `H` to confirm HUD is the cause
2. Check scene complexity (100+ objects can slow text rendering)
3. Review logs for HUD render time (should be <2ms)
4. Verify dirty flag optimization is working (HUD shouldn't redraw every frame unless needed)

## Advanced Usage

### Integrating HUD with Custom Controls

If you add camera or object manipulation controls:

```c
// After modifying camera position
render->scene->camera.position = new_position;
hud_mark_dirty(render);  // Tell HUD to update display
```

### Customizing HUD Appearance

Edit constants in `includes/hud.h`:

```c
#define HUD_WIDTH 500              // Wider HUD
#define HUD_HEIGHT 700             // Taller HUD
#define HUD_LINE_HEIGHT 20         // More spacing
#define HUD_OBJECTS_PER_PAGE 10    // More objects per page
#define HUD_COLOR_HIGHLIGHT 0xFF0000  // Red highlight instead of green
```

Rebuild after changes:
```bash
make re
```

### Logging and Debugging

HUD logs events and metrics:

```bash
# Enable debug logging (if your project supports log levels)
./miniRT scenes/test.rt --log-level=DEBUG

# Check logs for HUD events
cat logs/miniRT.log | grep HUD
```

**Log Examples**:
- `INFO: HUD initialized (visible=1)` - Startup
- `INFO: HUD toggled (visible=0)` - Hidden
- `DEBUG: HUD render time: 1.2ms` - Performance metric
- `ERROR: HUD background image creation failed` - Init error

## API Reference (For Developers)

### Public Functions

See `specs/001-scene-info-hud/contracts/hud_api.h` for full API documentation.

**Initialization**:
```c
int hud_init(t_hud_state *hud, void *mlx, void *win);
void hud_cleanup(t_hud_state *hud, void *mlx);
```

**Rendering**:
```c
void hud_render(t_render *render);
void hud_mark_dirty(t_render *render);
```

**User Interaction**:
```c
void hud_toggle(t_render *render);
void hud_select_next(t_render *render);
void hud_select_prev(t_render *render);
void hud_page_down(t_render *render);
void hud_page_up(t_render *render);
```

## Further Reading

- **Full Specification**: See `specs/001-scene-info-hud/spec.md`
- **Implementation Plan**: See `specs/001-scene-info-hud/plan.md`
- **Data Model**: See `specs/001-scene-info-hud/data-model.md`
- **API Contract**: See `specs/001-scene-info-hud/contracts/hud_api.h`

## Support

For issues or questions:

1. Check this quickstart guide first
2. Review the troubleshooting section
3. Check project documentation in `docs/`
4. Open an issue on GitHub with:
   - RT file content
   - Terminal output (errors/logs)
   - Expected vs actual behavior

---

**Quickstart Guide Version**: 1.0.0  
**Last Updated**: 2025-12-19  
**Maintained By**: miniRT Development Team
