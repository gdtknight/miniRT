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

/**
 * @brief Check if the scene has an ambient light definition.
 *
 * @param scene Scene to query.
 * @return int 1 if ambient flag is set, 0 otherwise.
 */
int	scene_has_ambient(t_scene *scene)
{
	if (!scene)
		return (0);
	return ((scene->flags & SCENE_HAS_AMBIENT) != 0);
}

/**
 * @brief Check if the scene has a camera definition.
 *
 * @param scene Scene to query.
 * @return int 1 if camera flag is set, 0 otherwise.
 */
int	scene_has_camera(t_scene *scene)
{
	if (!scene)
		return (0);
	return ((scene->flags & SCENE_HAS_CAMERA) != 0);
}

/**
 * @brief Check if the scene has a light definition.
 *
 * @param scene Scene to query.
 * @return int 1 if light flag is set, 0 otherwise.
 */
int	scene_has_light(t_scene *scene)
{
	if (!scene)
		return (0);
	return ((scene->flags & SCENE_HAS_LIGHT) != 0);
}

/**
 * @brief Set a scene flag bit.
 *
 * @param scene Scene to update.
 * @param flag Flag bit to set.
 */
void	scene_set_flag(t_scene *scene, int flag)
{
	if (scene)
		scene->flags |= flag;
}

/**
 * @brief Clear a scene flag bit.
 *
 * @param scene Scene to update.
 * @param flag Flag bit to clear.
 */
void	scene_clear_flag(t_scene *scene, int flag)
{
	if (scene)
		scene->flags &= ~flag;
}
