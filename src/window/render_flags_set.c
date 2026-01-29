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

#include "window.h"

int	render_has_flag(t_render *render, int flag)
{
	if (!render)
		return (0);
	return ((render->state_flags & flag) != 0);
}

void	render_set_flag(t_render *render, int flag)
{
	if (render)
		render->state_flags |= flag;
}

void	render_clear_flag(t_render *render, int flag)
{
	if (render)
		render->state_flags &= ~flag;
}
