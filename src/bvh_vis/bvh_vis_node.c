/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_vis_node.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:11:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh_vis.h"
#include "minirt.h"
#include "window.h"
#include <stdio.h>
#include <string.h>

int	is_leaf_node(t_bvh_node *node)
{
	return (node->left == NULL && node->right == NULL);
}

t_node_info	format_node_info(t_bvh_node *node, t_vis_config *config)
{
	t_node_info	info;

	(void)config;
	info.depth = node->depth;
	if (is_leaf_node(node))
		strcpy(info.type, "Leaf");
	else
		strcpy(info.type, "Internal");
	snprintf(info.bounds, sizeof(info.bounds),
		"min(%.2f, %.2f, %.2f) max(%.2f, %.2f, %.2f)",
		node->bounds.min.x, node->bounds.min.y, node->bounds.min.z,
		node->bounds.max.x, node->bounds.max.y, node->bounds.max.z);
	info.objects[0] = '\0';
	return (info);
}

void	format_bounding_box(t_aabb bounds, char *buffer, int compact)
{
	if (compact)
		snprintf(buffer, 128, "[%.1f,%.1f,%.1f]-[%.1f,%.1f,%.1f]",
			bounds.min.x, bounds.min.y, bounds.min.z,
			bounds.max.x, bounds.max.y, bounds.max.z);
	else
		snprintf(buffer, 128, "min(%.2f, %.2f, %.2f) max(%.2f, %.2f, %.2f)",
			bounds.min.x, bounds.min.y, bounds.min.z,
			bounds.max.x, bounds.max.y, bounds.max.z);
}

void	format_object_list(t_object_ref *objects, int count, char *buffer,
			void *scene_ptr)
{
	int			i;
	char		*id;
	t_scene		*scene;
	t_object	*obj;

	scene = (t_scene *)scene_ptr;
	strcpy(buffer, "Objects: [");
	i = 0;
	while (i < count)
	{
		obj = &scene->objects.items[objects[i].index];
		id = obj->id;
		if (id)
			strcat(buffer, id);
		if (i < count - 1)
			strcat(buffer, ", ");
		i++;
	}
	strcat(buffer, "]");
}

void	format_node_compact(t_bvh_node *node, t_node_info *info)
{
	if (is_leaf_node(node))
		strcpy(info->type, "L");
	else
		strcpy(info->type, "I");
	snprintf(info->bounds, sizeof(info->bounds), "[d=%d]", node->depth);
	info->objects[0] = '\0';
}
