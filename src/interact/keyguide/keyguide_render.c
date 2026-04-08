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

#include "render/render.h"
#include "interact/keyguide.h"
#include "render/render.h"
#include "interact/hud.h"

/**
 * @brief Render the camera controls section in the key guide.
 *
 * @param render Render context containing key guide position.
 * @param y Current y position (in/out) for text rendering.
 */
static void	keyguide_render_camera_section(t_render *render, int *y)
{
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 10, *y, KEYGUIDE_COLOR_TEXT, "Camera:");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "W/X - Fwd/Back");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "A/D - Left/Right");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "Q/Z - Up/Down");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "E/C - Pitch");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "S - Reset");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "1/3 - Yaw");
}

/**
 * @brief Render the main key guide content (navigation + camera).
 *
 * @param render Render context containing key guide position.
 */
void	keyguide_render_content(t_render *render)
{
	int	y;

	y = render->keyguide.y + 20;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 10, y, KEYGUIDE_COLOR_HEADING, "CONTROLS");
	y += 30;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 10, y, KEYGUIDE_COLOR_TEXT, "Navigation:");
	y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, y, KEYGUIDE_COLOR_TEXT, "ESC - Exit");
	y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, y, KEYGUIDE_COLOR_TEXT, "H - Toggle HUD");
	y += KEYGUIDE_SECTION_GAP + KEYGUIDE_LINE_HEIGHT;
	keyguide_render_camera_section(render, &y);
}

/**
 * @brief Render the secondary key guide content (objects + display).
 *
 * @param render Render context containing key guide position.
 * @param y Current y position (in/out) for text rendering.
 */
static void	keyguide_render_objects_section(t_render *render, int *y)
{
	*y += KEYGUIDE_SECTION_GAP + KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 10, *y, KEYGUIDE_COLOR_TEXT, "Objects:");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "TAB - Select");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "R/T - X axis");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "F/G - Y axis");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "V/B - Z axis");
}

/**
 * @brief Render the full key guide overlay.
 *
 * Draws all sections if the key guide is visible.
 *
 * @param render Render context containing key guide state.
 */
void	keyguide_render(t_render *render)
{
	int	y;

	if (!render->keyguide.visible || !render->keyguide.dirty)
		return ;
	keyguide_render_content(render);
	y = render->keyguide.y + 20 + 30
		+ KEYGUIDE_LINE_HEIGHT * KEYGUIDE_CONTENT1_LINES
		+ KEYGUIDE_SECTION_GAP + KEYGUIDE_LINE_HEIGHT * 3;
	keyguide_render_objects_section(render, &y);
	keyguide_render_extra(render, &y);
	render->keyguide.dirty = 0;
}
