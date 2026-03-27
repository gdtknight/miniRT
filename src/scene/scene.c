/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scene.h"
#include "spatial.h"
#include <stdlib.h>

/**
 * @brief Free shadow offset LUT memory.
 *
 * @param config Shadow configuration containing offset_lut.
 */
static void	free_shadow_offset_lut(t_shadow_config *config)
{
	if (config && config->offset_lut)
	{
		free(config->offset_lut);
		config->offset_lut = NULL;
	}
}

/**
 * @brief Initialize default scene settings.
 *
 * Sets shadow configuration defaults, enables BVH, clears BVH pointer,
 * and initializes metrics.
 *
 * @param scene Scene to initialize.
 */
static void	scene_init_defaults(t_scene *scene)
{
	scene->shadow_config = init_shadow_config();
	scene->flags = SCENE_BVH_ENABLED;
	scene->bvh = NULL;
	metrics_init(&scene->metrics);
}

/**
 * @brief Allocate and initialize a new scene.
 *
 * Initializes object list capacity and default settings.
 *
 * @return t_scene* Newly created scene or NULL on failure.
 */
t_scene	*scene_create(void)
{
	t_scene	*scene;

	scene = malloc(sizeof(t_scene));
	if (!scene)
		return (NULL);
	ft_bzero(scene, sizeof(t_scene));
	if (!object_list_init(&scene->objects, DEFAULT_OBJECT_CAPACITY))
	{
		free(scene);
		return (NULL);
	}
	scene_init_defaults(scene);
	return (scene);
}

/**
 * @brief Destroy a scene and free all associated resources.
 *
 * Releases object list storage, BVH data, and the scene structure.
 *
 * @param scene Scene to destroy.
 */
void	scene_destroy(t_scene *scene)
{
	if (!scene)
		return ;
	object_list_destroy(&scene->objects);
	if (scene->bvh)
		bvh_destroy(scene->bvh);
	free_shadow_offset_lut(&scene->shadow_config);
	free(scene);
}
