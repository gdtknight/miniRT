/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:56 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:33:02 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "vec3.h"
#include "ray.h"
#include <math.h>

/**
 * @brief Initialize camera basis vectors and projection parameters.
 *
 * Computes aspect ratio, field-of-view scaling, and orthonormal basis
 * vectors (right/up) derived from the camera direction.
 *
 * @param camera Camera defining position, direction, and FOV.
 * @param calc Output structure populated with derived values.
 */
static void	init_camera_calc(t_camera *camera, t_cam_calc *calc)
{
	calc->aspect_ratio = ASPECT_RATIO_NUM / ASPECT_RATIO_DEN;
	calc->fov_scale = tan(camera->fov * 0.5 * M_PI / 180.0);
	calc->right = vec3_normalize(vec3_cross(camera->direction,
				(t_vec3){0, 1, 0}));
	calc->up = vec3_normalize(vec3_cross(calc->right, camera->direction));
}

/**
 * @brief Create a camera ray for normalized device coordinates.
 *
 * Converts the normalized screen coordinate (x, y) into a ray direction
 * based on the camera basis and FOV, then returns a ray originating at
 * the camera position.
 *
 * @param camera Camera describing the view.
 * @param x Normalized horizontal coordinate in [-1, 1].
 * @param y Normalized vertical coordinate in [-1, 1].
 * @return t_ray Ray originating at the camera through (x, y).
 */
t_ray	create_camera_ray(t_camera *camera, double x, double y)
{
	t_ray		ray;
	t_vec3		pixel_pos;
	t_cam_calc	calc;

	init_camera_calc(camera, &calc);
	pixel_pos = camera->direction;
	pixel_pos = vec3_add(pixel_pos, vec3_multiply(calc.right,
				x * calc.fov_scale * calc.aspect_ratio));
	pixel_pos = vec3_add(pixel_pos, vec3_multiply(calc.up,
				y * calc.fov_scale));
	ray.origin = camera->position;
	ray.direction = vec3_normalize(pixel_pos);
	return (ray);
}
