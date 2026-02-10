/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_vis_print.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:12:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/29 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh_vis.h"
#include "libft.h"
#include <stdio.h>

/**
 * @brief Print a single BVH node line with tree prefixes.
 *
 * @param prefix Prefix state to draw tree branches.
 * @param info Formatted node info.
 * @param is_last Non-zero if this node is the last sibling.
 */
void	print_node_line(t_prefix_state *prefix, t_node_info *info,
			int is_last)
{
	prefix_print(prefix);
	if (is_last)
		printf("└── ");
	else
		printf("├── ");
	printf("%s [depth=%d] %s", info->type, info->depth, info->bounds);
	if (info->objects[0] != '\0')
		printf(" %s", info->objects);
	printf("\n");
}

/**
 * @brief Print a warning message to stderr.
 *
 * @param message Warning text.
 */
void	print_warning_message(const char *message)
{
	ft_putstr_fd("Warning: ", 2);
	ft_putstr_fd((char *)message, 2);
	ft_putstr_fd("\n", 2);
}
