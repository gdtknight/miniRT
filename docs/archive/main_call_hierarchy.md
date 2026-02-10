# main.c Call Hierarchy (miniRT)

This document summarizes the call hierarchy starting from `src/main.c`, and
lists each referenced function with its declaration header and implementation
source (where applicable). Static functions are noted explicitly.

## Call Hierarchy (from `main`)

```
main (src/main.c)
├─ parse_args [static] (src/main.c)
│  ├─ strcmp (libc)
│  └─ printf (libc)
├─ init_and_parse [static] (src/main.c)
│  ├─ scene_create (decl: includes/minirt.h; def: src/scene/scene.c)
│  ├─ error_print (decl: includes/error.h; def: src/utils/error.c)
│  ├─ parse_scene (decl: includes/parser.h; def: src/parser/parser.c)
│  │  ├─ validate_extension [static] (src/parser/parser.c)
│  │  ├─ read_line [static] (src/parser/parser.c)
│  │  │  ├─ read (libc)
│  │  │  ├─ malloc/free (libc)
│  │  │  └─ strcpy (libc)
│  │  ├─ parse_line [static] (src/parser/parser.c)
│  │  │  ├─ parse_ambient (decl: includes/parser.h; def: src/parser/parse_elements.c)
│  │  │  ├─ parse_camera (decl: includes/parser.h; def: src/parser/parse_elements.c)
│  │  │  ├─ parse_light (decl: includes/parser.h; def: src/parser/parse_elements.c)
│  │  │  ├─ parse_sphere (decl: includes/parser.h; def: src/parser/parse_objects.c)
│  │  │  ├─ parse_plane (decl: includes/parser.h; def: src/parser/parse_objects.c)
│  │  │  ├─ parse_cylinder (decl: includes/parser.h; def: src/parser/parse_cylinder.c)
│  │  │  └─ print_error (decl: includes/error.h; def: src/utils/error.c)
│  │  ├─ validate_scene (decl: includes/parser.h; def: src/parser/parser.c)
│  │  │  ├─ scene_has_ambient (decl: includes/minirt.h; def: src/scene/scene_flags.c)
│  │  │  ├─ scene_has_camera (decl: includes/minirt.h; def: src/scene/scene_flags.c)
│  │  │  ├─ scene_has_light (decl: includes/minirt.h; def: src/scene/scene_flags.c)
│  │  │  └─ print_error (decl: includes/error.h; def: src/utils/error.c)
│  │  ├─ open/close (libc)
│  │  └─ free (libc)
│  ├─ scene_destroy (decl: includes/minirt.h; def: src/scene/scene.c) [error path]
│  └─ scene_build_bvh (decl: includes/spatial.h; def: src/spatial/bvh_init.c)
├─ init_render_ctx [static] (src/main.c)
│  ├─ render_create (decl: includes/window.h; def: src/window/window_init.c)
│  │  ├─ mlx_context_init (decl: includes/mlx_context.h; def: src/window/mlx_context.c)
│  │  ├─ init_render_state [static] (src/window/window_init.c)
│  │  │  ├─ pixel_timing_init (decl: includes/pixel_timing.h; def: src/render/pixel_timing.c)
│  │  │  └─ debounce_init (decl: includes/render_debounce.h; def: src/render/render_debounce.c)
│  │  ├─ init_ui_components [static] (src/window/window_init.c)
│  │  │  ├─ hud_init (decl: includes/hud.h; def: src/hud/hud_init.c)
│  │  │  ├─ keyguide_init (decl: includes/keyguide.h; def: src/keyguide/keyguide_init.c)
│  │  │  └─ hud_calculate_total_pages (decl: includes/hud.h; def: src/hud/hud_init.c)
│  │  ├─ register_hooks [static] (src/window/window_init.c)
│  │  │  ├─ mlx_hook (MiniLibX)
│  │  │  │  ├─ close_window (decl: includes/window.h; def: src/window/window_lifecycle.c)
│  │  │  │  ├─ handle_key (decl: includes/window.h; def: src/window/window_events.c)
│  │  │  │  ├─ handle_key_release (decl: includes/window.h; def: src/window/window_events.c)
│  │  │  │  └─ handle_expose (decl: includes/window_internal.h; def: src/window/window_events.c)
│  │  │  └─ mlx_loop_hook (MiniLibX)
│  │  │     └─ render_loop (decl: includes/window.h; def: src/window/window_loop.c)
│  │  └─ malloc/free (libc)
│  ├─ error_print (decl: includes/error.h; def: src/utils/error.c)
│  └─ scene_destroy (decl: includes/minirt.h; def: src/scene/scene.c) [error path]
└─ mlx_loop (MiniLibX)
```

## Function Reference (Declaration / Definition)

### From `src/main.c`
- `int main(int argc, char **argv)`
  - Decl: implicit (C entry point)
  - Def: `src/main.c`
- `static int parse_args(int argc, char **argv, char **filename, int *bvh_vis)`
  - Decl/Def: `src/main.c` (static)
- `static int init_and_parse(char *filename, t_scene **scene)`
  - Decl/Def: `src/main.c` (static)
- `static int init_render_ctx(t_scene *scene, t_render **render, int bvh_vis)`
  - Decl/Def: `src/main.c` (static)

### Scene / Parser / BVH
- `t_scene *scene_create(void)`
  - Decl: `includes/minirt.h`
  - Def: `src/scene/scene.c`
- `void scene_destroy(t_scene *scene)`
  - Decl: `includes/minirt.h`
  - Def: `src/scene/scene.c`
- `void scene_build_bvh(t_scene *scene)`
  - Decl: `includes/spatial.h`
  - Def: `src/spatial/bvh_init.c`
- `int parse_scene(const char *filename, t_scene *scene)`
  - Decl: `includes/parser.h`
  - Def: `src/parser/parser.c`
- `int parse_ambient(char *line, t_scene *scene)`
  - Decl: `includes/parser.h`
  - Def: `src/parser/parse_elements.c`
- `int parse_camera(char *line, t_scene *scene)`
  - Decl: `includes/parser.h`
  - Def: `src/parser/parse_elements.c`
- `int parse_light(char *line, t_scene *scene)`
  - Decl: `includes/parser.h`
  - Def: `src/parser/parse_elements.c`
- `int parse_sphere(char *line, t_scene *scene)`
  - Decl: `includes/parser.h`
  - Def: `src/parser/parse_objects.c`
- `int parse_plane(char *line, t_scene *scene)`
  - Decl: `includes/parser.h`
  - Def: `src/parser/parse_objects.c`
- `int parse_cylinder(char *line, t_scene *scene)`
  - Decl: `includes/parser.h`
  - Def: `src/parser/parse_cylinder.c`
- `int validate_scene(t_scene *scene)`
  - Decl: `includes/parser.h`
  - Def: `src/parser/parser.c`
- `int scene_has_ambient(t_scene *scene)`
  - Decl: `includes/minirt.h`
  - Def: `src/scene/scene_flags.c`
- `int scene_has_camera(t_scene *scene)`
  - Decl: `includes/minirt.h`
  - Def: `src/scene/scene_flags.c`
- `int scene_has_light(t_scene *scene)`
  - Decl: `includes/minirt.h`
  - Def: `src/scene/scene_flags.c`
- `static char *read_line(int fd)`
  - Decl/Def: `src/parser/parser.c` (static)
- `static int parse_line(char *line, t_scene *scene)`
  - Decl/Def: `src/parser/parser.c` (static)
- `static int validate_extension(const char *filename)`
  - Decl/Def: `src/parser/parser.c` (static)

### Error Handling
- `int print_error(const char *message)`
  - Decl: `includes/error.h`
  - Def: `src/utils/error.c`
- `int error_print(t_error_code code)`
  - Decl: `includes/error.h`
  - Def: `src/utils/error.c`

### Rendering / Window
- `t_render *render_create(t_scene *scene)`
  - Decl: `includes/window.h`
  - Def: `src/window/window_init.c`
- `int mlx_context_init(t_mlx_context *ctx, int width, int height, char *title)`
  - Decl: `includes/mlx_context.h`
  - Def: `src/window/mlx_context.c`
- `void pixel_timing_init(t_pixel_timing *timing)`
  - Decl: `includes/pixel_timing.h`
  - Def: `src/render/pixel_timing.c`
- `void debounce_init(t_debounce_state *state)`
  - Decl: `includes/render_debounce.h`
  - Def: `src/render/render_debounce.c`
- `int hud_init(t_hud_state *hud, void *mlx, void *win)`
  - Decl: `includes/hud.h`
  - Def: `src/hud/hud_init.c`
- `int keyguide_init(t_keyguide_state *keyguide, void *mlx, void *win)`
  - Decl: `includes/keyguide.h`
  - Def: `src/keyguide/keyguide_init.c`
- `int hud_calculate_total_pages(t_scene *scene)`
  - Decl: `includes/hud.h`
  - Def: `src/hud/hud_init.c`
- `int close_window(void *param)`
  - Decl: `includes/window.h`
  - Def: `src/window/window_lifecycle.c`
- `int handle_key(int keycode, void *param)`
  - Decl: `includes/window.h`
  - Def: `src/window/window_events.c`
- `int handle_key_release(int keycode, void *param)`
  - Decl: `includes/window.h`
  - Def: `src/window/window_events.c`
- `int handle_expose(t_render *render)`
  - Decl: `includes/window_internal.h`
  - Def: `src/window/window_events.c`
- `int render_loop(void *param)`
  - Decl: `includes/window.h`
  - Def: `src/window/window_loop.c`

### External / Library Calls (not in project sources)
- `strcmp`, `printf`, `malloc`, `free`, `open`, `read`, `close` (libc)
- `mlx_hook`, `mlx_loop_hook`, `mlx_loop` (MiniLibX)

## Notes
- This hierarchy reflects direct calls found in `src/main.c` and the functions
  it invokes. Deeper rendering/math/physics chains are not expanded beyond the
  first level from `main` to keep this document focused.

---

## Expanded Call Hierarchy (Key Runtime Paths)

The sections below expand the call tree for the most important runtime paths:
render loop, input handling, and ray tracing. This is an additive view on top
of the base hierarchy above.

### Render Loop (from `mlx_loop_hook` → `render_loop`)

```
render_loop (decl: includes/window.h; def: src/window/window_loop.c)
├─ debounce_update (decl: includes/render_debounce.h; def: src/render/render_debounce.c)
├─ rebuild_bvh_if_dirty [static] (src/window/window_loop.c)
│  ├─ render_has_flag (decl: includes/window.h; def: src/window/render_flags_set.c)
│  ├─ scene_build_bvh (decl: includes/spatial.h; def: src/spatial/bvh_init.c)
│  └─ render_clear_flag (decl: includes/window.h; def: src/window/render_flags_set.c)
├─ execute_render_pass [static] (src/window/window_loop.c)
│  ├─ render_set_flag (decl: includes/window.h; def: src/window/render_flags_set.c)
│  ├─ metrics_start_frame (decl: includes/metrics.h; def: src/render/metrics_frame.c)
│  ├─ render_scene_to_buffer (decl: includes/minirt.h; def: src/render/render.c)
│  │  ├─ render_low_quality [static] (src/render/render.c)
│  │  │  ├─ create_camera_ray (decl: includes/minirt.h; def: src/render/camera.c)
│  │  │  ├─ trace_ray (decl: includes/minirt.h; def: src/render/trace.c)
│  │  │  └─ draw_pixel_block [static] (src/render/render.c)
│  │  └─ render_pixel [static] (src/render/render.c)
│  │     ├─ create_camera_ray (decl: includes/minirt.h; def: src/render/camera.c)
│  │     ├─ trace_ray (decl: includes/minirt.h; def: src/render/trace.c)
│  │     ├─ pixel_timing_add_sample (decl: includes/pixel_timing.h; def: src/render/pixel_timing.c)
│  │     └─ put_pixel_to_buffer [static] (src/render/render.c)
│  ├─ render_clear_flag (decl: includes/window.h; def: src/window/render_flags_set.c)
│  ├─ debounce_cancel (decl: includes/render_debounce.h; def: src/render/render_debounce.c)
│  ├─ metrics_end_frame (decl: includes/metrics.h; def: src/render/metrics_frame.c)
│  └─ mlx_put_image_to_window (MiniLibX)
├─ hud_render (decl: includes/hud.h; def: src/hud/hud_render.c)
└─ keyguide_render (decl: includes/keyguide.h; def: src/keyguide/keyguide_render.c)
```

### Input Handling (from `handle_key`)

```
handle_key (decl: includes/window.h; def: src/window/window_events.c)
├─ close_window (decl: includes/window.h; def: src/window/window_lifecycle.c)
├─ handle_hud_keys (decl: includes/window_internal.h; def: src/window/window_key_handlers.c)
│  ├─ hud_toggle (decl: includes/hud.h; def: src/hud/hud_toggle.c)
│  ├─ hud_select_next / hud_select_prev (decl: includes/hud.h; def: src/hud/hud_objects.c)
│  └─ hud_change_page (decl: includes/hud.h; def: src/hud/hud_objects.c)
├─ handle_object_selection (decl: includes/window_internal.h; def: src/window/window_selection.c)
│  ├─ hud_select_next / hud_select_prev (decl: includes/hud.h; def: src/hud/hud_objects.c)
│  └─ hud_mark_dirty (decl: includes/hud.h; def: src/hud/hud_objects.c)
├─ handle_camera_keys (decl: includes/window_internal.h; def: src/window/window_key_handlers.c)
│  ├─ handle_camera_move (decl: includes/window_internal.h; def: src/window/window_camera.c)
│  ├─ handle_camera_pitch (decl: includes/window_internal.h; def: src/window/window_camera.c)
│  ├─ handle_camera_reset (decl: includes/window_internal.h; def: src/window/window_camera.c)
│  ├─ debounce_on_input (decl: includes/render_debounce.h; def: src/render/render_debounce.c)
│  └─ hud_mark_dirty (decl: includes/hud.h; def: src/hud/hud_objects.c)
└─ handle_transform_keys (decl: includes/window_internal.h; def: src/window/window_key_handlers.c)
   ├─ handle_object_move (decl: includes/window_internal.h; def: src/window/window_objects.c)
   ├─ handle_light_move (decl: includes/window_internal.h; def: src/window/window_objects.c)
   ├─ handle_object_resize (decl: includes/window_internal.h; def: src/window/window_resize.c)
   ├─ handle_object_rotate (decl: includes/window_internal.h; def: src/window/window_rotate.c)
   ├─ debounce_on_input (decl: includes/render_debounce.h; def: src/render/render_debounce.c)
   └─ hud_mark_dirty (decl: includes/hud.h; def: src/hud/hud_objects.c)
```

### Ray Tracing Core (from `trace_ray`)

```
trace_ray (decl: includes/minirt.h; def: src/render/trace.c)
├─ metrics_add_ray (decl: includes/metrics.h; def: src/render/metrics_counters.c)
├─ bvh_intersect (decl: includes/spatial.h; def: src/spatial/bvh_traverse.c)
│  └─ (BVH traversal / node intersection helpers in src/spatial/*)
├─ check_all_objects [static] (src/render/trace.c)
│  └─ intersect_object_new (decl: includes/ray.h; def: src/ray/intersect_object.c)
└─ apply_lighting (decl: includes/minirt.h; def: src/lighting/lighting.c)
   ├─ shadow_test / shadow_calc (decl: includes/shadow.h; def: src/lighting/shadow_*.c)
   └─ (lighting helpers in src/lighting/*)
```

### HUD Rendering (from `hud_render`)

```
hud_render (decl: includes/hud.h; def: src/hud/hud_render.c)
├─ hud_render_content (decl: includes/hud.h; def: src/hud/hud_render.c)
│  ├─ hud_render_camera (decl: includes/hud.h; def: src/hud/hud_scene.c)
│  ├─ render_camera_fov (decl: includes/hud.h; def: src/hud/hud_scene.c)
│  ├─ hud_render_ambient (decl: includes/hud.h; def: src/hud/hud_scene.c)
│  ├─ hud_render_light (decl: includes/hud.h; def: src/hud/hud_scene.c)
│  ├─ render_light_bright (decl: includes/hud.h; def: src/hud/hud_scene.c)
│  ├─ hud_render_objects (decl: includes/hud.h; def: src/hud/hud_objects.c)
│  └─ hud_render_performance (decl: includes/hud.h; def: src/hud/hud_performance.c)
└─ (sets render->hud.dirty = 0)
```

### Keyguide Rendering (from `keyguide_render`)

```
keyguide_render (decl: includes/keyguide.h; def: src/keyguide/keyguide_render.c)
├─ keyguide_render_content (decl: includes/keyguide.h; def: src/keyguide/keyguide_render.c)
│  └─ keyguide_render_camera_section [static] (src/keyguide/keyguide_render.c)
├─ keyguide_render_content2 (decl: includes/keyguide.h; def: src/keyguide/keyguide_render.c)
└─ keyguide_render_extra (decl: includes/keyguide.h; def: src/keyguide/keyguide_render_extra.c)
   ├─ keyguide_render_resize [static] (src/keyguide/keyguide_render_extra.c)
   └─ keyguide_render_rotation [static] (src/keyguide/keyguide_render_extra.c)
```

### HUD Text & Formatting Helpers (deeper path)

```
hud_render_content (decl: includes/hud.h; def: src/hud/hud_render.c)
├─ hud_render_camera (decl: includes/hud.h; def: src/hud/hud_scene.c)
│  ├─ mlx_string_put (MiniLibX)
│  └─ format_and_print_vec3 (decl: includes/hud_text.h; def: src/hud/hud_text.c)
│     ├─ hud_format_vec3 (decl: includes/hud.h; def: src/hud/hud_format.c)
│     └─ mlx_string_put (MiniLibX)
├─ render_camera_fov (decl: includes/hud.h; def: src/hud/hud_scene.c)
│  ├─ hud_format_float (decl: includes/hud.h; def: src/hud/hud_format.c)
│  └─ mlx_string_put (MiniLibX)
├─ hud_render_ambient (decl: includes/hud.h; def: src/hud/hud_scene.c)
│  ├─ hud_format_float (decl: includes/hud.h; def: src/hud/hud_format.c)
│  ├─ hud_format_color (decl: includes/hud.h; def: src/hud/hud_format.c)
│  └─ mlx_string_put (MiniLibX)
├─ hud_render_light (decl: includes/hud.h; def: src/hud/hud_scene.c)
│  ├─ hud_format_vec3 (decl: includes/hud.h; def: src/hud/hud_format.c)
│  └─ mlx_string_put (MiniLibX)
├─ render_light_bright (decl: includes/hud.h; def: src/hud/hud_scene.c)
│  ├─ hud_format_float (decl: includes/hud.h; def: src/hud/hud_format.c)
│  ├─ hud_format_color (decl: includes/hud.h; def: src/hud/hud_format.c)
│  └─ mlx_string_put (MiniLibX)
├─ hud_render_objects (decl: includes/hud.h; def: src/hud/hud_objects.c)
│  ├─ hud_calculate_total_pages (decl: includes/hud.h; def: src/hud/hud_init.c)
│  ├─ render_object_entry (decl: includes/hud_obj_render.h; def: src/hud/hud_obj_render.c)
│  └─ hud_get_selection_from_global (decl: includes/hud.h; def: src/hud/hud_objects.c)
└─ hud_render_performance (decl: includes/hud.h; def: src/hud/hud_performance.c)
   ├─ hud_format_fps (decl: includes/hud.h; def: src/hud/hud_format_simple.c)
   ├─ hud_format_time_ms (decl: includes/hud.h; def: src/hud/hud_format_simple.c)
   ├─ hud_format_bvh_status (decl: includes/hud.h; def: src/hud/hud_format_simple.c)
   └─ mlx_string_put (MiniLibX)
```
