/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_vis_format.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/29 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh_vis.h"
#include "minirt.h"
#include <stdlib.h>

/**
 * @brief Format a list of object IDs into a buffer.
 *
 * @param objects Array of object references.
 * @param count Number of objects.
 * @param buffer Destination buffer (assumed large enough).
 * @param scene_ptr Pointer to the scene for ID lookup.
 */
void	format_object_list(t_object_ref *objects, int count, char *buffer,
			void *scene_ptr)
{
	int			i;
	char		*id;
	t_scene		*scene;
	t_object	*obj;

	scene = (t_scene *)scene_ptr;
	ft_strlcpy(buffer, "Objects: [", 64);
	i = 0;
	while (i < count)
	{
		obj = &scene->objects.items[objects[i].index];
		id = obj->id;
		if (id)
			ft_strlcat(buffer, id, 64);
		if (i < count - 1)
			ft_strlcat(buffer, ", ", 64);
		i++;
	}
	ft_strlcat(buffer, "]", 64);
}

/**
 * @brief Format node info in compact mode.
 *
 * Uses short type labels and depth-only bounds display.
 *
 * @param node BVH node to describe.
 * @param info Output node info structure.
 */
void	format_node_compact(t_bvh_node *node, t_node_info *info)
{
	char	*num;

	if (is_leaf_node(node))
		ft_strlcpy(info->type, "L", sizeof(info->type));
	else
		ft_strlcpy(info->type, "I", sizeof(info->type));
	ft_strlcpy(info->bounds, "[d=", sizeof(info->bounds));
	num = ft_itoa(node->depth);
	if (num)
	{
		ft_strlcat(info->bounds, num, sizeof(info->bounds));
		free(num);
	}
	ft_strlcat(info->bounds, "]", sizeof(info->bounds));
	info->objects[0] = '\0';
}
