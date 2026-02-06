/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_test.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:13 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"
#include "minirt.h"
#include "vec3.h"
#include "ray.h"
#include "metrics.h"

/**
 * @brief Test shadow ray against all objects in the scene.
 *
 * Iterates through all objects and returns as soon as any intersection is
 * found within the ray range.
 *
 * @param scene Scene containing objects.
 * @param ray Shadow ray to test.
 * @param hit Hit record used for intersection queries.
 * @return int 1 if occluded, 0 if clear.
 */
static int	check_object_shadow(t_scene *scene, t_ray *ray, t_hit *hit)
{
	int			i;
	t_object	*obj;

	i = 0;
	while (i < scene->objects.count)
	{
		obj = &scene->objects.items[i];
		metrics_add_shadow_intersect(&scene->metrics);
		if (intersect_object_new(ray, obj, hit))
			return (1);
		i++;
	}
	return (0);
}

/**
 * @brief Determine whether a point is shadowed from a light.
 *
 * Builds a shadow ray toward the light, applies a bias to avoid self-shadow,
 * and tests for occlusion by scene objects.
 *
 * @param scene Scene containing objects for occlusion tests.
 * @param point Shaded point in world space.
 * @param light_pos Light position in world space.
 * @param bias Bias distance to offset the shadow ray origin.
 * @return int 1 if the point is in shadow, 0 otherwise.
 */
int	is_in_shadow(t_scene *scene, t_vec3 point, t_vec3 light_pos, double bias)
{
	t_ray	shadow_ray;
	t_hit	shadow_hit;
	t_vec3	to_light;
	t_vec3	light_dir;
	double	mag;

	to_light = vec3_subtract(light_pos, point);
	mag = vec3_magnitude(to_light);
	shadow_hit.distance = mag;
	light_dir = vec3_multiply(to_light, 1.0 / mag);
	shadow_ray.origin = vec3_add(point, vec3_multiply(light_dir, bias));
	shadow_ray.direction = light_dir;
	return (check_object_shadow(scene, &shadow_ray, &shadow_hit));
}
