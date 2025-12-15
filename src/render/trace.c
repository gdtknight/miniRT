/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trace.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/15 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "ray.h"
#include "vec3.h"

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
	if (hit_found)
		return (apply_lighting(scene, &hit));
	return ((t_color){0, 0, 0});
}
