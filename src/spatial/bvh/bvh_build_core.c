/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_build_core.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:45:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 14:15:19 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial/spatial.h"
#include "spatial/bvh_internal.h"
#include <stdlib.h>

/**
 * @brief Create a leaf BVH node containing object references.
 *
 * Allocates the node, computes bounds, and copies object references.
 *
 * @param objects Array of object references.
 * @param count Number of objects in the leaf.
 * @param scene Pointer to the scene for bounds computation.
 * @return t_bvh_node* Newly allocated leaf node or NULL on failure.
 */
static t_bvh_node	*create_leaf_node(t_object_ref *objects, int count,
		t_scene *scene)
{
	t_bvh_node	*node;
	int			i;

	node = malloc(sizeof(t_bvh_node));
	if (!node)
		return (NULL);
	node->bounds = compute_bounds(objects, count, scene);
	node->left = NULL;
	node->right = NULL;
	node->object_count = count;
	node->depth = 0;
	node->split_axis = -1;
	node->objects = malloc(sizeof(t_object_ref) * count);
	if (!node->objects)
	{
		free(node);
		return (NULL);
	}
	i = 0;
	while (i < count)
	{
		node->objects[i] = objects[i];
		i++;
	}
	return (node);
}

/**
 * @brief Recursively build a BVH subtree.
 *
 * Splits objects along the best axis until leaf criteria are met.
 *
 * @param objects Array of object references.
 * @param count Number of objects in the current subset.
 * @param scene Pointer to the scene for bounds computation.
 * @param depth Current recursion depth.
 * @return t_bvh_node* Root node of the constructed subtree.
 */
t_bvh_node	*bvh_build_recursive(t_object_ref *objects, int count,
		t_scene *scene, int depth)
{
	t_aabb				bounds;
	int					axis;
	t_partition_params	params;
	t_split_params		sp;

	if (count <= 2 || depth > 20)
		return (create_leaf_node(objects, count, scene));
	bounds = compute_bounds(objects, count, scene);
	axis = choose_split_axis(bounds);
	params.objects = objects;
	params.count = count;
	params.axis = axis;
	params.split = calculate_split_position(bounds, axis);
	params.scene = scene;
	sp.mid = partition_objects(&params);
	sp.bounds = bounds;
	sp.objects = objects;
	sp.count = count;
	sp.scene = scene;
	sp.depth = depth;
	sp.axis = axis;
	return (create_split_node(&sp));
}

/**
 * @brief Build or rebuild the BVH from object references.
 *
 * Destroys any existing BVH tree and constructs a new one.
 *
 * @param bvh BVH structure to populate.
 * @param objects Array of object references.
 * @param count Number of objects.
 * @param scene Pointer to the scene for bounds computation.
 */
void	bvh_build(t_bvh *bvh, t_object_ref *objects, int count, t_scene *scene)
{
	if (!bvh || count == 0)
		return ;
	if (bvh->root)
		bvh_node_destroy(bvh->root);
	bvh->root = bvh_build_recursive(objects, count, scene, 0);
	if (bvh->root)
		bvh->root->depth = 0;
}
