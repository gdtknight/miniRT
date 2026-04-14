/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvhd_init.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:15:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 14:15:45 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial/bvhd_internal.h"

/**
 * @brief Visualize the BVH tree in the terminal.
 *
 * @param bvh BVH to visualize.
 * @param scene Scene for object label lookup.
 */
void	bvhd_run(t_bvh *bvh, t_scene *scene)
{
	t_prefix_state	prefix;
	t_bvh_stats		stats;
	t_traverse_ctx	ctx;

	if (!bvh || !bvh->root || !bvh->visualize)
		return ;
	prefix = bvhd_prefix_init();
	if (!prefix.buffer)
		return ;
	ctx.prefix = &prefix;
	ctx.scene = scene;
	bvhd_print_tree(bvh->root, &ctx, &stats);
	bvhd_prefix_destroy(&prefix);
}
