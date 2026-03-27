/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_trace.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:06 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "shading.h"
#include "intersect.h"
#include "spatial.h"
#include "metrics.h"
#include <math.h>

/**
 * @brief Brute-force intersection test against all objects.
 *
 * Iterates over every object in the scene and updates the hit record with
 * the closest intersection found along the ray.
 *
 * @param scene Scene containing objects.
 * @param ray Ray to test.
 * @param hit In/out hit record (initialized with max distance).
 * @return int 1 if any intersection is found, 0 otherwise.
 */
static int	check_all_objects(t_scene *scene, t_ray *ray, t_hit *hit)
{
	int			i;
	int			found;
	t_hit		temp;
	t_object	*obj;

	found = 0;
	i = 0;
	while (i < scene->objects.count)
	{
		obj = &scene->objects.items[i];
		temp.distance = hit->distance;
		metrics_add_intersect_test(&scene->metrics);
		if (intersect_object(ray, obj, &temp))
		{
			*hit = temp;
			found = 1;
		}
		i++;
	}
	return (found);
}

/**
 * @brief Test ray against planes separated from BVH.
 *
 * Iterates over plane indices stored in bvh->plane_refs and tests
 * each plane for intersection, tracking the closest hit.
 *
 * @param scene Scene containing objects and BVH with plane refs.
 * @param ray Ray to test.
 * @param hit In/out hit record.
 * @return int 1 if any plane hit is found, 0 otherwise.
 */
static int	intersect_planes(t_scene *scene, t_ray *ray, t_hit *hit)
{
	int			i;
	int			found;
	t_hit		temp;
	t_object	*obj;

	found = 0;
	i = 0;
	while (i < scene->bvh->plane_refs.count)
	{
		obj = &scene->objects.items[scene->bvh->plane_refs.indices[i]];
		temp.distance = hit->distance;
		metrics_add_intersect_test(&scene->metrics);
		if (intersect_object(ray, obj, &temp))
		{
			*hit = temp;
			found = 1;
		}
		i++;
	}
	return (found);
}

/**
 * @brief Trace a ray through the scene and compute its color.
 *
 * When BVH is valid, uses BVH for bounded objects plus separate plane
 * intersection. Otherwise falls back to brute-force scan.
 *
 * @param scene Scene containing geometry and lighting.
 * @param ray Ray to trace.
 * @return t_color Resulting color (black if no hit).
 */
t_color	trace_ray(t_scene *scene, t_ray *ray)
{
	t_hit	hit;
	int		found;

	metrics_add_ray(&scene->metrics);
	hit.distance = INFINITY;
	found = 0;
	if (scene->bvh && scene->bvh->enabled && scene->bvh->root)
	{
		if (bvh_intersect(scene->bvh, *ray, &hit, scene))
			found = 1;
		if (intersect_planes(scene, ray, &hit))
			found = 1;
		if (found)
			return (apply_lighting(scene, &hit));
		return ((t_color){0, 0, 0});
	}
	if (check_all_objects(scene, ray, &hit))
		return (apply_lighting(scene, &hit));
	return ((t_color){0, 0, 0});
}
