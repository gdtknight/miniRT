/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_occlusion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:13 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lighting/shadow.h"
#include "scene/scene.h"
#include "common/ray.h"
#include "common/objects.h"
#include "spatial/intersect.h"
#include "spatial/spatial.h"
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
		if (intersect_object(ray, obj, hit))
			return (1);
		i++;
	}
	return (0);
}

/**
 * @brief Test shadow ray against planes separated from BVH.
 *
 * Iterates over plane indices in bvh->plane_refs and returns on first hit.
 *
 * @param scene Scene containing objects and BVH with plane refs.
 * @param ray Shadow ray to test.
 * @param mag Maximum distance to light source.
 * @return int 1 if any plane occludes the light, 0 otherwise.
 */
static int	check_plane_shadow(t_scene *scene, t_ray *ray, double mag)
{
	int			i;
	t_hit		hit;
	t_object	*obj;

	if (!scene->bvh)
		return (0);
	i = 0;
	while (i < scene->bvh->plane_refs.count)
	{
		obj = &scene->objects.items[scene->bvh->plane_refs.indices[i]];
		hit.distance = mag;
		metrics_add_shadow_intersect(&scene->metrics);
		if (intersect_object(ray, obj, &hit))
			return (1);
		i++;
	}
	return (0);
}

/**
 * @brief Build a shadow ray biased along normal and light dir.
 *
 * @param ray Output shadow ray to initialize.
 * @param query Surface point and normal for origin bias.
 * @param light_dir Normalized direction toward the light.
 * @param bias Offset distance to prevent self-shadowing.
 */
static void	init_shadow_ray(t_ray *ray, t_shadow_query *query,
	t_vec3 light_dir, double bias)
{
	ray->origin = vec3_add(query->point, vec3_add(
				vec3_multiply(query->normal, bias),
				vec3_multiply(light_dir, bias)));
	ray->direction = light_dir;
	ray->inv_dir.x = 1.0 / (light_dir.x + (light_dir.x == 0) * 1e-15);
	ray->inv_dir.y = 1.0 / (light_dir.y + (light_dir.y == 0) * 1e-15);
	ray->inv_dir.z = 1.0 / (light_dir.z + (light_dir.z == 0) * 1e-15);
}

/**
 * @brief Determine whether a point is shadowed from a light.
 *
 * Builds a shadow ray toward the light, applies a bias along both the
 * surface normal and light direction to avoid self-shadowing.
 *
 * @param scene Scene containing objects for occlusion tests.
 * @param query Surface point and normal.
 * @param light_pos Light position in world space.
 * @param bias Bias distance to offset the shadow ray origin.
 * @return int 1 if the point is in shadow, 0 otherwise.
 */
int	shadow_is_occluded(t_scene *scene, t_shadow_query query, t_vec3 light_pos,
	double bias)
{
	t_ray	shadow_ray;
	t_hit	shadow_hit;
	t_vec3	to_light;
	double	mag;

	to_light = vec3_subtract(light_pos, query.point);
	mag = vec3_magnitude(to_light);
	if (mag < COEFF_EPSILON)
		return (0);
	shadow_hit.distance = mag;
	init_shadow_ray(&shadow_ray, &query,
		vec3_multiply(to_light, 1.0 / mag), bias);
	if (scene->bvh && scene->bvh->enabled && scene->bvh->root
		&& scene->objects.count > SHADOW_BVH_THRESHOLD)
	{
		if (bvh_intersect_any(scene->bvh, shadow_ray, mag, scene))
			return (1);
		return (check_plane_shadow(scene, &shadow_ray, mag));
	}
	return (check_object_shadow(scene, &shadow_ray, &shadow_hit));
}
