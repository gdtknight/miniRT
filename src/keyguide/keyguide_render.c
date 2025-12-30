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
		render->keyguide.x + 20, y, KEYGUIDE_COLOR_TEXT, "WASD - Move");
	y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, y, KEYGUIDE_COLOR_TEXT, "R/F - Pitch");
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
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "Numpad - Move");
	*y += KEYGUIDE_SECTION_GAP + KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 10, *y, KEYGUIDE_COLOR_TEXT, "Rendering:");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "B - Toggle BVH");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx, render->win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "Q - Adaptive");
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
	y = render->keyguide.y + 20 + 30 + KEYGUIDE_LINE_HEIGHT * 4
		+ KEYGUIDE_SECTION_GAP + KEYGUIDE_LINE_HEIGHT * 3;
	keyguide_render_content2(render, &y);
}
