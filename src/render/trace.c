/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trace.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:06 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:20:06 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "ray.h"
#include "spatial.h"
#include "metrics.h"
#include <math.h>

/*
** Generic intersection checking for object arrays.
** Iterates through object array and updates hit with closest intersection.
** Returns 1 if any intersection found, 0 otherwise.
*/
static int	check_intersections_generic(void *objects, int count, \
		size_t obj_size, t_intersect_fn intersect_fn, t_ray *ray, t_hit *hit)
{
	t_hit	temp_hit;
	int		i;
	int		hit_found;
	void	*current_obj;

	if (objects == NULL && count > 0)
		return (0);
	hit_found = 0;
	i = 0;
	while (i < count)
	{
		current_obj = (char *)objects + (i * obj_size);
		temp_hit.distance = hit->distance;
		if (intersect_fn(ray, current_obj, &temp_hit))
		{
			*hit = temp_hit;
			hit_found = 1;
		}
		i++;
	}
	return (hit_found);
}

/*
** Check ray intersection with all spheres in scene.
** Updates hit info with closest sphere intersection.
** Returns 1 if any sphere was hit, 0 otherwise.
*/
int	check_sphere_intersections(t_scene *scene, t_ray *ray, t_hit *hit)
{
	return (check_intersections_generic(scene->spheres, \
		scene->sphere_count, sizeof(t_sphere), \
		(t_intersect_fn)intersect_sphere, ray, hit));
}

/*
** Check ray intersection with all planes in scene.
** Updates hit info with closest plane intersection.
** Returns 1 if any plane was hit, 0 otherwise.
*/
int	check_plane_intersections(t_scene *scene, t_ray *ray, t_hit *hit)
{
	return (check_intersections_generic(scene->planes, \
		scene->plane_count, sizeof(t_plane), \
		(t_intersect_fn)intersect_plane, ray, hit));
}

/*
** Check ray intersection with all cylinders in scene.
** Updates hit info with closest cylinder intersection.
** Returns 1 if any cylinder was hit, 0 otherwise.
*/
int	check_cylinder_intersections(t_scene *scene, t_ray *ray, t_hit *hit)
{
	return (check_intersections_generic(scene->cylinders, \
		scene->cylinder_count, sizeof(t_cylinder), \
		(t_intersect_fn)intersect_cylinder, ray, hit));
}

/*
** Trace ray through scene and determine pixel color.
** Tests intersection with all objects and finds closest hit.
** Uses BVH acceleration if enabled, otherwise brute force.
** Returns lit color if object hit, black if no intersection.
*/
t_color	trace_ray(t_scene *scene, t_ray *ray)
{
	t_hit	hit;
	int		hit_found;

	metrics_add_ray(&scene->render_state.metrics);
	hit_found = 0;
	hit.distance = INFINITY;
	if (scene->render_state.bvh_enabled && scene->render_state.bvh)
	{
		hit_found = bvh_intersect(scene->render_state.bvh, *ray, &hit, scene);
	}
	else
	{
		if (check_sphere_intersections(scene, ray, &hit))
			hit_found = 1;
		if (check_plane_intersections(scene, ray, &hit))
			hit_found = 1;
		if (check_cylinder_intersections(scene, ray, &hit))
			hit_found = 1;
	}
	if (hit_found)
		return (apply_lighting(scene, &hit));
	return ((t_color){0, 0, 0});
}
