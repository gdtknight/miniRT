/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:56 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:56 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "vec3.h"
#include "ray.h"
#include <math.h>

/* Helper structure for camera ray calculations */
typedef struct s_cam_calc
{
	double	aspect_ratio;
	double	fov_scale;
	t_vec3	right;
	t_vec3	up;
}	t_cam_calc;

/*
** Initialize camera coordinate system (right and up vectors).
** Creates orthonormal basis from camera direction.
** Assumes world up is (0, 1, 0) for calculating right vector.
*/
static void	init_camera_calc(t_camera *camera, t_cam_calc *calc)
{
	calc->aspect_ratio = 800.0 / 600.0;
	calc->fov_scale = tan(camera->fov * 0.5 * M_PI / 180.0);
	calc->right = vec3_normalize(vec3_cross(camera->direction, \
		(t_vec3){0, 1, 0}));
	calc->up = vec3_normalize(vec3_cross(calc->right, camera->direction));
}

/*
** Create camera ray for pixel at normalized coordinates (x, y).
** x, y in range [-1, 1] where (0,0) is center of screen.
** Calculates ray direction based on camera FOV and orientation.
*/
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
