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
#include <math.h>

/*
** Check ray intersection with all spheres in scene.
** Updates hit info with closest sphere intersection.
** Returns 1 if any sphere was hit, 0 otherwise.
*/
int	check_sphere_intersections(t_scene *scene, t_ray *ray, t_hit *hit)
{
	t_hit	temp_hit;
	int		i;
	int		hit_found;

	hit_found = 0;
	i = 0;
	while (i < scene->sphere_count)
	{
		temp_hit.distance = hit->distance;
		if (intersect_sphere(ray, &scene->spheres[i], &temp_hit))
		{
			*hit = temp_hit;
			hit_found = 1;
		}
		i++;
	}
	return (hit_found);
}

/*
** Check ray intersection with all planes in scene.
** Updates hit info with closest plane intersection.
** Returns 1 if any plane was hit, 0 otherwise.
*/
int	check_plane_intersections(t_scene *scene, t_ray *ray, t_hit *hit)
{
	t_hit	temp_hit;
	int		i;
	int		hit_found;

	hit_found = 0;
	i = 0;
	while (i < scene->plane_count)
	{
		temp_hit.distance = hit->distance;
		if (intersect_plane(ray, &scene->planes[i], &temp_hit))
		{
			*hit = temp_hit;
			hit_found = 1;
		}
		i++;
	}
	return (hit_found);
}

/*
** Check ray intersection with all cylinders in scene.
** Updates hit info with closest cylinder intersection.
** Returns 1 if any cylinder was hit, 0 otherwise.
*/
int	check_cylinder_intersections(t_scene *scene, t_ray *ray, t_hit *hit)
{
	t_hit	temp_hit;
	int		i;
	int		hit_found;

	hit_found = 0;
	i = 0;
	while (i < scene->cylinder_count)
	{
		temp_hit.distance = hit->distance;
		if (intersect_cylinder(ray, &scene->cylinders[i], &temp_hit))
		{
			*hit = temp_hit;
			hit_found = 1;
		}
		i++;
	}
	return (hit_found);
}

/*
** Trace ray through scene and determine pixel color.
** Tests intersection with all objects and finds closest hit.
** Returns lit color if object hit, black if no intersection.
*/
t_color	trace_ray(t_scene *scene, t_ray *ray)
{
	t_hit	hit;
	int		hit_found;

	hit_found = 0;
	hit.distance = INFINITY;
	if (check_sphere_intersections(scene, ray, &hit))
		hit_found = 1;
	if (check_plane_intersections(scene, ray, &hit))
		hit_found = 1;
	if (check_cylinder_intersections(scene, ray, &hit))
		hit_found = 1;
	if (hit_found)
		return (apply_lighting(scene, &hit));
	return ((t_color){0, 0, 0});
}
