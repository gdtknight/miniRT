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

void	hud_toggle(t_render *render)
{
	render->hud.visible = !render->hud.visible;
	render->hud.dirty = 1;
}
