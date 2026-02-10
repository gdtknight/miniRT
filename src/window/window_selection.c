/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_selection.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window_internal.h"
#include "hud.h"

/**
 * @brief Handle object selection cycling keys.
 *
 * Moves selection forward/backward through objects and marks the HUD dirty.
 *
 * @param render Render context containing scene and HUD state.
 * @param keycode Key code indicating selection direction.
 */
void	handle_object_selection(t_render *render, int keycode)
{
	if (render->scene->objects.count == 0)
		return ;
	if (keycode == KEY_BRACKET_RIGHT)
		hud_select_next(render);
	else if (keycode == KEY_BRACKET_LEFT)
		hud_select_prev(render);
	hud_mark_dirty(render);
}
