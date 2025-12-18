/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_test.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:13 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:14 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"
#include "minirt.h"
#include "vec3.h"
#include "ray.h"

/*
** Check if shadow ray intersects any sphere in the scene.
** Returns 1 if intersection found, 0 otherwise.
*/
static int	check_sphere_shadow(t_scene *scene, t_ray *ray, t_hit *hit)
{
	int	i;

	i = 0;
	while (i < scene->sphere_count)
	{
		if (intersect_sphere(ray, &scene->spheres[i], hit))
			return (1);
		i++;
	}
	return (0);
}

/*
** Check if shadow ray intersects any plane in the scene.
** Returns 1 if intersection found, 0 otherwise.
*/
static int	check_plane_shadow(t_scene *scene, t_ray *ray, t_hit *hit)
{
	int	i;

	i = 0;
	while (i < scene->plane_count)
	{
		if (intersect_plane(ray, &scene->planes[i], hit))
			return (1);
		i++;
	}
	return (0);
}

/*
** Check if shadow ray intersects any cylinder in the scene.
** Returns 1 if intersection found, 0 otherwise.
*/
static int	check_cylinder_shadow(t_scene *scene, t_ray *ray, t_hit *hit)
{
	int	i;

	i = 0;
	while (i < scene->cylinder_count)
	{
		if (intersect_cylinder(ray, &scene->cylinders[i], hit))
			return (1);
		i++;
	}
	return (0);
}

/*
** Test if point is occluded from light source.
** Casts shadow ray from point toward light.
** Returns 1 if any object blocks the light, 0 if fully lit.
*/
int	is_in_shadow(t_scene *scene, t_vec3 point, t_vec3 light_pos, double bias)
{
	t_ray	shadow_ray;
	t_hit	shadow_hit;
	t_vec3	to_light;
	t_vec3	light_dir;

	to_light = vec3_subtract(light_pos, point);
	shadow_hit.distance = vec3_magnitude(to_light);
	light_dir = vec3_normalize(to_light);
	shadow_ray.origin = vec3_add(point, vec3_multiply(light_dir, bias));
	shadow_ray.direction = light_dir;
	if (check_sphere_shadow(scene, &shadow_ray, &shadow_hit))
		return (1);
	if (check_plane_shadow(scene, &shadow_ray, &shadow_hit))
		return (1);
	if (check_cylinder_shadow(scene, &shadow_ray, &shadow_hit))
		return (1);
	return (0);
}
