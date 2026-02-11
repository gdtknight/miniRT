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

#include "texture.h"
#include "vec3.h"
#include <math.h>

static t_vec3	get_tangent(t_vec3 n)
{
	if (fabs(n.y) > 0.9)
		return (vec3_normalize(vec3_cross(n, (t_vec3){1, 0, 0})));
	return (vec3_normalize(vec3_cross(n, (t_vec3){0, 1, 0})));
}

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

static t_color	checker_sphere(t_object *obj, t_hit *hit)
{
	t_vec3	local;
	double	u;
	double	v;
	int		pattern;

	local = vec3_normalize(vec3_subtract(hit->point,
				obj->data.sphere.center));
	u = 0.5 + atan2(local.z, local.x) / (2.0 * M_PI);
	v = 0.5 - asin(local.y) / M_PI;
	pattern = (int)floor(u * 8.0) + (int)floor(v * 8.0);
	if (pattern & 1)
		return (obj->checker_color);
	return (obj->color);
}

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
				vec3_dot(d, tangent)) * 4.0 / M_PI);
	pattern += (int)floor(h);
	if (pattern & 1)
		return (obj->checker_color);
	return (obj->color);
}

t_color	checkerboard_color(t_object *obj, t_hit *hit)
{
	if (obj->type == OBJ_PLANE)
		return (checker_plane(obj, hit));
	if (obj->type == OBJ_SPHERE)
		return (checker_sphere(obj, hit));
	return (checker_body(obj, hit));
}
