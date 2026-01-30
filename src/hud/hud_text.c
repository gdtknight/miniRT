/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_text.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.h"
#include "hud.h"

/**
 * @brief Mark the HUD as needing a redraw.
 *
 * @param render Render context containing HUD state.
 */
void	hud_mark_dirty(t_render *render)
{
	render->hud.dirty = 1;
}

/**
 * @brief Render one row of the HUD background overlay.
 *
 * @param render Render context containing image buffer.
 * @param y Screen y coordinate of the row.
 */
void	hud_render_background_row(t_render *render, int y)
{
	int				x;
	int				scene_color;
	int				blended;
	t_pixel_params	params;

	x = HUD_MARGIN_X;
	params.img_data = render->mlx.img.data;
	params.y = y;
	params.size_line = render->mlx.img.size_line;
	params.bpp = render->mlx.img.bpp;
	while (x < HUD_MARGIN_X + HUD_WIDTH)
	{
		params.x = x;
		scene_color = get_pixel(&params);
		blended = blend_colors(scene_color, HUD_COLOR_BG, HUD_BG_ALPHA);
		set_pixel(&params, blended);
		x++;
	}
}

/**
 * @brief Render the full HUD background overlay.
 *
 * @param render Render context containing image buffer.
 */
void	hud_render_background(t_render *render)
{
	int	y;

	y = HUD_MARGIN_Y;
	while (y < HUD_MARGIN_Y + HUD_HEIGHT)
	{
		hud_render_background_row(render, y);
		y++;
	}
}

/**
 * @brief Format a labeled vec3 and render it on the HUD.
 *
 * @param render Render context containing MLX handles.
 * @param y Current y position (in/out).
 * @param label Label text to display.
 * @param vec Vector to format and display.
 */
void	format_and_print_vec3(t_render *render, int *y,
		char *label, t_vec3 vec)
{
	char	buf[128];
	int		i;

	i = 0;
	while (i < 8)
		buf[i++] = ' ';
	while (*label)
		buf[i++] = *label++;
	buf[i] = '\0';
	hud_format_vec3(buf + i, vec);
	mlx_string_put(render->mlx.mlx, render->mlx.win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
}

/**
 * @brief Copy a string into a buffer and return length copied.
 *
 * @param dst Destination buffer.
 * @param src Source string.
 * @return int Number of characters copied.
 */
int	copy_str_to_buf(char *dst, char *src)
{
	int	i;

	i = 0;
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	return (i);
}
