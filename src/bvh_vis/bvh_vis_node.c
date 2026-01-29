/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_vis_node.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:11:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/29 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh_vis.h"
#include "minirt.h"
#include "window.h"
#include "utils.h"
#include <stdlib.h>

int	is_leaf_node(t_bvh_node *node)
{
	return (node->left == NULL && node->right == NULL);
}

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

void	format_bounding_box(t_aabb bounds, char *buffer, int compact)
{
	format_bounds_str(bounds, buffer, 128, compact);
}

void	format_object_list(t_object_ref *objects, int count, char *buffer,
			void *scene_ptr)
{
	int			i;
	char		*id;
	t_scene		*scene;
	t_object	*obj;

	scene = (t_scene *)scene_ptr;
	ft_strlcpy(buffer, "Objects: [", 256);
	i = 0;
	while (i < count)
	{
		obj = &scene->objects.items[objects[i].index];
		id = obj->id;
		if (id)
			ft_strlcat(buffer, id, 256);
		if (i < count - 1)
			ft_strlcat(buffer, ", ", 256);
		i++;
	}
	ft_strlcat(buffer, "]", 256);
}

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
