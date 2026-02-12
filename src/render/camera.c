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
 * @brief Update camera basis cache if invalid.
 *
 * Computes aspect ratio, field-of-view scaling, and orthonormal basis
 * vectors (right/up) only when the cache is marked invalid.
 *
 * @param camera Camera with cache to update.
 */
static void	update_camera_cache(t_camera *camera)
{
	if (camera->cache.valid)
		return ;
	camera->cache.aspect_ratio = ASPECT_RATIO_NUM / ASPECT_RATIO_DEN;
	camera->cache.fov_scale = tan(camera->fov * 0.5 * M_PI / 180.0);
	camera->cache.right.x = -cos(camera->yaw);
	camera->cache.right.y = 0;
	camera->cache.right.z = sin(camera->yaw);
	camera->cache.up = vec3_normalize(vec3_cross(camera->cache.right,
				camera->direction));
	camera->cache.valid = 1;
}

/**
 * @brief Create a camera ray for normalized device coordinates.
 *
 * Converts the normalized screen coordinate (x, y) into a ray direction
 * based on the cached camera basis and FOV, then returns a ray originating
 * at the camera position.
 *
 * @param camera Camera describing the view.
 * @param x Normalized horizontal coordinate in [-1, 1].
 * @param y Normalized vertical coordinate in [-1, 1].
 * @return t_ray Ray originating at the camera through (x, y).
 */
t_ray	create_camera_ray(t_camera *camera, double x, double y)
{
	t_ray			ray;
	t_vec3			pixel_pos;
	t_camera_cache	*c;

	update_camera_cache(camera);
	c = &camera->cache;
	pixel_pos = camera->direction;
	pixel_pos = vec3_add(pixel_pos, vec3_multiply(c->right,
				x * c->fov_scale * c->aspect_ratio));
	pixel_pos = vec3_add(pixel_pos, vec3_multiply(c->up, y * c->fov_scale));
	ray.origin = camera->position;
	ray.direction = vec3_normalize(pixel_pos);
	ray.inv_dir.x = 1.0 / ray.direction.x;
	ray.inv_dir.y = 1.0 / ray.direction.y;
	ray.inv_dir.z = 1.0 / ray.direction.z;
	return (ray);
}
