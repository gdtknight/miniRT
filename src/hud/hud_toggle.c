/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_toggle.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.h"
#include "hud.h"

/**
 * @brief Toggle HUD visibility and sync key guide visibility.
 *
 * Marks the render as dirty to force a redraw.
 *
 * @param render Render context containing HUD and key guide state.
 */
void	hud_toggle(t_render *render)
{
	render->hud.visible = !render->hud.visible;
	render->keyguide.visible = render->hud.visible;
	render_set_flag(render, RENDER_DIRTY);
	render->hud.dirty = 1;
}
