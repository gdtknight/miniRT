/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvhd_stats.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:14:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 15:14:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial/bvh_debug.h"
#include <stdio.h>

/**
 * @brief Recursively collect BVH statistics.
 *
 * Walks the tree to count nodes, leaves, and depth.
 *
 * @param node Current BVH node.
 * @param stats Stats structure to update.
 * @param depth Current traversal depth.
 */
void	bvhd_collect_recursive(t_bvh_node *node, t_bvh_stats *stats,
			int depth)
{
	if (!node || !stats)
		return ;
	stats->total_nodes++;
	if (depth > stats->max_depth)
		stats->max_depth = depth;
	if (bvhd_is_leaf(node))
	{
		stats->leaf_count++;
		stats->total_objects += node->object_count;
	}
	else
	{
		if (node->left)
			bvhd_collect_recursive(node->left, stats, depth + 1);
		if (node->right)
			bvhd_collect_recursive(node->right, stats, depth + 1);
	}
}

/**
 * @brief Initialize and compute BVH statistics from a tree.
 *
 * @param node Root node of the BVH.
 * @param stats Stats structure to populate.
 */
void	bvhd_collect_stats(t_bvh_node *node, t_bvh_stats *stats)
{
	if (!stats)
		return ;
	stats->total_nodes = 0;
	stats->leaf_count = 0;
	stats->internal_count = 0;
	stats->max_depth = 0;
	stats->total_objects = 0;
	stats->avg_objects_per_leaf = 0.0;
	bvhd_collect_recursive(node, stats, 0);
	stats->internal_count = stats->total_nodes - stats->leaf_count;
	if (stats->leaf_count > 0)
		stats->avg_objects_per_leaf = (double)stats->total_objects
			/ stats->leaf_count;
}

/**
 * @brief Print a summary of BVH statistics.
 *
 * @param stats Stats structure to print.
 */
void	bvhd_print_stats(t_bvh_stats *stats)
{
	if (!stats)
		return ;
	printf("BVH Statistics:\n");
	printf("  Total nodes: %d\n", stats->total_nodes);
	printf("  Leaf nodes: %d\n", stats->leaf_count);
	printf("  Internal nodes: %d\n", stats->internal_count);
	printf("  Max depth: %d\n", stats->max_depth);
	printf("  Average objects per leaf: %.2f\n", stats->avg_objects_per_leaf);
}
