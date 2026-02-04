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
 * Delegates all resource cleanup to cleanup_all (which calls render_destroy)
 * then exits the process.
 *
 * @param param Pointer to the render context.
 * @return int Always returns 0 for MLX event handling.
 */
int	close_window(void *param)
{
	t_render	*render;

	render = (t_render *)param;
	cleanup_all(render->scene, render);
	exit(0);
	return (0);
}
