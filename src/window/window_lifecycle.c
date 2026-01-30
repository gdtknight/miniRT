/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_lifecycle.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:35:40 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "hud.h"
#include "keyguide.h"
#include "pixel_timing.h"
#include <stdlib.h>

/**
 * @brief Handle window close event and shut down cleanly.
 *
 * Releases timing, HUD, and key guide resources, then frees scene and render
 * context before exiting the process.
 *
 * @param param Pointer to the render context.
 * @return int Always returns 0 for MLX event handling.
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
