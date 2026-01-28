/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_lifecycle.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:31:58 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "hud.h"
#include "keyguide.h"
#include "pixel_timing.h"
#include <stdlib.h>

/*
** Handle window close event (X button clicked).
** Cleans up resources and exits program.
*/
/**
 * @brief close window 함수
 *
 * @param param 파라미터
 *
 * @return int 반환값
 */
int	close_window(void *param)
{
	t_render	*render;

	render = (t_render *)param;
	pixel_timing_cleanup(&render->pixel_timing);
	keyguide_cleanup(&render->keyguide, render->mlx.mlx);
	hud_cleanup(&render->hud, render->mlx.mlx);
	cleanup_all(render->scene, render);
	exit(0);
	return (0);
}
