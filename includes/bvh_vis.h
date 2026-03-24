/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_vis.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:09:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 15:09:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BVH_VIS_H
# define BVH_VIS_H

# include "spatial.h"

/* Prefix segment length (bytes per tree-drawing indent level) */
# define PREFIX_SEGMENT_LEN 6

/* Visualization configuration */
typedef struct s_vis_config
{
	int		max_depth_display;
	int		compact_mode;
	int		terminal_width;
	int		show_warnings;
}	t_vis_config;

/* Prefix state for tree drawing */
typedef struct s_prefix_state
{
	char	*buffer;
	int		capacity;
	int		length;
	int		level;
}	t_prefix_state;

/* BVH statistics */
typedef struct s_bvh_stats
{
	int		total_nodes;
	int		leaf_count;
	int		internal_count;
	int		max_depth;
	int		total_objects;
	double	avg_objects_per_leaf;
}	t_bvh_stats;

/* Node display information */
typedef struct s_node_info
{
	char	type[16];
	char	bounds[128];
	char	objects[64];
	int		depth;
}	t_node_info;

/* Traversal context to avoid too many arguments */
typedef struct s_traverse_ctx
{
	t_prefix_state	*prefix;
	t_vis_config	*config;
	void			*scene;
}	t_traverse_ctx;

/* Public API */
void			bvh_visualize(t_bvh *bvh, t_vis_config *config, void *scene);
t_vis_config	bvh_vis_default_config(void);

/* Initialization and configuration */
int				bvh_vis_get_terminal_width(void);
void			check_edge_cases(t_bvh *bvh, t_vis_config *config);

/* Tree traversal and display */
void			bvh_visualize_tree(t_bvh_node *node, t_traverse_ctx *ctx,
					t_bvh_stats *stats);

/* Node formatting */
t_node_info		format_node_info(t_bvh_node *node);
int				is_leaf_node(t_bvh_node *node);
void			format_object_list(t_object_ref *objects, int count,
					char *buffer, void *scene);

/* Statistics */
void			bvh_collect_statistics(t_bvh_node *node, t_bvh_stats *stats);
void			collect_stats_recursive(t_bvh_node *node, t_bvh_stats *stats,
					int depth);
void			print_statistics_summary(t_bvh_stats *stats);

/* Prefix management */
t_prefix_state	prefix_init(void);
int				prefix_push(t_prefix_state *state, int is_last);
void			prefix_pop(t_prefix_state *state);
void			prefix_print(t_prefix_state *state);
void			prefix_destroy(t_prefix_state *state);

/* Output primitives */
void			print_node_line(t_prefix_state *prefix, t_node_info *info,
					int is_last);
void			print_warning_message(const char *message);

#endif
