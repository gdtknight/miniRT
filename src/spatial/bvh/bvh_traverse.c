/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_traverse.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial/spatial.h"
#include "scene/scene.h"
#include "spatial/intersect.h"
#include "metrics.h"

int	ray_goes_positive(t_vec3 dir, int axis)
{
	if (axis == 0)
		return (dir.x > 0);
	if (axis == 1)
		return (dir.y > 0);
	return (dir.z > 0);
}

/**
 * @brief Intersect a ray with all objects in a BVH leaf node.
 *
 * Resolves each object reference, tests intersection, and tracks the
 * nearest hit among the node's objects.
 *
 * @param node Leaf node containing object references.
 * @param ray Ray to test.
 * @param hit Output hit record.
 * @param scene Pointer to the scene.
 * @return int 1 if any hit is found, 0 otherwise.
 */
static int	bvh_leaf_intersect(t_bvh_node *node, t_ray ray, t_hit *hit,
		t_scene *scene)
{
	int			i;
	int			hit_anything;
	t_hit		temp_hit;
	t_object	*obj;

	temp_hit.distance = hit->distance;
	hit_anything = 0;
	i = 0;
	while (i < node->object_count)
	{
		metrics_add_intersect_test(&(scene)->metrics);
		obj = &(scene)->objects.items[node->objects[i].index];
		if (intersect_object(&ray, obj, &temp_hit))
		{
			if (!hit_anything || temp_hit.distance < hit->distance)
			{
				*hit = temp_hit;
				hit_anything = 1;
			}
		}
		i++;
	}
	return (hit_anything);
}

/**
 * @brief Recurse into children with near/far ordering for early pruning.
 *
 * Visits the child in the ray's direction first for better pruning.
 *
 * @param node Internal BVH node with left/right children.
 * @param ray Ray to test.
 * @param hit Output hit record.
 * @param scene Pointer to the scene.
 * @return int 1 if any child hit is found, 0 otherwise.
 */
static int	traverse_children(t_bvh_node *node, t_ray ray, t_hit *hit,
		t_scene *scene)
{
	t_bvh_node	*near;
	t_bvh_node	*far;
	t_hit		th;
	int			ret;

	near = node->left;
	far = node->right;
	if (!ray_goes_positive(ray.direction, node->split_axis))
	{
		near = node->right;
		far = node->left;
	}
	th.distance = hit->distance;
	ret = bvh_node_intersect(near, ray, &th, scene);
	if (ret)
		*hit = th;
	th.distance = hit->distance;
	if (bvh_node_intersect(far, ray, &th, scene) && th.distance < hit->distance)
	{
		*hit = th;
		ret = 1;
	}
	return (ret);
}

/**
 * @brief Traverse a BVH node to test for ray intersections.
 *
 * Performs AABB test, descends into children, and selects the closest hit.
 *
 * @param node BVH node to test.
 * @param ray Ray to test.
 * @param hit Output hit record.
 * @param scene Pointer to the scene.
 * @return int 1 if any hit is found, 0 otherwise.
 */
int	bvh_node_intersect(t_bvh_node *node, t_ray ray, t_hit *hit,
		t_scene *scene)
{
	double	t_min;
	double	t_max;

	if (!node)
		return (0);
	metrics_add_bvh_node_visit(&(scene)->metrics);
	t_min = RAY_T_MIN;
	t_max = hit->distance;
	if (!aabb_intersect(node->bounds, ray, &t_min, &t_max))
	{
		metrics_add_bvh_skip(&(scene)->metrics);
		return (0);
	}
	if (node->object_count > 0)
		return (bvh_leaf_intersect(node, ray, hit, scene));
	return (traverse_children(node, ray, hit, scene));
}

/**
 * @brief Intersect a ray against the BVH root if enabled.
 *
 * @param bvh BVH structure.
 * @param ray Ray to test.
 * @param hit Output hit record.
 * @param scene Pointer to the scene.
 * @return int 1 if any hit is found, 0 otherwise.
 */
int	bvh_intersect(t_bvh *bvh, t_ray ray, t_hit *hit, t_scene *scene)
{
	if (!bvh || !bvh->root || !bvh->enabled)
		return (0);
	return (bvh_node_intersect(bvh->root, ray, hit, scene));
}
