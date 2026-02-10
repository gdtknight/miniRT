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
#include <stdlib.h>

/**
 * @brief Handle window close event and shut down cleanly.
 *
 * Frees render and scene resources, then exits the process.
 *
 * @param param Pointer to the render context.
 * @return int Always returns 0 for MLX event handling.
 */
int	close_window(void *param)
{
	t_render	*render;
	t_scene		*scene;

	render = (t_render *)param;
	scene = render->scene;
	render_destroy(render);
	scene_destroy(scene);
	exit(0);
	return (0);
}
