/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_navigation.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.h"
#include "hud.h"

int	hud_get_global_index(t_selection sel, t_scene *scene)
{
	(void)scene;
	return (sel.index);
}

void	hud_get_selection_from_global(t_selection *sel, int idx,
	t_scene *scene)
{
	if (idx >= 0 && idx < scene->objects.count)
	{
		sel->type = scene->objects.items[idx].type;
		sel->index = idx;
	}
	else
	{
		sel->type = OBJ_NONE;
		sel->index = 0;
	}
}

void	hud_select_next(t_render *render)
{
	int	total;
	int	idx;

	total = render->scene->objects.count;
	if (total == 0)
		return ;
	idx = render->selection.index;
	idx = (idx + 1) % total;
	hud_get_selection_from_global(&render->selection, idx, render->scene);
	render->hud.dirty = 1;
}

void	hud_select_prev(t_render *render)
{
	int	total;
	int	idx;

	total = render->scene->objects.count;
	if (total == 0)
		return ;
	idx = render->selection.index;
	idx = (idx - 1 + total) % total;
	hud_get_selection_from_global(&render->selection, idx, render->scene);
	render->hud.dirty = 1;
}

void	hud_change_page(t_render *render, int direction)
{
	if (render->hud.total_pages <= 1)
		return ;
	if (direction > 0)
		render->hud.current_page = (render->hud.current_page + 1)
			% render->hud.total_pages;
	else
		render->hud.current_page = (render->hud.current_page - 1
				+ render->hud.total_pages) % render->hud.total_pages;
	render->hud.dirty = 1;
	render_set_flag(render, RENDER_DIRTY);
}
