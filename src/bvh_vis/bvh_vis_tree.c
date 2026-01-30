/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_vis_tree.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:13:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 15:13:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh_vis.h"
#include <stdio.h>

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
	info = format_node_info(node, ctx->config);
	if (is_leaf_node(node))
		format_object_list(node->objects, node->object_count, info.objects,
			ctx->scene);
	print_node_line(ctx->prefix, &info, is_last);
	if (!is_leaf_node(node))
	{
		if (node->left)
		{
			prefix_push(ctx->prefix, is_last);
			traverse_recursive(node->left, ctx, !node->right);
			prefix_pop(ctx->prefix);
		}
		if (node->right)
		{
			prefix_push(ctx->prefix, is_last);
			traverse_recursive(node->right, ctx, 1);
			prefix_pop(ctx->prefix);
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
void	bvh_visualize_tree(t_bvh_node *node, t_traverse_ctx *ctx,
			t_bvh_stats *stats)
{
	if (!node)
		return ;
	printf("\nBVH Tree Structure:\n");
	traverse_recursive(node, ctx, 1);
	if (stats)
	{
		bvh_collect_statistics(node, stats);
		printf("\n");
		print_statistics_summary(stats);
	}
}
