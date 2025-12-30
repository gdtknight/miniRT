/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_build.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 21:30:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "minirt.h"
#include <stdlib.h>

t_bvh	*bvh_create(void)
{
	t_bvh	*bvh;

	bvh = malloc(sizeof(t_bvh));
	if (!bvh)
		return (NULL);
	bvh->root = NULL;
	bvh->enabled = 1;
	bvh->total_nodes = 0;
	bvh->max_depth = 0;
	return (bvh);
}

static void	bvh_node_destroy(t_bvh_node *node)
{
	if (!node)
		return ;
	bvh_node_destroy(node->left);
	bvh_node_destroy(node->right);
	if (node->objects)
		free(node->objects);
	free(node);
}

void	bvh_destroy(t_bvh *bvh)
{
	if (!bvh)
		return ;
	bvh_node_destroy(bvh->root);
	free(bvh);
}

static t_aabb	compute_bounds(t_object_ref *objects, int count, void *scene)
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

static int	partition_objects(t_object_ref *objects, int count, int axis,
		double split, void *scene)
{
	int				i;
	int				left_count;
	t_object_ref	temp;
	t_vec3			center;

	left_count = 0;
	i = 0;
	while (i < count)
	{
		center = get_object_center(objects[i], scene);
		if ((axis == 0 && center.x < split) || (axis == 1 && center.y < split)
			|| (axis == 2 && center.z < split))
		{
			temp = objects[left_count];
			objects[left_count] = objects[i];
			objects[i] = temp;
			left_count++;
		}
		i++;
	}
	if (left_count == 0 || left_count == count)
		return (count / 2);
	return (left_count);
}

static t_bvh_node	*create_leaf_node(t_object_ref *objects, int count,
		void *scene)
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

t_bvh_node	*bvh_build_recursive(t_object_ref *objects, int count,
		void *scene, int depth)
{
	t_bvh_node	*node;
	t_aabb		bounds;
	int			axis;
	double		split;
	int			mid;

	if (count <= 2 || depth > 20)
		return (create_leaf_node(objects, count, scene));
	bounds = compute_bounds(objects, count, scene);
	axis = 0;
	if (bounds.max.y - bounds.min.y > bounds.max.x - bounds.min.x)
		axis = 1;
	if (bounds.max.z - bounds.min.z > (axis == 0 ? bounds.max.x - bounds.min.x
			: bounds.max.y - bounds.min.y))
		axis = 2;
	split = (axis == 0 ? (bounds.min.x + bounds.max.x) / 2.0 : (axis == 1
				? (bounds.min.y + bounds.max.y) / 2.0 : (bounds.min.z
					+ bounds.max.z) / 2.0));
	mid = partition_objects(objects, count, axis, split, scene);
	node = malloc(sizeof(t_bvh_node));
	if (!node)
		return (NULL);
	node->bounds = bounds;
	node->objects = NULL;
	node->object_count = 0;
	node->left = bvh_build_recursive(objects, mid, scene, depth + 1);
	node->right = bvh_build_recursive(objects + mid, count - mid, scene,
			depth + 1);
	return (node);
}

void	bvh_build(t_bvh *bvh, t_object_ref *objects, int count, void *scene)
{
	if (!bvh || count == 0)
		return ;
	if (bvh->root)
		bvh_node_destroy(bvh->root);
	bvh->root = bvh_build_recursive(objects, count, scene, 0);
	bvh->total_nodes = 1;
}
