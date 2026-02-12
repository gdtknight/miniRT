/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

/**
 * @brief Calculate the number of object list pages for the HUD.
 *
 * @param scene Scene containing object list.
 * @return int Total number of pages (minimum 1).
 */
int	hud_calculate_total_pages(t_scene *scene)
{
	int	total_objects;
	int	total_pages;

	total_objects = scene->objects.count;
	if (total_objects == 0)
		return (1);
	total_pages = (total_objects + HUD_OBJECTS_PER_PAGE - 1)
		/ HUD_OBJECTS_PER_PAGE;
	return (total_pages);
}

/**
 * @brief Initialize HUD state.
 *
 * Sets initial state values for visibility, pagination, and dirty flag.
 *
 * @param hud HUD state to initialize.
 * @return int 0 on success.
 */
int	hud_init(t_hud_state *hud)
{
	hud->visible = 1;
	hud->current_page = 0;
	hud->dirty = 1;
	return (0);
}
