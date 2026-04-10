/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_internal.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:50:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 18:50:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BVH_INTERNAL_H
# define BVH_INTERNAL_H

# include "spatial/spatial.h"

/* Internal BVH lifecycle functions */
/**
 * @brief Recursively destroy a BVH node and its children.
 *
 * Frees object references and all descendant nodes.
 *
 * @param node Node to destroy.
 */
void		bvh_node_destroy(t_bvh_node *node);

/* Internal BVH build partition functions */
/**
 * @brief Compute the bounding box for a set of object references.
 *
 * Merges individual object bounds into a single AABB.
 *
 * @param objects Array of object references.
 * @param count Number of objects.
 * @param scene Pointer to the scene for bounds lookup.
 * @return Combined bounds for the set.
 */
t_aabb		compute_bounds(t_object_ref *objects, int count, t_scene *scene);

/**
 * @brief Partition objects around a split plane along an axis.
 *
 * Reorders the object array in-place and returns the split index.
 * Falls back to midpoint when all objects share the same center.
 *
 * @param params Partition parameters.
 * @return Index separating left/right partitions.
 */
int			partition_objects(t_partition_params *params);

/* Internal BVH build split functions */
/**
 * @brief Choose the split axis with the largest extent.
 *
 * @param bounds Bounds of the current object set.
 * @return Axis index (0=x, 1=y, 2=z).
 */
int			choose_split_axis(t_aabb bounds);

/**
 * @brief Calculate split position at the center of bounds along an axis.
 *
 * @param bounds Bounds of the current object set.
 * @param axis Axis index (0=x, 1=y, 2=z).
 * @return Split coordinate.
 */
double		calculate_split_position(t_aabb bounds, int axis);

/**
 * @brief Create an internal BVH node and build child subtrees.
 *
 * @param sp Split parameters including object array and bounds.
 * @return Newly allocated internal node or NULL on failure.
 */
t_bvh_node	*create_split_node(t_split_params *sp);

/* Internal BVH build core functions */
/**
 * @brief Create a leaf BVH node containing object references.
 *
 * Allocates the node, computes bounds, and copies object references.
 *
 * @param objects Array of object references.
 * @param count Number of objects in the leaf.
 * @param scene Pointer to the scene for bounds computation.
 * @return Newly allocated leaf node or NULL on failure.
 */
t_bvh_node	*create_leaf_node(t_object_ref *objects, int count, t_scene *scene);

#endif
