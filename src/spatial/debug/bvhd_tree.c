/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvhd_tree.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:13:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 15:13:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial/bvh_debug.h"
#include <stdio.h>

/**
 * @brief Format leaf objects if applicable and print a node.
 *
 * @param node BVH node to display.
 * @param ctx Traversal context for formatting.
 * @param info Node display information to populate.
 * @param is_last Non-zero if this is the last sibling.
 */
static void	handle_node_display(t_bvh_node *node, t_traverse_ctx *ctx,
				t_node_info *info, int is_last)
{
	if (bvhd_is_leaf(node))
		bvhd_format_objects(node->objects, node->object_count, info->objects,
			ctx->scene);
	bvhd_print_node(ctx->prefix, info, is_last);
}

/**
 * @brief Recursively traverse the BVH and print each node.
 *
 * Formats node info, prints tree lines with prefixes, and visits children
 * in depth-first order.
 *
 * @param node Current BVH node.
 * @param ctx Traversal context containing prefix and config.
 * @param is_last Non-zero if this node is the last sibling.
 */
static void	traverse_recursive(t_bvh_node *node, t_traverse_ctx *ctx,
				int is_last)
{
	t_node_info	info;

	if (!node)
		return ;
	info = bvhd_format_node(node);
	handle_node_display(node, ctx, &info, is_last);
	if (!bvhd_is_leaf(node))
	{
		if (node->left)
		{
			if (!bvhd_prefix_push(ctx->prefix, is_last))
				return ;
			traverse_recursive(node->left, ctx, !node->right);
			bvhd_prefix_pop(ctx->prefix);
		}
		if (node->right)
		{
			if (!bvhd_prefix_push(ctx->prefix, is_last))
				return ;
			traverse_recursive(node->right, ctx, 1);
			bvhd_prefix_pop(ctx->prefix);
		}
	}
}

/**
 * @brief Print the BVH tree and optional statistics summary.
 *
 * @param node Root node to visualize.
 * @param ctx Traversal context for formatting.
 * @param stats Optional statistics structure to fill and print.
 */
void	bvhd_print_tree(t_bvh_node *node, t_traverse_ctx *ctx,
			t_bvh_stats *stats)
{
	if (!node)
		return ;
	printf("\nBVH Tree Structure:\n");
	traverse_recursive(node, ctx, 1);
	if (stats)
	{
		bvhd_collect_stats(node, stats);
		printf("\n");
		bvhd_print_stats(stats);
	}
}
