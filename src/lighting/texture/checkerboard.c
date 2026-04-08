/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkerboard.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/10 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lighting/texture.h"
#include <math.h>

/**
 * @brief Compute checkerboard color for a plane surface.
 */
static t_color	checker_plane(t_object *obj, t_hit *hit)
{
	t_vec3	tangent;
	t_vec3	bitangent;
	int		pattern;

	tangent = get_tangent(obj->data.plane.normal);
	bitangent = vec3_cross(obj->data.plane.normal, tangent);
	pattern = (int)floor(vec3_dot(hit->point, tangent) / CHECKER_SCALE);
	pattern += (int)floor(vec3_dot(hit->point, bitangent) / CHECKER_SCALE);
	if (pattern & 1)
		return (obj->checker_color);
	return (obj->color);
}

/**
 * @brief Compute checkerboard color for a sphere surface.
 */
static t_color	checker_sphere(t_object *obj, t_hit *hit)
{
	t_vec3	local;
	double	u;
	double	v;
	int		pattern;

	local = vec3_normalize(vec3_subtract(hit->point,
				obj->data.sphere.center));
	u = 0.5 + atan2(local.z, local.x) / (2.0 * M_PI);
	v = 0.5 - asin(fmax(-1.0, fmin(1.0, local.y))) / M_PI;
	pattern = (int)floor(u * 8.0) + (int)floor(v * 8.0);
	if (pattern & 1)
		return (obj->checker_color);
	return (obj->color);
}

/**
 * @brief Return the radius for a cylinder or cone object.
 */
static double	get_body_radius(t_object *obj)
{
	if (obj->type == OBJ_CYLINDER)
		return (obj->data.cylinder.radius);
	return (obj->data.cone.radius);
}

/**
 * @brief Compute checkerboard color for a cylinder or cone body.
 */
static t_color	checker_body(t_object *obj, t_hit *hit)
{
	t_vec3	axis;
	t_vec3	tangent;
	t_vec3	d;
	double	h;
	int		pattern;

	if (obj->type == OBJ_CYLINDER)
		axis = obj->data.cylinder.axis;
	else
		axis = obj->data.cone.axis;
	tangent = get_tangent(axis);
	if (obj->type == OBJ_CYLINDER)
		d = vec3_subtract(hit->point, obj->data.cylinder.center);
	else
		d = vec3_subtract(hit->point, obj->data.cone.center);
	h = vec3_dot(d, axis) / CHECKER_SCALE;
	pattern = (int)floor(atan2(vec3_dot(d, vec3_cross(axis, tangent)),
				vec3_dot(d, tangent))
			* get_body_radius(obj) / CHECKER_SCALE);
	pattern += (int)floor(h);
	if (pattern & 1)
		return (obj->checker_color);
	return (obj->color);
}

/**
 * @brief Return the checkerboard-modulated color for an object.
 *
 * Dispatches to the appropriate checker function based on the
 * object type. Falls back to the base color for unknown types.
 *
 * @param obj Object with checkerboard enabled.
 * @param hit Hit record with the surface point.
 * @return t_color Checkerboard-patterned color.
 */
t_color	checkerboard_color(t_object *obj, t_hit *hit)
{
	if (obj->type == OBJ_PLANE)
		return (checker_plane(obj, hit));
	if (obj->type == OBJ_SPHERE)
		return (checker_sphere(obj, hit));
	if (obj->type == OBJ_CYLINDER || obj->type == OBJ_CONE)
		return (checker_body(obj, hit));
	return (obj->color);
}
