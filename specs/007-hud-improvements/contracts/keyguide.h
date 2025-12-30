/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyguide.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/30 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KEYGUIDE_H
# define KEYGUIDE_H

# include "minirt.h"
# include "window.h"

/*
** ============================================================================
**                         KEY GUIDE HUD API CONTRACT
** ============================================================================
**
** Purpose: Displays compact keyboard control reference in upper-right corner
** Dependencies: MiniLibX for rendering, window.h for window dimensions
** Constraints: 42 School function restrictions (no pthread/fork)
**
** Lifecycle:
**   1. keyguide_init()    - Allocate resources during window initialization
**   2. keyguide_render()  - Draw overlay each frame when visible
**   3. keyguide_cleanup() - Free resources during window cleanup
**
** Design Decisions:
**   - Fixed 300x400px size (non-scalable, per requirements)
**   - Upper-right placement to avoid main HUD collision
**   - Semi-transparent background (alpha=0.65) matches main HUD
**   - Visibility synced with main HUD via 'H' key toggle
**   - Shows only essential controls (not comprehensive)
*/

/* ========================================================================== */
/*                           LAYOUT CONSTANTS                                 */
/* ========================================================================== */

/* Key guide dimensions (maximum allowed per FR-008) */
# define KEYGUIDE_WIDTH 300
# define KEYGUIDE_HEIGHT 400

/* Positioning margins from window edges */
# define KEYGUIDE_MARGIN_X 20
# define KEYGUIDE_MARGIN_Y 20

/* Text layout spacing */
# define KEYGUIDE_LINE_HEIGHT 16
# define KEYGUIDE_SECTION_GAP 8

/* ========================================================================== */
/*                            COLOR SCHEME                                    */
/* ========================================================================== */

/* Text colors (0xRRGGBB format) */
# define KEYGUIDE_COLOR_TEXT 0xFFFFFF       /* White - body text */
# define KEYGUIDE_COLOR_HEADING 0x00FFFF    /* Cyan - section headers */
# define KEYGUIDE_COLOR_BG 0x000000         /* Black - background */

/* Transparency level (0.0=transparent, 1.0=opaque) */
# define KEYGUIDE_BG_ALPHA 0.65

/* ========================================================================== */
/*                           DATA STRUCTURES                                  */
/* ========================================================================== */

/*
** Key guide state structure.
** 
** Manages lifecycle, visibility, and rendering resources for key guide overlay.
**
** Fields:
**   bg_img     - MiniLibX image handle for pre-rendered background
**   bg_data    - Direct pixel buffer pointer (from mlx_get_data_addr)
**   bpp        - Bits per pixel (typically 32 for ARGB)
**   size_line  - Bytes per scanline in image buffer
**   endian     - Endianness flag (little/big endian)
**   visible    - Visibility state: 1=shown, 0=hidden
**   x, y       - Screen coordinates of upper-left corner
**
** Memory:
**   - bg_img allocated via mlx_new_image (heap)
**   - Must be freed with mlx_destroy_image in keyguide_cleanup()
**   - bg_data points into bg_img (do not free separately)
*/
typedef struct s_keyguide_state
{
	void	*bg_img;
	char	*bg_data;
	int		bpp;
	int		size_line;
	int		endian;
	int		visible;
	int		x;
	int		y;
}	t_keyguide_state;

/* ========================================================================== */
/*                          PUBLIC API FUNCTIONS                              */
/* ========================================================================== */

/*
** Initialize key guide state and create background overlay.
**
** Parameters:
**   keyguide - Pointer to key guide state structure to initialize
**   mlx      - MiniLibX connection pointer
**   win      - Window pointer for rendering context
**
** Returns:
**   0 on success, -1 on failure (image creation failed)
**
** Side Effects:
**   - Allocates background image buffer via mlx_new_image
**   - Sets position: x = WINDOW_WIDTH - KEYGUIDE_WIDTH - MARGIN_X
**                    y = KEYGUIDE_MARGIN_Y
**   - Sets default values: visible=1
**   - Pre-fills background buffer with semi-transparent dark overlay
**
** Memory:
**   Background image must be freed later with keyguide_cleanup()
**
** Error Handling:
**   Returns -1 if mlx_new_image fails (out of memory or invalid parameters)
**
** Example:
**   t_keyguide_state guide;
**   if (keyguide_init(&guide, mlx, win) != 0)
**       return (error("Failed to initialize key guide"));
*/
int		keyguide_init(t_keyguide_state *keyguide, void *mlx, void *win);

/*
** Clean up key guide resources and free background image.
**
** Parameters:
**   keyguide - Pointer to key guide state structure
**   mlx      - MiniLibX connection pointer
**
** Returns:
**   void
**
** Side Effects:
**   - Destroys background image buffer via mlx_destroy_image
**   - Sets bg_img and bg_data to NULL
**
** Safety:
**   Safe to call even if keyguide_init failed (checks for NULL pointers)
**
** Example:
**   keyguide_cleanup(&render->keyguide, render->mlx);
*/
void	keyguide_cleanup(t_keyguide_state *keyguide, void *mlx);

/*
** Render key guide overlay on top of scene and main HUD.
**
** Parameters:
**   render - Render context containing scene data and key guide state
**
** Returns:
**   void
**
** Behavior:
**   - Returns immediately if keyguide.visible == 0 (hidden)
**   - Renders semi-transparent background rectangle
**   - Renders control reference text in functional groups:
**     * Navigation: ESC, H
**     * Camera: WASD, R/F
**     * Objects: TAB, [], Numpad 1-8
**     * Rendering: B, Q, I
**   - Section headers in cyan, controls in white
**
** Performance:
**   - Target: ≤1ms render time
**   - Limited mlx_string_put calls (≤15 per frame)
**
** Rendering Order:
**   1. Scene rendering (background)
**   2. Main HUD rendering (left side)
**   3. Key guide rendering (upper-right) ← This function
**
** 42 Norm:
**   Function delegates to internal helpers to stay under 25 lines
**
** Example:
**   keyguide_render(render);  // Called once per frame in render loop
*/
void	keyguide_render(t_render *render);

/* ========================================================================== */
/*                      INTERNAL HELPER FUNCTIONS                             */
/*                   (Not part of public API contract)                        */
/* ========================================================================== */

/*
** Create and fill background overlay image buffer with alpha blending.
** Called internally during keyguide_init().
**
** Parameters:
**   keyguide - Key guide state structure
**   mlx      - MiniLibX connection pointer
**
** Returns:
**   0 on success, -1 on failure
*/
int		keyguide_create_background(t_keyguide_state *keyguide, void *mlx);

/*
** Render semi-transparent background rectangle.
** Called internally by keyguide_render().
**
** Parameters:
**   render - Render context with MiniLibX handles
*/
void	keyguide_render_background(t_render *render);

/*
** Render text content (control reference).
** Called internally by keyguide_render().
**
** Parameters:
**   render - Render context for drawing text
*/
void	keyguide_render_content(t_render *render);

/*
** Render single section of controls (e.g., "Camera" section).
** Called internally by keyguide_render_content().
**
** Parameters:
**   render  - Render context
**   y       - Pointer to current Y position (updated after rendering)
**   heading - Section heading text (e.g., "Camera:")
**   lines   - Array of control lines (e.g., ["WASD - Move", "R/F - Pitch"])
**   count   - Number of lines in array
*/
void	keyguide_render_section(t_render *render, int *y,
			const char *heading, const char **lines, int count);

/* ========================================================================== */
/*                      INTEGRATION WITH MAIN HUD                             */
/* ========================================================================== */

/*
** IMPORTANT: Key guide visibility is synchronized with main HUD.
**
** When 'H' key is pressed:
**   1. hud_toggle() is called (in src/hud/hud_toggle.c)
**   2. Must update both: render->hud.visible and render->keyguide.visible
**
** Modified hud_toggle() implementation required:
**
**   void hud_toggle(t_render *render)
**   {
**       render->hud.visible = !render->hud.visible;
**       render->keyguide.visible = render->hud.visible;  // ADD THIS LINE
**   }
**
** Rationale:
**   - FR-010 requires both HUDs toggle together
**   - Consistent user experience (single 'H' key controls all overlays)
**   - Avoids confusion from independently controlled overlays
*/

#endif
