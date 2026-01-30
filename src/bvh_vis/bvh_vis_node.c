/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_vis_node.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:11:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:58:19 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh_vis.h"
#include "minirt.h"
#include "window.h"
#include "utils.h"
#include <stdlib.h>

/**
 * @brief Check whether a BVH node is a leaf.
 *
 * @param node BVH node to test.
 * @return int 1 if leaf, 0 otherwise.
 */
int	is_leaf_node(t_bvh_node *node)
{
	return (node->left == NULL && node->right == NULL);
}

/**
 * @brief Append a formatted vec3 to a buffer.
 *
 * @param buf Destination buffer.
 * @param size Destination buffer size.
 * @param v Vector to append.
 * @param precision Decimal precision for components.
 */
static void	append_vec3_to_buf(char *buf, size_t size, t_vec3 v, int precision)
{
	char	num[32];

	float_to_str(num, 32, v.x, precision);
	ft_strlcat(buf, num, size);
	ft_strlcat(buf, ", ", size);
	float_to_str(num, 32, v.y, precision);
	ft_strlcat(buf, num, size);
	ft_strlcat(buf, ", ", size);
	float_to_str(num, 32, v.z, precision);
	ft_strlcat(buf, num, size);
}

/**
 * @brief Format an AABB bounds string in compact or verbose form.
 *
 * @param bounds AABB to format.
 * @param buf Destination buffer.
 * @param size Destination buffer size.
 * @param comp Non-zero for compact formatting.
 */
static void	format_bounds_str(t_aabb bounds, char *buf, size_t size, int comp)
{
	buf[0] = '\0';
	if (comp)
	{
		ft_strlcpy(buf, "[", size);
		append_vec3_to_buf(buf, size, bounds.min, 1);
		ft_strlcat(buf, "]-[", size);
		append_vec3_to_buf(buf, size, bounds.max, 1);
		ft_strlcat(buf, "]", size);
	}
	else
	{
		ft_strlcpy(buf, "min(", size);
		append_vec3_to_buf(buf, size, bounds.min, 2);
		ft_strlcat(buf, ") max(", size);
		append_vec3_to_buf(buf, size, bounds.max, 2);
		ft_strlcat(buf, ")", size);
	}
}

/**
 * @brief Build formatted node info for visualization.
 *
 * @param node BVH node to format.
 * @param config Visualization configuration (unused here).
 * @return t_node_info Populated node info struct.
 */
t_node_info	format_node_info(t_bvh_node *node, t_vis_config *config)
{
	t_node_info	info;

	(void)config;
	info.depth = node->depth;
	if (is_leaf_node(node))
		ft_strlcpy(info.type, "Leaf", sizeof(info.type));
	else
		ft_strlcpy(info.type, "Internal", sizeof(info.type));
	format_bounds_str(node->bounds, info.bounds, sizeof(info.bounds), 0);
	info.objects[0] = '\0';
	return (info);
}

/**
 * @brief Format a bounding box string for display.
 *
 * @param bounds AABB to format.
 * @param buffer Destination buffer.
 * @param compact Non-zero for compact format.
 */
void	format_bounding_box(t_aabb bounds, char *buffer, int compact)
{
	format_bounds_str(bounds, buffer, 128, compact);
}
