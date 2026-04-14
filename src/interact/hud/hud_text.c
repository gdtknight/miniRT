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

#include "render/render.h"
#include "interact/hud.h"

/**
 * @brief Mark the HUD as needing a redraw.
 *
 * @param render Render context owning the HUD state.
 */
void	hud_mark_dirty(t_render *render)
{
	render->hud.dirty = 1;
}

/**
 * @brief Draw a single text line on the HUD and advance y.
 *
 * @param r Render context for MLX output.
 * @param y Current y position (in/out).
 * @param color Text color value.
 * @param text String to display.
 */
void	hud_print_line(t_render *r, int *y, int color, char *text)
{
	mlx_string_put(r->mlx.mlx, r->mlx.win,
		HUD_MARGIN_X + 10, *y, color, text);
	*y += HUD_LINE_HEIGHT;
}

/**
 * @brief Print a labeled vec3 value on the HUD.
 *
 * @param render Render context for text output.
 * @param y Current y position (in/out).
 * @param label Text label preceding the vector.
 * @param vec Vector to format and display.
 */
void	hud_print_vec3(t_render *render, int *y,
		char *label, t_vec3 vec)
{
	char	buf[128];
	int		i;

	i = 0;
	while (i < 8)
		buf[i++] = ' ';
	while (*label && i < 80)
		buf[i++] = *label++;
	buf[i] = '\0';
	hud_format_vec3(buf + i, vec);
	hud_print_line(render, y, HUD_COLOR_TEXT, buf);
}

/**
 * @brief Copy src into dst up to max chars without
 *        null-terminating.
 *
 * @param dst Destination buffer.
 * @param src Source string.
 * @param max Maximum characters to copy.
 * @return Number of characters copied.
 */
int	hud_append(char *dst, const char *src, int max)
{
	int	i;

	i = 0;
	while (src[i] && i < max)
	{
		dst[i] = src[i];
		i++;
	}
	return (i);
}
