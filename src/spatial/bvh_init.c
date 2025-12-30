/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 21:30:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "minirt.h"
#include "window.h"
#include <stdlib.h>

static int	count_objects(t_scene *scene)
{
	return (scene->sphere_count + scene->plane_count + scene->cylinder_count);
}

static void	fill_object_refs(t_scene *scene, t_object_ref *refs)
{
	int	i;
	int	idx;

	idx = 0;
	i = 0;
	while (i < scene->sphere_count)
	{
		refs[idx].type = OBJ_SPHERE;
		refs[idx].index = i;
		idx++;
		i++;
	}
	i = 0;
	while (i < scene->plane_count)
	{
		refs[idx].type = OBJ_PLANE;
		refs[idx].index = i;
		idx++;
		i++;
	}
	i = 0;
	while (i < scene->cylinder_count)
	{
		refs[idx].type = OBJ_CYLINDER;
		refs[idx].index = i;
		idx++;
		i++;
	}
}

void	scene_build_bvh(t_scene *scene)
{
	t_object_ref	*refs;
	int				total_objects;

	if (!scene->render_state.bvh_enabled)
		return ;
	total_objects = count_objects(scene);
	if (total_objects == 0)
		return ;
	refs = malloc(sizeof(t_object_ref) * total_objects);
	if (!refs)
		return ;
	fill_object_refs(scene, refs);
	if (!scene->render_state.bvh)
		scene->render_state.bvh = bvh_create();
	if (scene->render_state.bvh)
		bvh_build(scene->render_state.bvh, refs, total_objects, scene);
	free(refs);
}
