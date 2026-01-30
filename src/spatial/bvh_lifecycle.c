/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_lifecycle.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:45:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 14:15:06 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "bvh_internal.h"
#include <stdlib.h>

/**
 * @brief Allocate and initialize a BVH structure.
 *
 * @return t_bvh* Newly allocated BVH or NULL on failure.
 */
t_bvh	*bvh_create(void)
{
	t_bvh	*bvh;

	bvh = malloc(sizeof(t_bvh));
	if (!bvh)
		return (NULL);
	bvh->root = NULL;
	bvh->enabled = 1;
	bvh->total_nodes = 0;
	bvh->max_depth = 0;
	bvh->visualize = 0;
	return (bvh);
}

/**
 * @brief Recursively destroy a BVH node and its children.
 *
 * Frees object references and all descendant nodes.
 *
 * @param node Node to destroy.
 */
void	bvh_node_destroy(t_bvh_node *node)
{
	if (!node)
		return ;
	bvh_node_destroy(node->left);
	bvh_node_destroy(node->right);
	if (node->objects)
		free(node->objects);
	free(node);
}

/**
 * @brief Destroy a BVH and free all resources.
 *
 * @param bvh BVH to destroy.
 */
void	bvh_destroy(t_bvh *bvh)
{
	if (!bvh)
		return ;
	bvh_node_destroy(bvh->root);
	free(bvh);
}
