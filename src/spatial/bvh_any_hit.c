/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_any_hit.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/08 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "minirt.h"
#include "ray.h"
#include "metrics.h"

static void	get_near_far(t_bvh_node *node, t_ray ray,
		t_bvh_node **near, t_bvh_node **far)
{
	*near = node->left;
	*far = node->right;
	if (!ray_goes_positive(ray.direction, node->split_axis))
	{
		*near = node->right;
		*far = node->left;
	}
}

/**
 * @brief Check if ray hits any object in a leaf node (early exit).
 *
 * Returns immediately on first intersection found.
 *
 * @param node Leaf node containing object references.
 * @param ray Ray to test.
 * @param max_dist Maximum distance to consider.
 * @param scene Pointer to the scene.
 * @return int 1 if any hit found, 0 otherwise.
 */
static int	leaf_any_hit(t_bvh_node *node, t_ray ray, double max_dist,
		void *scene)
{
	int			i;
	t_hit		temp_hit;
	t_object	*obj;

	temp_hit.distance = max_dist;
	i = 0;
	while (i < node->object_count)
	{
		metrics_add_shadow_intersect(&((t_scene *)scene)->metrics);
		obj = &((t_scene *)scene)->objects.items[node->objects[i].index];
		if (intersect_object_new(&ray, obj, &temp_hit))
			return (1);
		i++;
	}
	return (0);
}

/**
 * @brief Recursively check BVH node for any intersection (early exit).
 *
 * Uses AABB test to skip nodes that the ray misses.
 *
 * @param node BVH node to test.
 * @param ray Ray to test.
 * @param max_dist Maximum distance to consider.
 * @param scene Pointer to the scene.
 * @return int 1 if any hit found, 0 otherwise.
 */
static int	node_any_hit(t_bvh_node *node, t_ray ray, double max_dist,
		void *scene)
{
	double		t_min;
	double		t_max;
	t_bvh_node	*near;
	t_bvh_node	*far;

	if (!node)
		return (0);
	metrics_add_bvh_node_visit(&((t_scene *)scene)->metrics);
	t_min = RAY_T_MIN;
	t_max = max_dist;
	if (!aabb_intersect(node->bounds, ray, &t_min, &t_max))
	{
		metrics_add_bvh_skip(&((t_scene *)scene)->metrics);
		return (0);
	}
	if (node->object_count > 0)
		return (leaf_any_hit(node, ray, max_dist, scene));
	get_near_far(node, ray, &near, &far);
	if (node_any_hit(near, ray, max_dist, scene))
		return (1);
	return (node_any_hit(far, ray, max_dist, scene));
}

/**
 * @brief Test if any object intersects the ray within max_dist (shadow query).
 *
 * Optimized for shadow rays: returns immediately on first hit found.
 * Uses BVH acceleration for spatial culling.
 *
 * @param bvh BVH structure.
 * @param ray Ray to test.
 * @param max_dist Maximum distance (typically distance to light).
 * @param scene Pointer to the scene.
 * @return int 1 if any intersection found, 0 if clear.
 */
int	bvh_intersect_any(t_bvh *bvh, t_ray ray, double max_dist, void *scene)
{
	if (!bvh || !bvh->root || !bvh->enabled)
		return (0);
	return (node_any_hit(bvh->root, ray, max_dist, scene));
}
