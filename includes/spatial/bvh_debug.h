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

# include "spatial/spatial.h"

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
	t_scene			*scene;
}	t_traverse_ctx;

/* Public API */
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
void			bvhd_run(t_bvh *bvh, t_vis_config *config, t_scene *scene);

/**
 * @brief Build a default configuration for BVH visualization.
 *
 * @return Default visualization configuration.
 */
t_vis_config	bvhd_default_config(void);

/* Initialization and configuration */
/**
 * @brief Get the current terminal width in characters.
 *
 * Falls back to 80 columns if the query fails.
 *
 * @return Terminal width in columns.
 */
int				bvhd_get_terminal_width(void);

/**
 * @brief Check visualization edge cases and emit warnings.
 *
 * @param bvh BVH to visualize.
 * @param config Visualization configuration.
 */
void			bvhd_check_edges(t_bvh *bvh, t_vis_config *config);

/* Tree traversal and display */
/**
 * @brief Print the BVH tree and optional statistics summary.
 *
 * @param node Root node to visualize.
 * @param ctx Traversal context for formatting.
 * @param stats Optional statistics structure to fill and print.
 */
void			bvhd_print_tree(t_bvh_node *node, t_traverse_ctx *ctx,
					t_bvh_stats *stats);

/* Node formatting */
/**
 * @brief Build formatted node info for visualization.
 *
 * @param node BVH node to format.
 * @return Populated node info struct.
 */
t_node_info		bvhd_format_node(t_bvh_node *node);

/**
 * @brief Check whether a BVH node is a leaf.
 *
 * @param node BVH node to test.
 * @return 1 if leaf, 0 otherwise.
 */
int				bvhd_is_leaf(t_bvh_node *node);

/**
 * @brief Format a list of object IDs into a buffer.
 *
 * @param objects Array of object references.
 * @param count Number of objects.
 * @param buffer Destination buffer (assumed large enough).
 * @param scene Pointer to the scene for ID lookup.
 */
void			bvhd_format_objects(t_object_ref *objects, int count,
					char *buffer, t_scene *scene);

/* Statistics */
/**
 * @brief Initialize and compute BVH statistics from a tree.
 *
 * @param node Root node of the BVH.
 * @param stats Stats structure to populate.
 */
void			bvhd_collect_stats(t_bvh_node *node, t_bvh_stats *stats);

/**
 * @brief Recursively collect BVH statistics.
 *
 * Walks the tree to count nodes, leaves, and depth.
 *
 * @param node Current BVH node.
 * @param stats Stats structure to update.
 * @param depth Current traversal depth.
 */
void			bvhd_collect_recursive(t_bvh_node *node, t_bvh_stats *stats,
					int depth);

/**
 * @brief Print a summary of BVH statistics.
 *
 * @param stats Stats structure to print.
 */
void			bvhd_print_stats(t_bvh_stats *stats);

/* Prefix management */
/**
 * @brief Initialize the prefix state used for tree printing.
 *
 * Allocates a buffer and resets prefix metadata.
 *
 * @return Initialized prefix state.
 */
t_prefix_state	bvhd_prefix_init(void);

/**
 * @brief Append a new prefix segment for a child node.
 *
 * Expands the buffer if needed and writes either a branch or space
 * segment.
 *
 * @param state Prefix state to update.
 * @param is_last Non-zero if the current node is the last sibling.
 * @return 1 on success, 0 on allocation failure.
 */
int				bvhd_prefix_push(t_prefix_state *state, int is_last);

/**
 * @brief Remove the last prefix segment.
 *
 * @param state Prefix state to update.
 */
void			bvhd_prefix_pop(t_prefix_state *state);

/**
 * @brief Print the current prefix to stdout.
 *
 * @param state Prefix state to print.
 */
void			bvhd_prefix_print(t_prefix_state *state);

/**
 * @brief Free prefix buffer and reset state.
 *
 * @param state Prefix state to destroy.
 */
void			bvhd_prefix_destroy(t_prefix_state *state);

/* Output primitives */
/**
 * @brief Print a single BVH node line with tree prefixes.
 *
 * @param prefix Prefix state to draw tree branches.
 * @param info Formatted node info.
 * @param is_last Non-zero if this node is the last sibling.
 */
void			bvhd_print_node(t_prefix_state *prefix, t_node_info *info,
					int is_last);

/**
 * @brief Print a warning message to stderr.
 *
 * @param message Warning text.
 */
void			bvhd_warn(const char *message);

#endif
