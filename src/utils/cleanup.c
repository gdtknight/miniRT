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

/* Legacy cleanup function - delegates to scene_destroy */
void	cleanup_scene(t_scene *scene)
{
	scene_destroy(scene);
}

/* Legacy cleanup function - delegates to render_destroy */
void	cleanup_render(void *param)
{
	render_destroy((t_render *)param);
}

/* Legacy cleanup function */
void	cleanup_all(t_scene *scene, void *param)
{
	cleanup_render(param);
	cleanup_scene(scene);
}
