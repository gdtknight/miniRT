/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyguide_init.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/30 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render/render.h"
#include "interact/keyguide.h"

/**
 * @brief Initialize the key guide overlay state.
 *
 * Computes the on-screen position and sets initial visibility.
 *
 * @param keyguide Key guide state to initialize.
 */
void	keyguide_init(t_keyguide_state *keyguide)
{
	keyguide->x = WINDOW_WIDTH - KEYGUIDE_WIDTH - KEYGUIDE_MARGIN_X;
	keyguide->y = KEYGUIDE_MARGIN_Y;
	keyguide->visible = 1;
	keyguide->dirty = 1;
}
