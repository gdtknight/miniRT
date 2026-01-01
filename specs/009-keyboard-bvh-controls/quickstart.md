# Quick Start Guide: Keyboard Controls Implementation

**Feature**: Keyboard Controls Finalization and BVH Enforcement  
**Target Audience**: Developers implementing this feature  
**Estimated Time**: 2-4 hours for implementation + testing

## Overview

This guide helps developers implement the new keyboard control scheme and permanently enforce BVH acceleration in miniRT. The changes are focused on three main areas:

1. **Keyboard Mapping Updates** - Replace Numpad controls with R/T/F/G/V/B layout
2. **Camera Control Extensions** - Add X/E/C/Z keys for complete camera control
3. **BVH Enforcement** - Remove conditional BVH logic to ensure always-on acceleration

## Prerequisites

- Existing miniRT codebase with BVH already implemented
- Working knowledge of C and MLX library
- Understanding of ray tracing and spatial acceleration structures
- Access to both macOS and Linux for cross-platform testing

## Implementation Steps

### Step 1: Update Key Code Definitions (15 minutes)

**File**: `includes/window.h` or `src/window/window.c` (depending on current location)

**Action**: Add new key code definitions following the existing platform-specific pattern.

**New macOS Key Codes to Add**:
```c
#ifdef __APPLE__
# define KEY_T 17          /* Object move X+ (right) */
# define KEY_G 5           /* Object move Y+ (up) */
# define KEY_V 9           /* Object move Z- (backward) */
# define KEY_E 14          /* Camera pitch up */
# define KEY_C 8           /* Camera pitch down */
# define KEY_X 7           /* Camera backward */
# define KEY_Z 6           /* Camera down (world Y-) */
#endif
```

**New Linux Key Codes to Add**:
```c
#elif defined(__linux__)
# define KEY_T 116         /* Object move X+ (right) */
# define KEY_G 103         /* Object move Y+ (up) */
# define KEY_V 118         /* Object move Z- (backward) */
# define KEY_E 101         /* Camera pitch up */
# define KEY_C 99          /* Camera pitch down */
# define KEY_X 120         /* Camera backward */
# define KEY_Z 122         /* Camera down (world Y-) */
#endif
```

**Verification**: Compile with `make` - should succeed with no errors.

### Step 2: Update Camera Movement Handler (20 minutes)

**File**: `src/window/window.c`

**Current Function**: `handle_camera_move()` (handles W/A/S/D)

**Changes Required**:
1. Add X key for backward movement
2. Add Q key for upward world movement (already may exist)
3. Add Z key for downward world movement

**Example Implementation**:
```c
static void	handle_camera_move(t_render *render, int keycode)
{
	t_vec3	right;
	t_vec3	move;
	double	step;

	step = 1.0;
	right = vec3_normalize(vec3_cross(render->scene->camera.direction,
			(t_vec3){0, 1, 0}));
	if (keycode == KEY_W)
		move = vec3_multiply(render->scene->camera.direction, step);
	else if (keycode == KEY_X)  /* NEW: backward movement */
		move = vec3_multiply(render->scene->camera.direction, -step);
	else if (keycode == KEY_A)
		move = vec3_multiply(right, -step);
	else if (keycode == KEY_D)
		move = vec3_multiply(right, step);
	else if (keycode == KEY_Q)  /* NEW: world up */
		move = (t_vec3){0, step, 0};
	else if (keycode == KEY_Z)  /* NEW: world down */
		move = (t_vec3){0, -step, 0};
	else
		return ;
	render->scene->camera.position = vec3_add(render->scene->camera.position,
			move);
}
```

**Verification**: Test camera movement with W/X/A/D/Q/Z keys in running application.

### Step 3: Update Camera Rotation Handler (20 minutes)

**File**: `src/window/window.c`

**Current Function**: `handle_camera_pitch()` (currently uses R/F keys)

**Changes Required**:
1. Change R key to E key (pitch up)
2. Change F key to C key (pitch down)
3. Update condition checks

**Example Changes**:
```c
static void	handle_camera_pitch(t_render *render, int keycode)
{
	/* ... existing variable declarations ... */
	
	angle = 5.0 * M_PI / 180.0;
	if (keycode == KEY_C)  /* CHANGED: was KEY_F */
		angle = -angle;
	else if (keycode != KEY_E)  /* CHANGED: was KEY_R */
		return ;
	
	/* ... rest of rotation logic unchanged ... */
}
```

**Verification**: Test camera pitch with E/C keys.

### Step 4: Add Camera Reset Handler (15 minutes)

**File**: `src/window/window.c`

**Action**: Create new function to reset camera to initial state.

**New Function**:
```c
static void	handle_camera_reset(t_render *render)
{
	render->scene->camera.position = render->scene->camera.initial_position;
	render->scene->camera.direction = render->scene->camera.initial_direction;
}
```

**Note**: Requires storing initial camera state when scene is loaded. Add to scene initialization:
```c
/* In scene parsing code, after camera setup: */
scene->camera.initial_position = scene->camera.position;
scene->camera.initial_direction = scene->camera.direction;
```

**Update t_camera structure** in `includes/minirt.h` or wherever defined:
```c
typedef struct s_camera
{
	t_vec3	position;
	t_vec3	direction;
	t_vec3	initial_position;    /* NEW */
	t_vec3	initial_direction;   /* NEW */
	double	fov;
}	t_camera;
```

**Verification**: Press S key to verify camera returns to starting position.

### Step 5: Update Object Movement Handler (30 minutes)

**File**: `src/window/window.c`

**Current Function**: `handle_object_move()` (uses KEY_KP_* codes)

**Changes Required**: Replace Numpad keys with R/T/F/G/V/B keys.

**Complete Replacement**:
```c
static void	handle_object_move(t_render *render, int keycode)
{
	t_vec3	move;
	double	step;

	step = 1.0;
	move = (t_vec3){0, 0, 0};
	if (keycode == KEY_R)         /* NEW: X- (left) */
		move.x = -step;
	else if (keycode == KEY_T)    /* NEW: X+ (right) */
		move.x = step;
	else if (keycode == KEY_F)    /* NEW: Y- (down) */
		move.y = -step;
	else if (keycode == KEY_G)    /* NEW: Y+ (up) */
		move.y = step;
	else if (keycode == KEY_V)    /* NEW: Z- (backward) */
		move.z = -step;
	else if (keycode == KEY_B)    /* NEW: Z+ (forward) */
		move.z = step;
	else
		return ;
	move_selected_object(render, move);
}
```

**Verification**: Select an object with bracket keys, then move with R/T/F/G/V/B.

### Step 6: Update Main Key Handler Dispatcher (20 minutes)

**File**: `src/window/window.c`

**Function**: `handle_key()` (main keyboard event dispatcher)

**Changes Required**:
1. Update camera movement condition to include X/Q/Z
2. Update camera rotation condition to use E/C instead of R/F
3. Add S key for camera reset
4. Update object movement condition to use R/T/F/G/V/B instead of Numpad keys

**Example Updates**:
```c
int	handle_key(int keycode, void *param)
{
	t_render	*render;

	render = (t_render *)param;
	if (keycode == KEY_ESC)
		close_window(param);
	else if (keycode == KEY_H)
		hud_toggle(render);
	/* ... existing TAB/Arrow key handling ... */
	else if (keycode == KEY_W || keycode == KEY_X || keycode == KEY_A 
		|| keycode == KEY_D || keycode == KEY_Q || keycode == KEY_Z)  /* UPDATED */
	{
		handle_camera_move(render, keycode);
		render->low_quality = 1;
		render->dirty = 1;
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_E || keycode == KEY_C)  /* CHANGED: was KEY_R || KEY_F */
	{
		handle_camera_pitch(render, keycode);
		render->low_quality = 1;
		render->dirty = 1;
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_S)  /* NEW: camera reset */
	{
		handle_camera_reset(render);
		render->dirty = 1;
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_R || keycode == KEY_T || keycode == KEY_F
		|| keycode == KEY_G || keycode == KEY_V || keycode == KEY_B)  /* CHANGED */
	{
		handle_object_move(render, keycode);
		render->low_quality = 1;
		render->dirty = 1;
		hud_mark_dirty(render);
	}
	/* ... rest of handler ... */
	return (0);
}
```

**Verification**: Test all key combinations to ensure correct routing.

### Step 7: Enforce BVH Permanently (10 minutes)

**File**: `src/main.c`

**Current Code** (example):
```c
scene->render_state.bvh_enabled = 1;  /* Already set to 1 */
/* ... */
scene_build_bvh(scene);
```

**Changes Required**:
1. Remove any conditional BVH initialization
2. Ensure BVH always built regardless of flags
3. Remove any runtime toggle functionality (if exists)

**Updated Code**:
```c
/* BVH is permanently enabled - no configuration needed */
scene_build_bvh(scene);
if (scene->bvh == NULL || scene->bvh->root == NULL)
{
	ft_putendl_fd("Error: Failed to build BVH acceleration structure", 2);
	/* Handle error appropriately */
}
```

**Optional**: Remove `bvh_enabled` field from structure if no longer needed, or document it as "always 1".

**Verification**: Verify BVH is built for all scenes by checking performance metrics.

### Step 8: Update Key Guide Display (30 minutes)

**File**: `src/keyguide/keyguide_render.c`

**Action**: Update the in-game key guide text to reflect new keyboard layout.

**Changes Required**:
1. Replace Numpad references (4/6/2/8/1/3) with R/T/F/G/V/B
2. Add new camera keys (X/E/C/Z)
3. Update S key description to "Reset Camera"

**Example String Updates**:
```c
/* Object Movement Section */
mlx_string_put(mlx, win, x, y, COLOR, "Object Movement:");
y += LINE_HEIGHT;
mlx_string_put(mlx, win, x, y, COLOR, "  R/T: X-axis (left/right)");
y += LINE_HEIGHT;
mlx_string_put(mlx, win, x, y, COLOR, "  F/G: Y-axis (down/up)");
y += LINE_HEIGHT;
mlx_string_put(mlx, win, x, y, COLOR, "  V/B: Z-axis (back/forward)");
y += LINE_HEIGHT + SECTION_GAP;

/* Camera Section */
mlx_string_put(mlx, win, x, y, COLOR, "Camera:");
y += LINE_HEIGHT;
mlx_string_put(mlx, win, x, y, COLOR, "  W/X: Forward/Backward");
y += LINE_HEIGHT;
mlx_string_put(mlx, win, x, y, COLOR, "  A/D: Left/Right");
y += LINE_HEIGHT;
mlx_string_put(mlx, win, x, y, COLOR, "  Q/Z: Up/Down (world)");
y += LINE_HEIGHT;
mlx_string_put(mlx, win, x, y, COLOR, "  E/C: Pitch Up/Down");
y += LINE_HEIGHT;
mlx_string_put(mlx, win, x, y, COLOR, "  S: Reset Camera");
```

**Verification**: Press H key to toggle key guide and verify displayed text.

### Step 9: Update Documentation (20 minutes)

**Files to Update**:
1. `README.md` - Update controls section if present
2. Create `docs/CONTROLS.md` - Comprehensive keyboard reference

**docs/CONTROLS.md Template**:
```markdown
# miniRT Keyboard Controls

## Camera Controls

### Translation (Movement)
- **W**: Move forward (along view direction)
- **X**: Move backward (along view direction)
- **A**: Move left (perpendicular to view)
- **D**: Move right (perpendicular to view)
- **Q**: Move up (world space Y+)
- **Z**: Move down (world space Y-)

### Rotation
- **E**: Pitch up (look upward)
- **C**: Pitch down (look downward)

### Reset
- **S**: Reset camera to initial position and orientation

## Object Manipulation

First select an object using `[` (previous) or `]` (next), then:

- **R**: Move left (X-)
- **T**: Move right (X+)
- **F**: Move down (Y-)
- **G**: Move up (Y+)
- **V**: Move backward (Z-)
- **B**: Move forward (Z+)

## UI Controls

- **H**: Toggle HUD visibility
- **TAB**: Cycle object selection forward
- **Shift+TAB**: Cycle object selection backward
- **↑/↓**: Navigate HUD pages
- **ESC**: Exit application

## Performance

BVH acceleration is permanently enabled for optimal rendering performance with complex scenes.
```

**Verification**: Review documentation for accuracy and completeness.

### Step 10: Testing Checklist (30 minutes)

**Manual Test Scenarios**:

1. **Camera Translation** (all 6 directions):
   - [ ] W moves camera forward
   - [ ] X moves camera backward
   - [ ] A moves camera left
   - [ ] D moves camera right
   - [ ] Q moves camera up in world space
   - [ ] Z moves camera down in world space

2. **Camera Rotation**:
   - [ ] E pitches camera upward
   - [ ] C pitches camera downward
   - [ ] Direction vector remains normalized

3. **Camera Reset**:
   - [ ] S returns camera to initial position
   - [ ] Camera direction also reset

4. **Object Movement**:
   - [ ] Select object with bracket keys
   - [ ] R moves object left (X-)
   - [ ] T moves object right (X+)
   - [ ] F moves object down (Y-)
   - [ ] G moves object up (Y+)
   - [ ] V moves object backward (Z-)
   - [ ] B moves object forward (Z+)

5. **BVH Performance**:
   - [ ] Scene with 100+ objects renders smoothly
   - [ ] BVH metrics visible in HUD (if applicable)
   - [ ] No way to disable BVH

6. **Cross-Platform**:
   - [ ] All keys work correctly on macOS
   - [ ] All keys work correctly on Linux

7. **Key Guide**:
   - [ ] H toggles key guide visibility
   - [ ] Key guide shows updated controls
   - [ ] No references to old Numpad controls

8. **Code Quality**:
   - [ ] `make` succeeds with no warnings
   - [ ] `norminette` passes all checks
   - [ ] `valgrind` shows zero memory leaks

**Test Scenes**:
- `scenes/simple.rt` - Basic functionality
- `scenes/complex.rt` - Performance with many objects

## Common Issues and Solutions

### Issue: Key not responding
**Solution**: Check platform-specific key code is correct. Use `printf("%d\n", keycode);` in handler to debug.

### Issue: Camera rotation broken
**Solution**: Ensure direction vector is normalized after rotation calculation.

### Issue: Object not moving
**Solution**: Verify an object is selected (check `render->selection.type != OBJ_NONE`).

### Issue: Compilation errors
**Solution**: Ensure all new key codes defined for both macOS and Linux platforms.

### Issue: Norminette violations
**Solution**: Check line length (<80 chars), function length (<25 lines), proper formatting.

## Performance Expectations

- **With BVH enabled**: 15+ FPS with 100 objects, sub-second renders with 1000 objects
- **Response time**: <100ms from key press to visual feedback
- **Memory**: No significant increase from previous implementation

## Next Steps After Implementation

1. Create Korean translation of documentation in `docs/specs/009-keyboard-bvh-controls/spec.md`
2. Update GitHub Wiki with new keyboard controls
3. Run full regression test suite
4. Create pull request with detailed description of changes
5. Request code review focusing on cross-platform compatibility

## Additional Resources

- **Contract Reference**: `specs/009-keyboard-bvh-controls/contracts/keyboard_layout.h`
- **Data Model**: `specs/009-keyboard-bvh-controls/data-model.md`
- **Research Decisions**: `specs/009-keyboard-bvh-controls/research.md`
- **42 School Constitution**: `.specify/memory/constitution.md`
