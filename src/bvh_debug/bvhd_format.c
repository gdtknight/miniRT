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

#include "bvh_debug.h"
#include "scene.h"
#include <stdlib.h>
#include "libft.h"

/**
 * @brief Format a list of object IDs into a buffer.
 *
 * @param objects Array of object references.
 * @param count Number of objects.
 * @param buffer Destination buffer (assumed large enough).
 * @param scene_ptr Pointer to the scene for ID lookup.
 */
void	bvhd_format_objects(t_object_ref *objects, int count, char *buffer,
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
