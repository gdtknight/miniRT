/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_debug.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:09:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 15:09:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BVH_DEBUG_H
# define BVH_DEBUG_H

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
void			bvhd_run(t_bvh *bvh, t_vis_config *config, void *scene);
t_vis_config	bvhd_default_config(void);

/* Initialization and configuration */
int				bvhd_get_terminal_width(void);
void			bvhd_check_edges(t_bvh *bvh, t_vis_config *config);

/* Tree traversal and display */
void			bvhd_print_tree(t_bvh_node *node, t_traverse_ctx *ctx,
					t_bvh_stats *stats);

/* Node formatting */
t_node_info		bvhd_format_node(t_bvh_node *node);
int				bvhd_is_leaf(t_bvh_node *node);
void			bvhd_format_objects(t_object_ref *objects, int count,
					char *buffer, void *scene);

/* Statistics */
void			bvhd_collect_stats(t_bvh_node *node, t_bvh_stats *stats);
void			bvhd_collect_recursive(t_bvh_node *node, t_bvh_stats *stats,
					int depth);
void			bvhd_print_stats(t_bvh_stats *stats);

/* Prefix management */
t_prefix_state	bvhd_prefix_init(void);
int				bvhd_prefix_push(t_prefix_state *state, int is_last);
void			bvhd_prefix_pop(t_prefix_state *state);
void			bvhd_prefix_print(t_prefix_state *state);
void			bvhd_prefix_destroy(t_prefix_state *state);

/* Output primitives */
void			bvhd_print_node(t_prefix_state *prefix, t_node_info *info,
					int is_last);
void			bvhd_warn(const char *message);

#endif
