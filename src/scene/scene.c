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

#include "minirt.h"
#include "spatial.h"
#include <stdlib.h>
#include <string.h>

static void	scene_init_defaults(t_scene *scene)
{
	scene->shadow_config = init_shadow_config();
	scene->flags = SCENE_BVH_ENABLED;
	scene->bvh = NULL;
	metrics_init(&scene->metrics);
}

t_scene	*scene_create(void)
{
	t_scene	*scene;

	scene = malloc(sizeof(t_scene));
	if (!scene)
		return (NULL);
	memset(scene, 0, sizeof(t_scene));
	if (!object_list_init(&scene->objects, DEFAULT_OBJECT_CAPACITY))
	{
		free(scene);
		return (NULL);
	}
	scene_init_defaults(scene);
	return (scene);
}

void	scene_destroy(t_scene *scene)
{
	if (!scene)
		return ;
	object_list_destroy(&scene->objects);
	if (scene->bvh)
		bvh_destroy(scene->bvh);
	free(scene);
}
