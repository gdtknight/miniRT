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

#include "spatial.h"
#include "minirt.h"
#include "ray.h"
#include "metrics.h"

/**
 * @brief Intersect a ray with an object reference.
 *
 * Resolves the object from the scene and dispatches to the intersection
 * routine.
 *
 * @param ref Object reference containing index.
 * @param ray Ray to test.
 * @param hit Hit record to update.
 * @param scene_ptr Pointer to the scene.
 * @return int 1 if hit, 0 otherwise.
 */
static int	intersect_ref(t_object_ref ref, t_ray ray, t_hit_record *hit,
		void *scene_ptr)
{
	t_scene		*scene;
	t_object	*obj;

	scene = (t_scene *)scene_ptr;
	obj = &scene->objects.items[ref.index];
	return (intersect_object_new(&ray, obj, hit));
}

/**
 * @brief Intersect a ray with all objects in a BVH leaf node.
 *
 * Tracks the nearest hit among the node's object references.
 *
 * @param node Leaf node containing object references.
 * @param ray Ray to test.
 * @param hit Output hit record.
 * @param scene Pointer to the scene.
 * @return int 1 if any hit is found, 0 otherwise.
 */
static int	bvh_leaf_intersect(t_bvh_node *node, t_ray ray, t_hit_record *hit,
		void *scene)
{
	int				i;
	int				hit_anything;
	t_hit_record	temp_hit;

	temp_hit.distance = hit->distance;
	hit_anything = 0;
	i = 0;
	while (i < node->object_count)
	{
		metrics_add_intersect_test(&((t_scene *)scene)->metrics);
		if (intersect_ref(node->objects[i], ray, &temp_hit, scene))
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
 * @brief Choose the closest hit among left and right child results.
 *
 * @param hc Hit check context with child hit results.
 * @return int 1 if any child hit is valid, 0 otherwise.
 */
static int	check_child_hits(t_hit_check *hc)
{
	if (hc->hit_left && hc->hit_right)
	{
		if (hc->left_hit->distance < hc->right_hit->distance)
			*hc->hit = *hc->left_hit;
		else
			*hc->hit = *hc->right_hit;
		return (1);
	}
	if (hc->hit_left)
	{
		*hc->hit = *hc->left_hit;
		return (1);
	}
	if (hc->hit_right)
	{
		*hc->hit = *hc->right_hit;
		return (1);
	}
	return (0);
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
int	bvh_node_intersect(t_bvh_node *node, t_ray ray, t_hit_record *hit,
		void *scene)
{
	double			t_min;
	double			t_max;
	t_hit_record	left_hit;
	t_hit_record	right_hit;
	t_hit_check		hc;

	if (!node)
		return (0);
	metrics_add_bvh_node_visit(&((t_scene *)scene)->metrics);
	t_min = 0.001;
	t_max = 1000000.0;
	metrics_add_bvh_box_test(&((t_scene *)scene)->metrics);
	if (!aabb_intersect(node->bounds, ray, &t_min, &t_max))
		return (0);
	if (node->object_count > 0)
		return (bvh_leaf_intersect(node, ray, hit, scene));
	left_hit.distance = hit->distance;
	right_hit.distance = hit->distance;
	hc.hit_left = bvh_node_intersect(node->left, ray, &left_hit, scene);
	hc.hit_right = bvh_node_intersect(node->right, ray, &right_hit, scene);
	hc.left_hit = &left_hit;
	hc.right_hit = &right_hit;
	hc.hit = hit;
	return (check_child_hits(&hc));
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
int	bvh_intersect(t_bvh *bvh, t_ray ray, t_hit_record *hit, void *scene)
{
	if (!bvh || !bvh->root || !bvh->enabled)
		return (0);
	return (bvh_node_intersect(bvh->root, ray, hit, scene));
}
