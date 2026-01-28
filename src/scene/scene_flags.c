/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene_flags.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int	scene_has_ambient(t_scene *scene)
{
	if (!scene)
		return (0);
	return ((scene->flags & SCENE_HAS_AMBIENT) != 0);
}

int	scene_has_camera(t_scene *scene)
{
	if (!scene)
		return (0);
	return ((scene->flags & SCENE_HAS_CAMERA) != 0);
}

int	scene_has_light(t_scene *scene)
{
	if (!scene)
		return (0);
	return ((scene->flags & SCENE_HAS_LIGHT) != 0);
}

void	scene_set_flag(t_scene *scene, int flag)
{
	if (scene)
		scene->flags |= flag;
}

void	scene_clear_flag(t_scene *scene, int flag)
{
	if (scene)
		scene->flags &= ~flag;
}
