/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_navigation.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.h"
#include "hud.h"

int	hud_get_global_index(t_selection sel, t_scene *scene)
{
	if (sel.type == OBJ_SPHERE)
		return (sel.index);
	else if (sel.type == OBJ_PLANE)
		return (scene->sphere_count + sel.index);
	else if (sel.type == OBJ_CYLINDER)
		return (scene->sphere_count + scene->plane_count + sel.index);
	return (0);
}

t_selection	hud_get_selection_from_global(int global_idx, t_scene *scene)
{
	t_selection	sel;

	if (global_idx < scene->sphere_count)
	{
		sel.type = OBJ_SPHERE;
		sel.index = global_idx;
	}
	else if (global_idx < scene->sphere_count + scene->plane_count)
	{
		sel.type = OBJ_PLANE;
		sel.index = global_idx - scene->sphere_count;
	}
	else
	{
		sel.type = OBJ_CYLINDER;
		sel.index = global_idx - scene->sphere_count - scene->plane_count;
	}
	return (sel);
}

void	hud_select_next(t_render *render)
{
	int	total_objects;
	int	global_idx;

	total_objects = render->scene->sphere_count
		+ render->scene->plane_count + render->scene->cylinder_count;
	if (total_objects == 0)
		return ;
	global_idx = hud_get_global_index(render->selection, render->scene);
	global_idx = (global_idx + 1) % total_objects;
	render->selection = hud_get_selection_from_global(global_idx,
			render->scene);
	render->hud.dirty = 1;
}

void	hud_select_prev(t_render *render)
{
	int	total_objects;
	int	global_idx;

	total_objects = render->scene->sphere_count
		+ render->scene->plane_count + render->scene->cylinder_count;
	if (total_objects == 0)
		return ;
	global_idx = hud_get_global_index(render->selection, render->scene);
	global_idx = (global_idx - 1 + total_objects) % total_objects;
	render->selection = hud_get_selection_from_global(global_idx,
			render->scene);
	render->hud.dirty = 1;
}

void	hud_page_down(t_render *render)
{
	if (render->hud.total_pages <= 1)
		return ;
	render->hud.current_page = (render->hud.current_page + 1)
		% render->hud.total_pages;
	render->hud.dirty = 1;
}

void	hud_page_up(t_render *render)
{
	if (render->hud.total_pages <= 1)
		return ;
	render->hud.current_page = (render->hud.current_page - 1
			+ render->hud.total_pages) % render->hud.total_pages;
	render->hud.dirty = 1;
}
