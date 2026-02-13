/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_objects.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.h"
#include "hud.h"
#include "hud_text.h"
#include "hud_obj_render.h"

/**
 * @brief Render the object list header with page info.
 *
 * @param render Render context containing HUD state.
 * @param y Current y position (in/out).
 */
static void	render_object_header(t_render *render, int *y)
{
	char	buf[64];
	int		len;

	len = 0;
	buf[len++] = '-';
	buf[len++] = '-';
	buf[len++] = '-';
	buf[len++] = ' ';
	len += copy_str_to_buf(buf + len, "Objects (Page ", 64 - len);
	ft_itoa_buf(buf + len, render->hud.current_page + 1);
	while (buf[len])
		len++;
	buf[len++] = '/';
	ft_itoa_buf(buf + len, render->hud.total_pages);
	while (buf[len])
		len++;
	len += copy_str_to_buf(buf + len, ") ---", 64 - len);
	buf[len] = '\0';
	mlx_string_put(render->mlx.mlx, render->mlx.win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
}

/**
 * @brief Render an object entry by global index.
 *
 * Highlights the selected object entry.
 *
 * @param render Render context containing scene and selection.
 * @param g_idx Global object index.
 * @param y Current y position (in/out).
 */
static void	render_object_by_index(t_render *render, int g_idx, int *y)
{
	t_selection	sel;
	int			color;

	hud_get_selection_from_global(&sel, g_idx, render->scene);
	color = HUD_COLOR_TEXT;
	if (sel.index == render->selection.index
		&& sel.type == render->selection.type)
		color = HUD_COLOR_HIGHLIGHT;
	render_object_entry(render, g_idx, y, color);
}

/**
 * @brief Render the HUD object list for the current page.
 *
 * @param render Render context containing HUD and scene.
 * @param y Current y position (in/out).
 */
void	hud_render_objects(t_render *render, int *y)
{
	int	total_objects;
	int	start_idx;
	int	end_idx;
	int	i;

	total_objects = render->scene->objects.count;
	render->hud.total_pages = hud_calculate_total_pages(render->scene);
	if (render->hud.current_page >= render->hud.total_pages)
		render->hud.current_page = 0;
	render_object_header(render, y);
	if (total_objects == 0)
		return ;
	start_idx = render->hud.current_page * HUD_OBJECTS_PER_PAGE;
	end_idx = start_idx + HUD_OBJECTS_PER_PAGE;
	if (end_idx > total_objects)
		end_idx = total_objects;
	i = start_idx;
	while (i < end_idx)
	{
		render_object_by_index(render, i, y);
		i++;
	}
}
