/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyguide_render.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/30 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "keyguide.h"
#include "hud.h"
#include "window.h"

int	mlx_string_put(void *mlx_ptr, void *win_ptr, int x, int y,
		int color, char *string);

void	keyguide_render_background(t_render *render)
{
	int	x;
	int	y;
	int	scene_color;
	int	blended;

	y = render->keyguide.y;
	while (y < render->keyguide.y + KEYGUIDE_HEIGHT)
	{
		x = render->keyguide.x;
		while (x < render->keyguide.x + KEYGUIDE_WIDTH)
		{
			scene_color = get_pixel(render->img_data, x, y,
					render->size_line, render->bpp);
			blended = blend_colors(scene_color, KEYGUIDE_COLOR_BG,
					KEYGUIDE_BG_ALPHA);
			set_pixel(render->img_data, x, y, blended,
				render->size_line, render->bpp);
			x++;
		}
		y++;
	}
}

void	keyguide_render_content(t_render *render)
{
	int	y;

	y = render->keyguide.y + 20;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 10, y, KEYGUIDE_COLOR_HEADING, "CONTROLS");
	y += 30;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 10, y, KEYGUIDE_COLOR_TEXT, "Navigation:");
	y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, y, KEYGUIDE_COLOR_TEXT, "ESC - Exit");
	y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, y, KEYGUIDE_COLOR_TEXT, "H - Toggle HUD");
	y += KEYGUIDE_SECTION_GAP + KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 10, y, KEYGUIDE_COLOR_TEXT, "Camera:");
	y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, y, KEYGUIDE_COLOR_TEXT, "W/X - Fwd/Back");
	y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, y, KEYGUIDE_COLOR_TEXT, "A/D - Left/Right");
	y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, y, KEYGUIDE_COLOR_TEXT, "Q/Z - Up/Down");
	y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, y, KEYGUIDE_COLOR_TEXT, "E/C - Pitch");
	y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, y, KEYGUIDE_COLOR_TEXT, "S - Reset");
}

void	keyguide_render_content2(t_render *render, int *y)
{
	*y += KEYGUIDE_SECTION_GAP + KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 10, *y, KEYGUIDE_COLOR_TEXT, "Objects:");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "TAB - Next");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "[] - Select");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "R/T - X axis");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "F/G - Y axis");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "V/B - Z axis");
	*y += KEYGUIDE_SECTION_GAP + KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 10, *y, KEYGUIDE_COLOR_TEXT, "Display:");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "I - Info");
}

void	keyguide_render(t_render *render)
{
	int	y;

	if (!render->keyguide.visible)
		return ;
	keyguide_render_background(render);
	keyguide_render_content(render);
	y = render->keyguide.y + 20 + 30 + KEYGUIDE_LINE_HEIGHT * 7
		+ KEYGUIDE_SECTION_GAP + KEYGUIDE_LINE_HEIGHT * 3;
	keyguide_render_content2(render, &y);
}
