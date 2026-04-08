/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_flags_set.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render/render.h"

/**
 * @brief Check whether a render state flag is set.
 *
 * @param render Render context to query.
 * @param flag Flag bit to test.
 * @return int 1 if the flag is set, 0 otherwise.
 */
int	render_has_flag(t_render *render, int flag)
{
	if (!render)
		return (0);
	return ((render->state_flags & flag) != 0);
}

/**
 * @brief Set a render state flag.
 *
 * @param render Render context to update.
 * @param flag Flag bit to set.
 */
void	render_set_flag(t_render *render, int flag)
{
	if (render)
		render->state_flags |= flag;
}

/**
 * @brief Clear a render state flag.
 *
 * @param render Render context to update.
 * @param flag Flag bit to clear.
 */
void	render_clear_flag(t_render *render, int flag)
{
	if (render)
		render->state_flags &= ~flag;
}
