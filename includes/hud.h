/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUD_H
# define HUD_H

# include "minirt.h"
# include "window.h"

/* HUD display dimensions and layout constants */
# define HUD_WIDTH 400
# define HUD_HEIGHT 600
# define HUD_MARGIN_X 10
# define HUD_MARGIN_Y 10
# define HUD_LINE_HEIGHT 18
# define HUD_OBJECTS_PER_PAGE 8

/* HUD color scheme (0xRRGGBB format) */
# define HUD_COLOR_TEXT 0xFFFFFF
# define HUD_COLOR_HIGHLIGHT 0x00FF00
# define HUD_COLOR_BG 0x000000
# define HUD_BG_ALPHA 0.65

/* Alpha blending utilities */
int		blend_colors(int bg_color, int fg_color, double alpha);
int		get_red(int color);
int		get_green(int color);
int		get_blue(int color);
int		make_color(int r, int g, int b);
int		get_pixel(char *img_data, int x, int y, int size_line, int bpp);
void	set_pixel(char *img_data, int x, int y, int color,
			int size_line, int bpp);

/* ========================================================================== */
/*                            PUBLIC API FUNCTIONS                            */
/* ========================================================================== */

/*
** Initialize HUD state and create background overlay image.
** 
** Parameters:
**   hud: Pointer to HUD state structure to initialize
**   mlx: MiniLibX connection pointer
**   win: Window pointer for rendering context
**
** Returns:
**   0 on success, -1 on failure (image creation failed)
**
** Side Effects:
**   - Allocates background image buffer via mlx_new_image
**   - Sets default values: visible=1, current_page=0, dirty=1
**   - Pre-fills background buffer with semi-transparent dark overlay
**
** Memory:
**   Background image must be freed later with hud_cleanup()
*/
int		hud_init(t_hud_state *hud, void *mlx, void *win);

/*
** Clean up HUD resources and free background image.
**
** Parameters:
**   hud: Pointer to HUD state structure
**   mlx: MiniLibX connection pointer
**
** Returns:
**   void
**
** Side Effects:
**   - Destroys background image buffer via mlx_destroy_image
**   - Sets bg_img and bg_data to NULL
**
** Safety:
**   Safe to call even if hud_init failed (checks for NULL pointers)
*/
void	hud_cleanup(t_hud_state *hud, void *mlx);

/*
** Render HUD overlay on top of rendered scene.
**
** Parameters:
**   render: Render context containing scene data and HUD state
**
** Returns:
**   void
**
** Behavior:
**   - Returns immediately if hud.visible == 0 (HUD hidden)
**   - Renders semi-transparent background rectangle
**   - Renders camera, ambient light, light source information
**   - Renders object list for current page with pagination
**   - Highlights selected object if selection is active
**   - Clears hud.dirty flag after rendering
**
** Performance:
**   - Target: ≤2ms render time
**   - Optimized via dirty flag (skips if no changes)
**   - Limited mlx_string_put calls (≤25 per frame)
**
** 42 Norm:
**   Function delegates to internal helpers to stay under 25 lines
*/
void	hud_render(t_render *render);

/*
** Toggle HUD visibility on/off.
**
** Parameters:
**   render: Render context containing HUD state
**
** Returns:
**   void
**
** Side Effects:
**   - Flips render->hud.visible (0 <-> 1)
**   - Logs toggle event at INFO level (OBS-002)
**
** Key Binding:
**   Typically called from handle_key() on 'H' key press
*/
void	hud_toggle(t_render *render);

/*
** Select next object in scene (Tab key navigation).
**
** Parameters:
**   render: Render context with selection and scene data
**
** Returns:
**   void
**
** Behavior:
**   - Calculates total objects (spheres + planes + cylinders)
**   - Converts current selection to global index
**   - Increments index with wraparound
**   - Updates render->selection with new (type, index)
**   - Sets render->hud.dirty = 1 to trigger redraw
**
** Edge Cases:
**   - If total_objects == 0, does nothing
**   - Wraps from last cylinder back to first sphere
**
** Key Binding:
**   Called from handle_key() on Tab key press
*/
void	hud_select_next(t_render *render);

/*
** Select previous object in scene (Shift+Tab key navigation).
**
** Parameters:
**   render: Render context with selection and scene data
**
** Returns:
**   void
**
** Behavior:
**   - Same as hud_select_next but decrements index
**   - Handles wraparound in reverse (first sphere -> last cylinder)
**
** Key Binding:
**   Called from handle_key() on Shift+Tab key press
*/
void	hud_select_prev(t_render *render);

/*
** Navigate to next page of objects (Down arrow or PgDown).
**
** Parameters:
**   render: Render context with HUD pagination state
**
** Returns:
**   void
**
** Behavior:
**   - Increments render->hud.current_page
**   - Wraps to 0 if exceeds total_pages
**   - Sets render->hud.dirty = 1
**
** Edge Cases:
**   - If total_pages <= 1, does nothing
**
** Key Binding:
**   Called from handle_key() on Down arrow key
*/
void	hud_page_down(t_render *render);

/*
** Navigate to previous page of objects (Up arrow or PgUp).
**
** Parameters:
**   render: Render context with HUD pagination state
**
** Returns:
**   void
**
** Behavior:
**   - Decrements render->hud.current_page
**   - Wraps to (total_pages - 1) if goes below 0
**   - Sets render->hud.dirty = 1
**
** Key Binding:
**   Called from handle_key() on Up arrow key
*/
void	hud_page_up(t_render *render);

/*
** Mark HUD as dirty to force redraw on next render loop.
**
** Parameters:
**   render: Render context with HUD state
**
** Returns:
**   void
**
** Use Cases:
**   - Camera position/direction changed (real-time updates)
**   - Object properties modified
**   - Any scene data displayed in HUD changed
*/
void	hud_mark_dirty(t_render *render);

/* ========================================================================== */
/*                          INTERNAL HELPER FUNCTIONS                         */
/*                      (Not part of public API contract)                     */
/* ========================================================================== */

/*
** Convert selection (type, index) to global linear index.
** Used internally for Tab navigation logic.
*/
int		hud_get_global_index(t_selection sel, t_scene *scene);

/*
** Convert global linear index to selection (type, index).
** Used internally for Tab navigation logic.
*/
t_selection	hud_get_selection_from_global(int global_idx, t_scene *scene);

/*
** Format floating point number to string with 2 decimal places.
** Example: 123.456 -> "123.46"
** Buffer must be at least 32 bytes.
*/
void	hud_format_float(char *buffer, double value);

/*
** Format vec3 to string "(x, y, z)" with 2 decimal places.
** Example: {1.5, 2.3, 3.7} -> "(1.50, 2.30, 3.70)"
** Buffer must be at least 64 bytes.
*/
void	hud_format_vec3(char *buffer, t_vec3 vec);

/*
** Format color to string "R:255 G:128 B:64".
** Buffer must be at least 32 bytes.
*/
void	hud_format_color(char *buffer, t_color color);

/*
** Format FPS value to string with 1 decimal place.
** Buffer must be at least 16 bytes.
*/
void	hud_format_fps(char *buf, double fps);

/*
** Format time in microseconds to milliseconds with 1 decimal.
** Buffer must be at least 16 bytes.
*/
void	hud_format_time_ms(char *buf, long time_us);

/*
** Format BVH enabled status to "ON" or "OFF".
** Buffer must be at least 4 bytes.
*/
void	hud_format_bvh_status(char *buf, int enabled);

/*
** Render performance metrics section in HUD.
** Shows FPS, frame time, and BVH status.
** Called internally by hud_render_content().
*/
void	hud_render_performance(t_render *render, int *y);

/*
** Render HUD background overlay (semi-transparent dark rectangle).
** Called internally by hud_render().
*/
void	hud_render_background(t_render *render);

/*
** Render HUD text content (camera, lights, objects).
** Called internally by hud_render().
*/
void	hud_render_content(t_render *render);

/*
** Render camera information section.
** Called internally by hud_render_content().
*/
void	hud_render_camera(t_render *render, int *y);

/*
** Render ambient light information section.
** Called internally by hud_render_content().
*/
void	hud_render_ambient(t_render *render, int *y);

/*
** Render light source information section.
** Called internally by hud_render_content().
*/
void	hud_render_light(t_render *render, int *y);

/*
** Render object list with pagination.
** Called internally by hud_render_content().
*/
void	hud_render_objects(t_render *render, int *y);

/*
** Calculate total number of pages based on object count.
** Called during init and when scene changes.
*/
int		hud_calculate_total_pages(t_scene *scene);

/*
** Create and fill background overlay image buffer.
** Called during hud_init().
*/
int		hud_create_background(t_hud_state *hud, void *mlx);

#endif
