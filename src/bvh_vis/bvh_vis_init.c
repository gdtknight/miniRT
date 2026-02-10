/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_vis_init.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:15:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 14:15:45 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh_vis.h"
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * @brief Build a default configuration for BVH visualization.
 *
 * Initializes display options and captures current terminal width.
 *
 * @return t_vis_config Default visualization configuration.
 */
t_vis_config	bvh_vis_default_config(void)
{
	t_vis_config	config;

	config.max_depth_display = -1;
	config.compact_mode = 0;
	config.terminal_width = bvh_vis_get_terminal_width();
	config.show_warnings = 1;
	return (config);
}

/**
 * @brief Get the current terminal width in characters.
 *
 * Falls back to 80 columns if the query fails.
 *
 * @return int Terminal width in columns.
 */
int	bvh_vis_get_terminal_width(void)
{
	struct winsize	ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
		return (80);
	if (ws.ws_col == 0)
		return (80);
	return (ws.ws_col);
}

/**
 * @brief Check visualization edge cases and emit warnings.
 *
 * Warns about deep trees or narrow terminals when enabled.
 *
 * @param bvh BVH to visualize.
 * @param config Visualization configuration.
 */
void	check_edge_cases(t_bvh *bvh, t_vis_config *config)
{
	if (!bvh || !bvh->root || !config)
		return ;
	if (bvh->max_depth > 20)
	{
		if (config->show_warnings)
			print_warning_message("BVH tree is very deep (>20 levels)");
	}
	if (config->terminal_width < 80)
	{
		if (config->show_warnings)
			print_warning_message("Terminal width < 80 chars, output may wrap");
	}
}

/**
 * @brief Visualize the BVH tree in the terminal.
 *
 * Applies defaults when configuration is NULL and prints the tree plus
 * statistics if enabled.
 *
 * @param bvh BVH to visualize.
 * @param config Visualization configuration (optional).
 * @param scene Pointer to the scene used for object labels.
 */
void	bvh_visualize(t_bvh *bvh, t_vis_config *config, void *scene)
{
	t_prefix_state	prefix;
	t_bvh_stats		stats;
	t_vis_config	default_config;
	t_traverse_ctx	ctx;

	if (!bvh || !bvh->root || !bvh->visualize)
		return ;
	if (!config)
	{
		default_config = bvh_vis_default_config();
		config = &default_config;
	}
	check_edge_cases(bvh, config);
	stats.total_nodes = 0;
	stats.leaf_count = 0;
	stats.internal_count = 0;
	stats.max_depth = 0;
	stats.total_objects = 0;
	stats.avg_objects_per_leaf = 0.0;
	prefix = prefix_init();
	ctx.prefix = &prefix;
	ctx.config = config;
	ctx.scene = scene;
	bvh_visualize_tree(bvh->root, &ctx, &stats);
	prefix_destroy(&prefix);
}
