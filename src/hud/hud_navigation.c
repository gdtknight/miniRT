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

/**
 * @brief Convert a global index to a selection struct.
 *
 * @param sel Output selection to populate.
 * @param idx Global object index.
 * @param scene Scene containing objects.
 */
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

/**
 * @brief Advance selection to the next object.
 *
 * Wraps around at the end of the object list and marks HUD dirty.
 *
 * @param render Render context containing selection and HUD state.
 */
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


/**
 * @brief Change HUD object list page.
 *
 * Increments or decrements the current page and marks the render dirty.
 *
 * @param render Render context containing HUD state.
 * @param direction Positive for next page, negative for previous.
 */
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
}
