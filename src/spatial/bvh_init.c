/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "minirt.h"
#include <stdlib.h>

static void	fill_object_refs(t_scene *scene, t_object_ref *refs)
{
	int	i;

	i = 0;
	while (i < scene->objects.count)
	{
		refs[i].index = i;
		i++;
	}
}

void	scene_build_bvh(t_scene *scene)
{
	t_object_ref	*refs;
	int				total;

	if (!(scene->flags & SCENE_BVH_ENABLED))
		return ;
	total = scene->objects.count;
	if (total == 0)
		return ;
	refs = malloc(sizeof(t_object_ref) * total);
	if (!refs)
		return ;
	fill_object_refs(scene, refs);
	if (!scene->bvh)
		scene->bvh = bvh_create();
	if (scene->bvh)
		bvh_build(scene->bvh, refs, total, scene);
	free(refs);
}
