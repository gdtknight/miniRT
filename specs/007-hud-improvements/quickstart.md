# Quickstart: HUD Improvements Implementation

**Feature**: HUD Improvements and Key Guide  
**Date**: 2025-12-30  
**Estimated Time**: 6-8 hours  
**Difficulty**: Medium (requires MiniLibX familiarity)

## Prerequisites

- ✅ miniRT project building successfully (`make`)
- ✅ Familiarity with MiniLibX image buffer API
- ✅ Understanding of existing HUD system (`src/hud/`, `includes/hud.h`)
- ✅ Access to MacBook M1 or similar display for testing 1440x900 resolution

## Implementation Checklist

### Phase 1: Resolution Update (1-2 hours)

**Goal**: Update window resolution from 800x600 to 1440x900 throughout codebase.

#### 1.1 Define Constants

**File**: `includes/window.h` (create if not exists) or `includes/minirt.h`

```c
/* Window resolution - MacBook M1 13-inch native resolution */
#define WINDOW_WIDTH    1440
#define WINDOW_HEIGHT   900
#define ASPECT_RATIO    (1440.0 / 900.0)  /* 1.6 (16:10) */
```

**Validation**: Grep for hardcoded `800` and `600` literals to find all update locations.

```bash
grep -rn "800\|600" src/ includes/ | grep -v ".backup"
```

---

#### 1.2 Update Window Initialization

**File**: `src/window/window.c`

**Find**:
```c
render->win = mlx_new_window(render->mlx, 800, 600, "miniRT");
render->img = mlx_new_image(render->mlx, 800, 600);
```

**Replace with**:
```c
render->win = mlx_new_window(render->mlx, WINDOW_WIDTH, WINDOW_HEIGHT, "miniRT");
render->img = mlx_new_image(render->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
```

---

#### 1.3 Update Camera Aspect Ratio

**File**: `src/render/camera.c`

**Find**:
```c
calc->aspect_ratio = 800.0 / 600.0;
```

**Replace with**:
```c
calc->aspect_ratio = ASPECT_RATIO;
```

---

#### 1.4 Update Pixel Bounds Checking

**File**: `src/render/render.c`

**Find**:
```c
if (x < 0 || x >= 800 || y < 0 || y >= 600)
```

**Replace with**:
```c
if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
```

---

#### 1.5 Update Documentation

**Files**: Comments in `includes/minirt.h`, `src/render/render.c`

**Find**: All references to "800x600"  
**Replace with**: "1440x900"

---

**Validation**:
```bash
make re
./miniRT scenes/simple.rt
# Verify window opens at 1440x900
# Verify scene renders without distortion
# Verify HUD appears in correct position
```

---

### Phase 2: HUD Transparency (2-3 hours)

**Goal**: Add semi-transparent background to existing main HUD.

#### 2.1 Implement Alpha Blending Utilities

**File**: `src/hud/hud_transparency.c` (new)

```c
#include "hud.h"

int	blend_colors(int bg_color, int fg_color, double alpha)
{
	int	r;
	int	g;
	int	b;

	r = (int)((1.0 - alpha) * ((bg_color >> 16) & 0xFF)
		+ alpha * ((fg_color >> 16) & 0xFF));
	g = (int)((1.0 - alpha) * ((bg_color >> 8) & 0xFF)
		+ alpha * ((fg_color >> 8) & 0xFF));
	b = (int)((1.0 - alpha) * (bg_color & 0xFF)
		+ alpha * (fg_color & 0xFF));
	return ((r << 16) | (g << 8) | b);
}

int	get_pixel(char *img_data, int x, int y, int size_line, int bpp)
{
	char	*pixel;

	pixel = img_data + (y * size_line + x * (bpp / 8));
	return (*(int *)pixel);
}

void	set_pixel(char *img_data, int x, int y, int color,
	int size_line, int bpp)
{
	char	*pixel;

	pixel = img_data + (y * size_line + x * (bpp / 8));
	*(int *)pixel = color;
}
```

**Add to hud.h**:
```c
int		blend_colors(int bg_color, int fg_color, double alpha);
int		get_pixel(char *img_data, int x, int y, int size_line, int bpp);
void	set_pixel(char *img_data, int x, int y, int color,
			int size_line, int bpp);
```

---

#### 2.2 Modify HUD Rendering

**File**: `src/hud/hud_render.c`

**Modify `hud_render_background()` function**:

```c
void	hud_render_background(t_render *render)
{
	int	x;
	int	y;
	int	scene_color;
	int	blended;

	y = HUD_MARGIN_Y;
	while (y < HUD_MARGIN_Y + HUD_HEIGHT)
	{
		x = HUD_MARGIN_X;
		while (x < HUD_MARGIN_X + HUD_WIDTH)
		{
			scene_color = get_pixel(render->img_data, x, y,
				render->size_line, render->bpp);
			blended = blend_colors(scene_color, HUD_COLOR_BG, HUD_BG_ALPHA);
			set_pixel(render->img_data, x, y, blended,
				render->size_line, render->bpp);
			x++;
		}
		y++;
	}
}
```

**Note**: This may exceed 25-line limit. Split into helper functions if needed:
- `hud_blend_row()` - Process single row
- `hud_render_background()` - Loop over rows

---

**Validation**:
```bash
make re
./miniRT scenes/simple.rt
# Press 'H' to toggle HUD
# Verify HUD background is semi-transparent (scene visible behind)
# Verify text remains readable (white on dark background)
```

---

### Phase 3: Key Guide Implementation (2-3 hours)

**Goal**: Create new key guide HUD in upper-right corner.

#### 3.1 Create Key Guide Header

**File**: `includes/keyguide.h` (new)

Copy from `specs/007-hud-improvements/contracts/keyguide.h`.

---

#### 3.2 Add Key Guide State to Render Context

**File**: `includes/render_state.h` or appropriate header

```c
#include "keyguide.h"

typedef struct s_render
{
	/* ... existing fields ... */
	t_hud_state			hud;
	t_keyguide_state	keyguide;  /* NEW */
	/* ... */
}	t_render;
```

---

#### 3.3 Implement Key Guide Initialization

**File**: `src/keyguide/keyguide_init.c` (new directory)

```c
#include "keyguide.h"

int	keyguide_init(t_keyguide_state *keyguide, void *mlx, void *win)
{
	keyguide->bg_img = mlx_new_image(mlx, KEYGUIDE_WIDTH, KEYGUIDE_HEIGHT);
	if (!keyguide->bg_img)
		return (-1);
	keyguide->bg_data = mlx_get_data_addr(keyguide->bg_img,
		&keyguide->bpp, &keyguide->size_line, &keyguide->endian);
	keyguide->x = WINDOW_WIDTH - KEYGUIDE_WIDTH - KEYGUIDE_MARGIN_X;
	keyguide->y = KEYGUIDE_MARGIN_Y;
	keyguide->visible = 1;
	return (0);
}
```

---

#### 3.4 Implement Key Guide Rendering

**File**: `src/keyguide/keyguide_render.c` (new)

```c
#include "keyguide.h"

void	keyguide_render(t_render *render)
{
	int	y;

	if (!render->keyguide.visible)
		return ;
	keyguide_render_background(render);
	y = render->keyguide.y + 20;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 10, y, KEYGUIDE_COLOR_HEADING, "CONTROLS");
	y += 30;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 10, y, KEYGUIDE_COLOR_TEXT, "Navigation:");
	y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, y, KEYGUIDE_COLOR_TEXT, "ESC - Exit");
	/* ... continue for all controls ... */
}

void	keyguide_render_background(t_render *render)
{
	/* Similar to HUD background rendering with alpha blending */
	/* Render semi-transparent rectangle at keyguide position */
}
```

**Full content structure** (from research.md):
```
CONTROLS

Navigation:
  ESC - Exit
  H - Toggle HUD

Camera:
  WASD - Move
  R/F - Pitch up/down

Objects:
  TAB - Next object
  [] - Select object
  Numpad 1-8 - Move

Rendering:
  B - Toggle BVH
  Q - Adaptive samples
  I - Show info
```

---

#### 3.5 Implement Key Guide Cleanup

**File**: `src/keyguide/keyguide_cleanup.c` (new)

```c
#include "keyguide.h"

void	keyguide_cleanup(t_keyguide_state *keyguide, void *mlx)
{
	if (keyguide->bg_img)
	{
		mlx_destroy_image(mlx, keyguide->bg_img);
		keyguide->bg_img = NULL;
		keyguide->bg_data = NULL;
	}
}
```

---

#### 3.6 Integrate Key Guide into Window Lifecycle

**File**: `src/window/window.c`

**In window initialization function**:
```c
int	init_window(t_render *render)
{
	/* ... existing HUD init ... */
	if (hud_init(&render->hud, render->mlx, render->win) != 0)
		return (-1);
	
	/* ADD: Initialize key guide */
	if (keyguide_init(&render->keyguide, render->mlx, render->win) != 0)
	{
		hud_cleanup(&render->hud, render->mlx);
		return (-1);
	}
	
	return (0);
}
```

**In close_window() function**:
```c
int	close_window(void *param)
{
	t_render	*render;

	render = (t_render *)param;
	keyguide_cleanup(&render->keyguide, render->mlx);  /* ADD */
	hud_cleanup(&render->hud, render->mlx);
	cleanup_all(render->scene, render);
	exit(0);
	return (0);
}
```

**In render loop** (after HUD render):
```c
void	render_loop(t_render *render)
{
	/* ... scene rendering ... */
	hud_render(render);
	keyguide_render(render);  /* ADD */
	/* ... present frame ... */
}
```

---

#### 3.7 Sync Key Guide Visibility with Main HUD

**File**: `src/hud/hud_toggle.c`

**Modify**:
```c
void	hud_toggle(t_render *render)
{
	render->hud.visible = !render->hud.visible;
	render->keyguide.visible = render->hud.visible;  /* ADD */
	/* ... existing logging ... */
}
```

---

**Validation**:
```bash
make re
./miniRT scenes/simple.rt
# Verify key guide appears in upper-right corner
# Press 'H' - verify both HUDs toggle together
# Verify text is readable with transparent background
# Verify key guide doesn't exceed 300x400 pixels (visual estimate)
```

---

### Phase 4: Real-Time Selection Updates (0.5 hours)

**Goal**: Trigger HUD refresh immediately when object selection changes.

#### 4.1 Update Bracket Key Handler

**File**: `src/window/window.c`

**Find `handle_object_selection()` or equivalent**:
```c
/* Current code (example) */
else if (keycode == KEY_BRACKET_LEFT || keycode == KEY_BRACKET_RIGHT)
	handle_object_selection(render, keycode);
```

**Modify handler to add dirty flag**:
```c
void	handle_object_selection(t_render *render, int keycode)
{
	/* ... existing selection logic ... */
	
	/* ADD at end of function */
	hud_mark_dirty(render);
}
```

**Alternative**: If bracket keys call `hud_select_next()`/`hud_select_prev()`:
- Those functions already call `hud_mark_dirty()` ✅
- No changes needed, verify behavior works

---

**Validation**:
```bash
make re
./miniRT scenes/multi_objects.rt
# Press ']' key
# Verify HUD immediately highlights newly selected object (green text)
# Rapidly press ']' multiple times
# Verify HUD updates smoothly without lag
```

---

### Phase 5: Update Makefile (0.5 hours)

**File**: `Makefile`

**Add new source files**:
```makefile
SRC = src/main.c \
      src/hud/hud_init.c \
      src/hud/hud_render.c \
      src/hud/hud_transparency.c \  # NEW
      src/keyguide/keyguide_init.c \  # NEW
      src/keyguide/keyguide_render.c \  # NEW
      src/keyguide/keyguide_cleanup.c \  # NEW
      # ... rest of sources ...
```

**Validation**:
```bash
make fclean
make
# Verify clean build with no errors
# Run norminette on all modified/new files
norminette src/hud/*.c src/keyguide/*.c includes/keyguide.h
```

---

## Testing Checklist

### Visual Tests

- [ ] Window opens at 1440x900 resolution
- [ ] Scene renders without distortion at new resolution
- [ ] Main HUD has semi-transparent background (scene visible behind)
- [ ] HUD text remains readable (white on dark background)
- [ ] Key guide appears in upper-right corner
- [ ] Key guide has semi-transparent background matching main HUD
- [ ] Key guide displays all essential controls (per FR-009)
- [ ] Both HUDs toggle together with 'H' key
- [ ] Object selection updates HUD immediately (green highlight)

### Functional Tests

- [ ] All existing controls still work (WASD, RF, bracket keys, etc.)
- [ ] Tab navigation updates HUD
- [ ] Bracket key selection updates HUD
- [ ] Camera movement updates HUD (position/direction)
- [ ] HUD pagination still works (up/down arrows)

### Performance Tests

- [ ] Frame rate maintains 60fps during camera movement
- [ ] HUD render time ≤2ms (measure with profiling if possible)
- [ ] Selection update latency ≤16ms (visual observation)

### Quality Tests

- [ ] `make` builds without warnings
- [ ] `norminette` passes on all files (zero errors/warnings)
- [ ] `valgrind` reports zero memory leaks
- [ ] Test on both macOS and Linux (if applicable)

---

## Troubleshooting

### Issue: HUD background still opaque

**Cause**: Alpha blending not applied or alpha value incorrect.

**Solution**: 
- Check `HUD_BG_ALPHA` is 0.65 (not 65)
- Verify `blend_colors()` function called in `hud_render_background()`
- Debug: Print blended color values to verify calculation

### Issue: Text unreadable with transparency

**Cause**: Alpha value too low (background too transparent).

**Solution**:
- Increase `HUD_BG_ALPHA` from 0.65 to 0.75 or 0.80
- Test with various scene backgrounds (bright/dark)

### Issue: Key guide doesn't appear

**Cause**: Initialization failed or render order incorrect.

**Solution**:
- Check `keyguide_init()` return value (should be 0)
- Verify `keyguide_render()` called after `hud_render()` in render loop
- Check `keyguide.visible` flag is 1

### Issue: Both HUDs don't toggle together

**Cause**: `hud_toggle()` not updated to sync key guide visibility.

**Solution**:
- Add `render->keyguide.visible = render->hud.visible;` to `hud_toggle()`

### Issue: Selection changes don't update HUD

**Cause**: `hud_mark_dirty()` not called in selection handlers.

**Solution**:
- Add `hud_mark_dirty(render);` after updating `render->selection`

### Issue: Build fails with linking errors

**Cause**: New source files not added to Makefile.

**Solution**:
- Add all `src/keyguide/*.c` and `src/hud/hud_transparency.c` to `SRC` variable

---

## Completion Criteria

✅ All user stories from spec.md pass acceptance scenarios  
✅ All functional requirements (FR-001 through FR-012) implemented  
✅ All success criteria (SC-001 through SC-006) validated  
✅ Constitution compliance verified (norminette, memory leaks, 42 constraints)  
✅ Code review passed  
✅ Documentation updated (comments, CHANGES.md)

---

## Next Steps

After completing implementation:
1. Run `/speckit.tasks` command to generate task breakdown
2. Create PR with detailed description linking to spec.md
3. Request code review
4. Update Korean documentation in `docs/specs/007-hud-improvements/`
5. Add release notes to CHANGES.md

---

## Time Estimates

| Phase | Task | Time |
|-------|------|------|
| 1 | Resolution update | 1-2h |
| 2 | HUD transparency | 2-3h |
| 3 | Key guide implementation | 2-3h |
| 4 | Selection updates | 0.5h |
| 5 | Makefile & testing | 0.5h |
| **Total** | | **6-9h** |

**Experienced developers**: 6 hours  
**New to codebase**: 9 hours  
**With interruptions**: Add 20% buffer (7-11h)
