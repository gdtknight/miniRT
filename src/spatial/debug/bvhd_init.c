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

#include "bvhd_internal.h"
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * @brief Get the current terminal width in characters.
 *
 * @return Terminal width in columns, defaults to 80.
 */
static int	get_terminal_width(void)
{
	struct winsize	ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
		return (80);
	if (ws.ws_col == 0)
		return (80);
	return (ws.ws_col);
}

/**
 * @brief Build a default visualization configuration.
 *
 * @return Default configuration with current terminal width.
 */
static t_vis_config	init_default_config(void)
{
	t_vis_config	config;

	config.max_depth_display = -1;
	config.compact_mode = 0;
	config.terminal_width = get_terminal_width();
	config.show_warnings = 1;
	return (config);
}

/**
 * @brief Warn about narrow terminal if applicable.
 *
 * @param config Visualization configuration.
 */
static void	check_edges(t_vis_config *config)
{
	if (config->terminal_width < 80 && config->show_warnings)
		bvhd_warn("Terminal width < 80 chars, output may wrap");
}

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
	t_vis_config	config;
	t_traverse_ctx	ctx;

	if (!bvh || !bvh->root || !bvh->visualize)
		return ;
	config = init_default_config();
	check_edges(&config);
	prefix = bvhd_prefix_init();
	if (!prefix.buffer)
		return ;
	ctx.prefix = &prefix;
	ctx.scene = scene;
	bvhd_print_tree(bvh->root, &ctx, &stats);
	bvhd_prefix_destroy(&prefix);
}
