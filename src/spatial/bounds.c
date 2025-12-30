/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bounds.c                                           :+:      :+:    :+:   */
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

t_aabb	get_object_bounds(t_object_ref ref, void *scene_ptr)
{
	t_scene	*scene;

	scene = (t_scene *)scene_ptr;
	if (ref.type == OBJ_SPHERE)
		return (aabb_for_sphere(scene->spheres[ref.index].center,
				scene->spheres[ref.index].diameter / 2.0));
	else if (ref.type == OBJ_CYLINDER)
		return (aabb_for_cylinder(scene->cylinders[ref.index].center,
				scene->cylinders[ref.index].axis,
				scene->cylinders[ref.index].diameter / 2.0,
				scene->cylinders[ref.index].height));
	else
		return (aabb_for_plane(scene->planes[ref.index].point,
				scene->planes[ref.index].normal));
}

t_vec3	get_object_center(t_object_ref ref, void *scene_ptr)
{
	t_scene	*scene;

	scene = (t_scene *)scene_ptr;
	if (ref.type == OBJ_SPHERE)
		return (scene->spheres[ref.index].center);
	else if (ref.type == OBJ_CYLINDER)
		return (scene->cylinders[ref.index].center);
	else
		return (scene->planes[ref.index].point);
}
