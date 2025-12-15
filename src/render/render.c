/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
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
#include "window.h"
#include <math.h>

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

static t_ray	create_camera_ray(t_camera *camera, double x, double y)
{
	t_ray	ray;
	t_vec3	right;
	t_vec3	up;
	t_vec3	pixel_pos;
	double	aspect_ratio;
	double	fov_scale;

	aspect_ratio = 800.0 / 600.0;
	fov_scale = tan(camera->fov * 0.5 * M_PI / 180.0);
	right = vec3_normalize(vec3_cross(camera->direction, (t_vec3){0, 1, 0}));
	up = vec3_normalize(vec3_cross(right, camera->direction));
	pixel_pos = camera->direction;
	pixel_pos = vec3_add(pixel_pos, vec3_multiply(right, x * fov_scale * aspect_ratio));
	pixel_pos = vec3_add(pixel_pos, vec3_multiply(up, y * fov_scale));
	ray.origin = camera->position;
	ray.direction = vec3_normalize(pixel_pos);
	return (ray);
}

static t_color	trace_ray(t_scene *scene, t_ray *ray)
{
	t_hit	hit;
	t_hit	temp_hit;
	int		i;
	int		hit_found;

	hit_found = 0;
	hit.distance = INFINITY;
	i = 0;
	while (i < scene->sphere_count)
	{
		temp_hit.distance = hit.distance;
		if (intersect_sphere(ray, &scene->spheres[i], &temp_hit))
		{
			hit = temp_hit;
			hit_found = 1;
		}
		i++;
	}
	i = 0;
	while (i < scene->plane_count)
	{
		temp_hit.distance = hit.distance;
		if (intersect_plane(ray, &scene->planes[i], &temp_hit))
		{
			hit = temp_hit;
			hit_found = 1;
		}
		i++;
	}
	if (hit_found)
		return (apply_lighting(scene, &hit));
	return ((t_color){0, 0, 0});
}

void	render_scene(t_scene *scene, void *mlx, void *win)
{
	int		x;
	int		y;
	t_ray	ray;
	t_color	color;
	double	u;
	double	v;

	y = 0;
	while (y < 600)
	{
		x = 0;
		while (x < 800)
		{
			u = (2.0 * x / 800.0) - 1.0;
			v = 1.0 - (2.0 * y / 600.0);
			ray = create_camera_ray(&scene->camera, u, v);
			color = trace_ray(scene, &ray);
			mlx_pixel_put(mlx, win, x, y, (color.r << 16) | (color.g << 8) | color.b);
			x++;
		}
		y++;
	}
}
