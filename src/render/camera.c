/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
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

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

typedef struct s_cam_calc
{
	double	aspect_ratio;
	double	fov_scale;
	t_vec3	right;
	t_vec3	up;
}	t_cam_calc;

static void	init_camera_calc(t_camera *camera, t_cam_calc *calc)
{
	calc->aspect_ratio = 800.0 / 600.0;
	calc->fov_scale = tan(camera->fov * 0.5 * M_PI / 180.0);
	calc->right = vec3_normalize(vec3_cross(camera->direction, \
		(t_vec3){0, 1, 0}));
	calc->up = vec3_normalize(vec3_cross(calc->right, camera->direction));
}

t_ray	create_camera_ray(t_camera *camera, double x, double y)
{
	t_ray		ray;
	t_vec3		pixel_pos;
	t_cam_calc	calc;

	init_camera_calc(camera, &calc);
	pixel_pos = camera->direction;
	pixel_pos = vec3_add(pixel_pos, vec3_multiply(calc.right, \
		x * calc.fov_scale * calc.aspect_ratio));
	pixel_pos = vec3_add(pixel_pos, vec3_multiply(calc.up, \
		y * calc.fov_scale));
	ray.origin = camera->position;
	ray.direction = vec3_normalize(pixel_pos);
	return (ray);
}
