/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyguide_render_extra.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/28 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "keyguide.h"

/**
 * @brief Render resize controls section.
 *
 * @param render Render context containing key guide position.
 * @param y Current y position (in/out) for text rendering.
 */
static void	keyguide_render_resize(t_render *render, int *y)
{
	*y += KEYGUIDE_SECTION_GAP + KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 10, *y, KEYGUIDE_COLOR_TEXT, "Resize:");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "J/K - Diameter");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "N/M - Height");
}

/**
 * @brief Render rotation controls section.
 *
 * @param render Render context containing key guide position.
 * @param y Current y position (in/out) for text rendering.
 */
static void	keyguide_render_rotation(t_render *render, int *y)
{
	*y += KEYGUIDE_SECTION_GAP + KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 10, *y, KEYGUIDE_COLOR_TEXT, "Rotation:");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "U/O - Rot X");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT, "Y/P - Rot Y");
	*y += KEYGUIDE_LINE_HEIGHT;
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		render->keyguide.x + 20, *y, KEYGUIDE_COLOR_TEXT,
		"<-/-> - Rot Z");
}

/**
 * @brief Render extra key guide sections (resize and rotation).
 *
 * @param render Render context containing key guide state.
 * @param y Current y position (in/out) for text rendering.
 */
void	keyguide_render_extra(t_render *render, int *y)
{
	keyguide_render_resize(render, y);
	keyguide_render_rotation(render, y);
}
