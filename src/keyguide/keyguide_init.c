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

#include "keyguide.h"
#include "window.h"

/**
 * @brief Initialize the key guide overlay state.
 *
 * Computes the on-screen position and sets initial visibility.
 *
 * @param keyguide Key guide state to initialize.
 * @param mlx MLX connection pointer (unused, kept for API consistency).
 * @param win Window pointer (unused, kept for API consistency).
 * @return int 0 on success.
 */
int	keyguide_init(t_keyguide_state *keyguide, void *mlx, void *win)
{
	(void)mlx;
	(void)win;
	keyguide->x = WINDOW_WIDTH - KEYGUIDE_WIDTH - KEYGUIDE_MARGIN_X;
	keyguide->y = KEYGUIDE_MARGIN_Y;
	keyguide->visible = 1;
	return (0);
}
