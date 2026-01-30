/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "minirt.h"
#include <stdlib.h>

/**
 * @brief Get the geometric center of an object referenced by index.
 *
 * @param ref Object reference containing index.
 * @param scene_ptr Pointer to the scene.
 * @return t_vec3 Center point of the object.
 */
t_vec3	get_object_center(t_object_ref ref, void *scene_ptr)
{
	t_scene		*scene;
	t_object	*obj;

	scene = (t_scene *)scene_ptr;
	obj = &scene->objects.items[ref.index];
	if (obj->type == OBJ_SPHERE)
		return (obj->data.sphere.center);
	else if (obj->type == OBJ_CYLINDER)
		return (obj->data.cylinder.center);
	else
		return (obj->data.plane.point);
}

/**
 * @brief Fill object reference array with indices from the scene.
 *
 * @param scene Scene containing object list.
 * @param refs Output array to populate.
 */
static void	fill_object_refs(t_scene *scene, t_object_ref *refs)
{
	int	i;

	i = 0;
	while (i < scene->objects.count)
	{
		refs[i].index = i;
		i++;
	}
}

/**
 * @brief Build or rebuild the BVH for the scene.
 *
 * Allocates an index array, creates the BVH if needed, and constructs the
 * hierarchy from current scene objects.
 *
 * @param scene Scene containing objects and BVH state.
 */
void	scene_build_bvh(t_scene *scene)
{
	t_object_ref	*refs;
	int				total;

	if (!(scene->flags & SCENE_BVH_ENABLED))
		return ;
	total = scene->objects.count;
	if (total == 0)
		return ;
	refs = malloc(sizeof(t_object_ref) * total);
	if (!refs)
		return ;
	fill_object_refs(scene, refs);
	if (!scene->bvh)
		scene->bvh = bvh_create();
	if (scene->bvh)
		bvh_build(scene->bvh, refs, total, scene);
	free(refs);
}
