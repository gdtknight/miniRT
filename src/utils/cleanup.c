/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:11 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include <stdlib.h>

/**
 * @brief Legacy scene cleanup wrapper.
 *
 * Delegates to scene_destroy for backward compatibility.
 *
 * @param scene Scene to destroy.
 */
void	cleanup_scene(t_scene *scene)
{
	scene_destroy(scene);
}

/**
 * @brief Legacy render cleanup wrapper.
 *
 * Delegates to render_destroy for backward compatibility.
 *
 * @param param Pointer to render context.
 */
void	cleanup_render(void *param)
{
	render_destroy((t_render *)param);
}

/**
 * @brief Legacy combined cleanup wrapper.
 *
 * Cleans render and scene resources in order.
 *
 * @param scene Scene to destroy.
 * @param param Pointer to render context.
 */
void	cleanup_all(t_scene *scene, void *param)
{
	cleanup_render(param);
	cleanup_scene(scene);
}
