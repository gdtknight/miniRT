/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvhd_format.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/29 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvhd_internal.h"
#include <stdlib.h>
#include "libft.h"

/**
 * @brief Format leaf object IDs into info->objects buffer.
 *
 * @param node BVH leaf node containing object references.
 * @param info Node info whose objects buffer will be filled.
 * @param scene Scene for object ID lookup.
 */
void	bvhd_format_objects(t_bvh_node *node, t_node_info *info,
			t_scene *scene)
{
	int			i;
	char		*id;
	t_object	*obj;

	ft_strlcpy(info->objects, "Objects: [", sizeof(info->objects));
	i = 0;
	while (i < node->object_count)
	{
		obj = &scene->objects.items[node->objects[i].index];
		id = obj->id;
		if (id)
			ft_strlcat(info->objects, id, sizeof(info->objects));
		if (i < node->object_count - 1)
			ft_strlcat(info->objects, ", ", sizeof(info->objects));
		i++;
	}
	ft_strlcat(info->objects, "]", sizeof(info->objects));
}
