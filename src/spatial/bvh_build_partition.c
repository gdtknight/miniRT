/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_build_partition.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:45:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 14:15:15 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "bvh_internal.h"

/**
 * @brief Compute the bounding box for a set of object references.
 *
 * Merges individual object bounds into a single AABB.
 *
 * @param objects Array of object references.
 * @param count Number of objects.
 * @param scene Pointer to the scene for bounds lookup.
 * @return t_aabb Combined bounds for the set.
 */
t_aabb	compute_bounds(t_object_ref *objects, int count, void *scene)
{
	t_aabb	bounds;
	t_aabb	obj_bounds;
	t_vec3	zero;
	int		i;

	zero.x = 0;
	zero.y = 0;
	zero.z = 0;
	if (count == 0)
		return (aabb_create(zero, zero));
	bounds = get_object_bounds(objects[0], scene);
	i = 1;
	while (i < count)
	{
		obj_bounds = get_object_bounds(objects[i], scene);
		bounds = aabb_merge(bounds, obj_bounds);
		i++;
	}
	return (bounds);
}

/**
 * @brief Partition objects around a split plane along an axis.
 *
 * Reorders the object array in-place and returns the split index.
 * When all objects share the same center on the split axis, the midpoint
 * fallback guarantees both children are non-empty (count >= 3 at this point).
 *
 * @param params Partition parameters.
 * @return int Index separating left/right partitions.
 */
int	partition_objects(t_partition_params *params)
{
	int				i;
	int				left_count;
	t_object_ref	temp;
	t_vec3			center;

	left_count = 0;
	i = 0;
	while (i < params->count)
	{
		center = get_object_center(params->objects[i], params->scene);
		if ((params->axis == 0 && center.x < params->split)
			|| (params->axis == 1 && center.y < params->split)
			|| (params->axis == 2 && center.z < params->split))
		{
			temp = params->objects[left_count];
			params->objects[left_count] = params->objects[i];
			params->objects[i] = temp;
			left_count++;
		}
		i++;
	}
	if (left_count == 0 || left_count == params->count)
		return (params->count / 2);
	return (left_count);
}
