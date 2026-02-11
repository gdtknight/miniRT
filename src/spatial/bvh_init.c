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
	else if (obj->type == OBJ_CONE)
		return (obj->data.cone.center);
	else
		return (obj->data.plane.point);
}

/**
 * @brief Count the number of plane objects in the scene.
 *
 * @param scene Scene containing object list.
 * @return int Number of plane objects.
 */
static int	count_planes(t_scene *scene)
{
	int	i;
	int	count;

	count = 0;
	i = 0;
	while (i < scene->objects.count)
	{
		if (scene->objects.items[i].type == OBJ_PLANE)
			count++;
		i++;
	}
	return (count);
}

/**
 * @brief Separate plane indices and bounded object refs from scene.
 *
 * Fills bvh->plane_refs.indices with plane object indices,
 * and refs with non-plane (bounded) object references.
 *
 * @param scene Scene containing object list.
 * @param refs Output array for bounded object refs (may be NULL).
 */
static void	fill_separated_refs(t_scene *scene, t_object_ref *refs)
{
	int	i;
	int	bi;
	int	pi;

	bi = 0;
	pi = 0;
	i = 0;
	while (i < scene->objects.count)
	{
		if (scene->objects.items[i].type == OBJ_PLANE)
			scene->bvh->plane_refs.indices[pi++] = i;
		else
			refs[bi++].index = i;
		i++;
	}
}

/**
 * @brief Allocate plane indices and bounded object refs for BVH build.
 *
 * Frees existing plane_refs.indices before reallocating (R2 leak fix).
 * On malloc failure, cleans up partial allocations and returns 0 (R1).
 *
 * @param scene Scene with BVH to populate.
 * @param refs Output pointer for bounded object refs.
 * @param pc Plane count.
 * @param bc Bounded object count.
 * @return int 1 on success, 0 on allocation failure.
 */
static int	alloc_bvh_refs(t_scene *scene, t_object_ref **refs, int pc, int bc)
{
	free(scene->bvh->plane_refs.indices);
	scene->bvh->plane_refs.count = 0;
	scene->bvh->plane_refs.indices = NULL;
	if (pc > 0)
		scene->bvh->plane_refs.indices = malloc(sizeof(int) * pc);
	*refs = NULL;
	if (bc > 0)
		*refs = malloc(sizeof(t_object_ref) * bc);
	if (pc > 0 && !scene->bvh->plane_refs.indices)
		return (0);
	if (bc > 0 && !*refs)
	{
		free(scene->bvh->plane_refs.indices);
		scene->bvh->plane_refs.indices = NULL;
		return (0);
	}
	scene->bvh->plane_refs.count = pc;
	return (1);
}

/**
 * @brief Build or rebuild the BVH for the scene with plane separation.
 *
 * Separates planes from bounded objects, builds BVH with bounded
 * objects only, and stores plane indices for separate intersection.
 *
 * @param scene Scene containing objects and BVH state.
 */
void	scene_build_bvh(t_scene *scene)
{
	t_object_ref	*refs;
	int				pc;
	int				bc;

	if (!(scene->flags & SCENE_BVH_ENABLED) || scene->objects.count == 0)
		return ;
	if (!scene->bvh)
		scene->bvh = bvh_create();
	if (!scene->bvh)
		return ;
	pc = count_planes(scene);
	bc = scene->objects.count - pc;
	if (!alloc_bvh_refs(scene, &refs, pc, bc))
	{
		free(refs);
		return ;
	}
	fill_separated_refs(scene, refs);
	if (bc > 0)
		bvh_build(scene->bvh, refs, bc, scene);
	free(refs);
}
