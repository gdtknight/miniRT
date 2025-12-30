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

int	keyguide_init(t_keyguide_state *keyguide, void *mlx, void *win)
{
	(void)win;
	keyguide->bg_img = mlx_new_image(mlx, KEYGUIDE_WIDTH, KEYGUIDE_HEIGHT);
	if (!keyguide->bg_img)
		return (-1);
	keyguide->bg_data = mlx_get_data_addr(keyguide->bg_img,
			&keyguide->bpp, &keyguide->size_line, &keyguide->endian);
	keyguide->x = WINDOW_WIDTH - KEYGUIDE_WIDTH - KEYGUIDE_MARGIN_X;
	keyguide->y = KEYGUIDE_MARGIN_Y;
	keyguide->visible = 1;
	return (0);
}
