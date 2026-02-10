/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_build_split.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:45:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 14:15:10 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "bvh_internal.h"
#include <stdlib.h>

/**
 * @brief Choose the split axis with the largest extent.
 *
 * @param bounds Bounds of the current object set.
 * @return int Axis index (0=x, 1=y, 2=z).
 */
int	choose_split_axis(t_aabb bounds)
{
	int		axis;
	double	max_extent;

	axis = 0;
	if (bounds.max.y - bounds.min.y > bounds.max.x - bounds.min.x)
		axis = 1;
	max_extent = bounds.max.x - bounds.min.x;
	if (axis == 1)
		max_extent = bounds.max.y - bounds.min.y;
	if (bounds.max.z - bounds.min.z > max_extent)
		axis = 2;
	return (axis);
}

/**
 * @brief Calculate split position at the center of bounds along an axis.
 *
 * @param bounds Bounds of the current object set.
 * @param axis Axis index (0=x, 1=y, 2=z).
 * @return double Split coordinate.
 */
double	calculate_split_position(t_aabb bounds, int axis)
{
	if (axis == 0)
		return ((bounds.min.x + bounds.max.x) / 2.0);
	else if (axis == 1)
		return ((bounds.min.y + bounds.max.y) / 2.0);
	else
		return ((bounds.min.z + bounds.max.z) / 2.0);
}

/**
 * @brief Create an internal BVH node and build child subtrees.
 *
 * @param sp Split parameters including object array and bounds.
 * @return t_bvh_node* Newly allocated internal node or NULL on failure.
 */
t_bvh_node	*create_split_node(t_split_params *sp)
{
	t_bvh_node	*node;

	node = malloc(sizeof(t_bvh_node));
	if (!node)
		return (NULL);
	node->bounds = sp->bounds;
	node->objects = NULL;
	node->object_count = 0;
	node->depth = sp->depth;
	node->split_axis = sp->axis;
	node->left = bvh_build_recursive(sp->objects, sp->mid,
			sp->scene, sp->depth + 1);
	node->right = bvh_build_recursive(sp->objects + sp->mid,
			sp->count - sp->mid, sp->scene, sp->depth + 1);
	if (node->left)
		node->left->depth = sp->depth + 1;
	if (node->right)
		node->right->depth = sp->depth + 1;
	return (node);
}
