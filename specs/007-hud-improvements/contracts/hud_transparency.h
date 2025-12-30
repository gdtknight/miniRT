/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_transparency.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/30 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUD_TRANSPARENCY_H
# define HUD_TRANSPARENCY_H

# include "minirt.h"

/*
** ============================================================================
**                    HUD TRANSPARENCY API CONTRACT
** ============================================================================
**
** Purpose: Provides alpha blending utilities for semi-transparent HUD overlays
** Dependencies: MiniLibX for image buffer access
** Constraints: MiniLibX does not support native alpha channel
**
** Design Decision:
**   Manual alpha blending via direct pixel manipulation in image buffers.
**   MiniLibX limitation: No ARGB support, only RGB. We implement software
**   alpha blending by reading scene pixels and compositing with HUD background.
**
** Performance:
**   - Pre-compute background once during init: ~0.5ms
**   - Runtime composite: Minimal (background pre-blended, only text overlaid)
**
** 42 School Compliance:
**   Uses only allowed functions: mlx_get_data_addr, math operations, malloc/free
**   No pthread, SIMD, or external libraries
*/

/* ========================================================================== */
/*                         ALPHA BLENDING UTILITIES                           */
/* ========================================================================== */

/*
** Blend two RGB colors using alpha transparency.
**
** Formula: result = (1 - alpha) * bg_color + alpha * fg_color
**
** Parameters:
**   bg_color - Background color (scene rendering, 0xRRGGBB format)
**   fg_color - Foreground color (HUD overlay, 0xRRGGBB format)
**   alpha    - Transparency level: 0.0=fully transparent, 1.0=fully opaque
**
** Returns:
**   Blended color in 0xRRGGBB format
**
** Example:
**   int scene_pixel = 0xFF0000;  // Red scene
**   int hud_bg = 0x000000;       // Black HUD background
**   double alpha = 0.65;         // 65% opaque
**   int blended = blend_colors(scene_pixel, hud_bg, alpha);
**   // Result: Dark red (scene partially visible through HUD)
**
** Performance:
**   - Pure arithmetic, no branches: ~5 CPU cycles
**   - Suitable for per-pixel blending in tight loops
**
** Constraints:
**   - alpha MUST be in [0.0, 1.0] range (undefined behavior otherwise)
**   - Color values MUST be valid 24-bit RGB (no alpha channel)
*/
int		blend_colors(int bg_color, int fg_color, double alpha);

/*
** Extract red channel from RGB color.
**
** Parameters:
**   color - RGB color in 0xRRGGBB format
**
** Returns:
**   Red component in range [0, 255]
**
** Implementation:
**   return ((color >> 16) & 0xFF);
*/
int		get_red(int color);

/*
** Extract green channel from RGB color.
**
** Parameters:
**   color - RGB color in 0xRRGGBB format
**
** Returns:
**   Green component in range [0, 255]
**
** Implementation:
**   return ((color >> 8) & 0xFF);
*/
int		get_green(int color);

/*
** Extract blue channel from RGB color.
**
** Parameters:
**   color - RGB color in 0xRRGGBB format
**
** Returns:
**   Blue component in range [0, 255]
**
** Implementation:
**   return (color & 0xFF);
*/
int		get_blue(int color);

/*
** Combine RGB channels into single color integer.
**
** Parameters:
**   r - Red component [0, 255]
**   g - Green component [0, 255]
**   b - Blue component [0, 255]
**
** Returns:
**   Combined color in 0xRRGGBB format
**
** Implementation:
**   return ((r << 16) | (g << 8) | b);
**
** Validation:
**   Input values should be clamped to [0, 255] before calling.
**   Out-of-range values produce undefined colors.
*/
int		make_color(int r, int g, int b);

/* ========================================================================== */
/*                    PIXEL BUFFER MANIPULATION                               */
/* ========================================================================== */

/*
** Get pixel color from MiniLibX image buffer.
**
** Parameters:
**   img_data  - Pixel data pointer from mlx_get_data_addr
**   x, y      - Pixel coordinates
**   size_line - Bytes per scanline (from mlx_get_data_addr)
**   bpp       - Bits per pixel (from mlx_get_data_addr)
**
** Returns:
**   Pixel color in 0xRRGGBB format
**
** Bounds Checking:
**   Caller MUST ensure x, y are within image bounds.
**   No internal validation performed (performance optimization).
**
** Example:
**   char *data;
**   int bpp, size_line, endian;
**   data = mlx_get_data_addr(img, &bpp, &size_line, &endian);
**   int color = get_pixel(data, 100, 50, size_line, bpp);
*/
int		get_pixel(char *img_data, int x, int y, int size_line, int bpp);

/*
** Set pixel color in MiniLibX image buffer.
**
** Parameters:
**   img_data  - Pixel data pointer from mlx_get_data_addr
**   x, y      - Pixel coordinates
**   color     - Color to set (0xRRGGBB format)
**   size_line - Bytes per scanline
**   bpp       - Bits per pixel
**
** Returns:
**   void
**
** Bounds Checking:
**   Caller MUST ensure x, y are within image bounds.
**
** Example:
**   set_pixel(data, 100, 50, 0xFF0000, size_line, bpp);  // Set red pixel
*/
void	set_pixel(char *img_data, int x, int y, int color,
			int size_line, int bpp);

/* ========================================================================== */
/*                    BACKGROUND BUFFER CREATION                              */
/* ========================================================================== */

/*
** Fill rectangular region in image buffer with alpha-blended color.
**
** Parameters:
**   img_data  - Target image buffer (modified in-place)
**   rect      - Rectangle bounds {x, y, width, height}
**   color     - Foreground color (HUD background)
**   alpha     - Transparency level [0.0, 1.0]
**   scene_img - Scene image buffer (read-only, for blending)
**   size_line - Bytes per scanline
**   bpp       - Bits per pixel
**
** Behavior:
**   For each pixel in rectangle:
**     1. Read scene pixel color at (x, y)
**     2. Blend scene color with HUD color using alpha
**     3. Write blended color to target buffer at (x, y)
**
** Performance:
**   - O(width * height) pixel operations
**   - For 400x600 HUD: ~240K pixels, ~1ms on M1 MacBook
**
** Safety:
**   - Both img_data and scene_img MUST be valid image buffers
**   - Rectangle MUST be within both image bounds
**   - No validation performed (caller responsibility)
**
** Example:
**   // Pre-render semi-transparent HUD background during init
**   t_rect hud_rect = {0, 0, 400, 600};
**   fill_rect_alpha(hud_bg_data, hud_rect, 0x000000, 0.65,
**                   scene_data, size_line, bpp);
*/
void	fill_rect_alpha(char *img_data, t_rect rect, int color, double alpha,
			char *scene_img, int size_line, int bpp);

/*
** Rectangle structure for fill_rect_alpha.
*/
typedef struct s_rect
{
	int	x;
	int	y;
	int	width;
	int	height;
}	t_rect;

/* ========================================================================== */
/*                         USAGE EXAMPLES                                     */
/* ========================================================================== */

/*
** Example 1: Initialize HUD with transparent background
**
**   int hud_init(t_hud_state *hud, void *mlx, void *win)
**   {
**       hud->bg_img = mlx_new_image(mlx, 400, 600);
**       hud->bg_data = mlx_get_data_addr(hud->bg_img, &hud->bpp,
**                                         &hud->size_line, &hud->endian);
**       
**       // Pre-render background with transparency
**       // Note: Scene not available yet, so fill with solid color for now
**       // Real blending happens during first render when scene exists
**       t_rect hud_rect = {0, 0, 400, 600};
**       for (int y = 0; y < 600; y++)
**           for (int x = 0; x < 400; x++)
**               set_pixel(hud->bg_data, x, y, 0x000000,
**                         hud->size_line, hud->bpp);
**       
**       return (0);
**   }
**
** Example 2: Runtime blending during render
**
**   void hud_render(t_render *render)
**   {
**       if (!render->hud.visible)
**           return;
**       
**       // Composite HUD background with scene
**       for (int y = 0; y < 600; y++)
**       {
**           for (int x = 0; x < 400; x++)
**           {
**               int scene_px = get_pixel(render->img_data, x, y,
**                                        render->size_line, render->bpp);
**               int blended = blend_colors(scene_px, 0x000000, 0.65);
**               set_pixel(render->img_data, x, y, blended,
**                         render->size_line, render->bpp);
**           }
**       }
**       
**       // Then render text on top
**       hud_render_content(render);
**   }
**
** Performance Note:
**   For better performance, consider pre-rendering background once per scene
**   change rather than per frame. Use dirty flag to detect when re-blend needed.
*/

#endif
