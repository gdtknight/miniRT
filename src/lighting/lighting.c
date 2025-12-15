/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lighting.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/15 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "vec3.h"
#include "ray.h"
#include <math.h>

static int	in_shadow(t_scene *scene, t_vec3 point, t_vec3 light_dir)
{
	t_ray	shadow_ray;
	t_hit	shadow_hit;
	int		i;
	double	light_distance;

	shadow_ray.origin = vec3_add(point, vec3_multiply(light_dir, 0.001));
	shadow_ray.direction = light_dir;
	light_distance = vec3_magnitude(vec3_subtract(scene->light.position, point));
	shadow_hit.distance = light_distance;
	i = 0;
	while (i < scene->sphere_count)
	{
		if (intersect_sphere(&shadow_ray, &scene->spheres[i], &shadow_hit))
			return (1);
		i++;
	}
	i = 0;
	while (i < scene->plane_count)
	{
		if (intersect_plane(&shadow_ray, &scene->planes[i], &shadow_hit))
			return (1);
		i++;
	}
	return (0);
}

t_color	apply_lighting(t_scene *scene, t_hit *hit)
{
	t_vec3	light_dir;
	double	diffuse;
	t_color	result;
	double	ambient;

	ambient = scene->ambient.ratio;
	light_dir = vec3_normalize(vec3_subtract(scene->light.position, hit->point));
	diffuse = vec3_dot(hit->normal, light_dir);
	if (diffuse < 0)
		diffuse = 0;
	if (in_shadow(scene, hit->point, light_dir))
		diffuse = 0;
	result.r = hit->color.r * (ambient + diffuse * scene->light.brightness);
	result.g = hit->color.g * (ambient + diffuse * scene->light.brightness);
	result.b = hit->color.b * (ambient + diffuse * scene->light.brightness);
	if (result.r > 255)
		result.r = 255;
	if (result.g > 255)
		result.g = 255;
	if (result.b > 255)
		result.b = 255;
	return (result);
}
