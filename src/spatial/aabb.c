/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 21:30:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "minirt.h"
#include <math.h>

static double	min_double(double a, double b)
{
	if (a < b)
		return (a);
	return (b);
}

static double	max_double(double a, double b)
{
	if (a > b)
		return (a);
	return (b);
}

t_aabb	aabb_create(t_vec3 min, t_vec3 max)
{
	t_aabb	box;

	box.min = min;
	box.max = max;
	return (box);
}

t_aabb	aabb_merge(t_aabb a, t_aabb b)
{
	t_aabb	result;

	result.min.x = min_double(a.min.x, b.min.x);
	result.min.y = min_double(a.min.y, b.min.y);
	result.min.z = min_double(a.min.z, b.min.z);
	result.max.x = max_double(a.max.x, b.max.x);
	result.max.y = max_double(a.max.y, b.max.y);
	result.max.z = max_double(a.max.z, b.max.z);
	return (result);
}

t_aabb	aabb_for_sphere(t_vec3 center, double radius)
{
	t_aabb	box;
	t_vec3	offset;

	offset.x = radius;
	offset.y = radius;
	offset.z = radius;
	box.min = vec3_subtract(center, offset);
	box.max = vec3_add(center, offset);
	return (box);
}

t_aabb	aabb_for_cylinder(t_vec3 center, t_vec3 axis, double radius,
		double height)
{
	t_aabb	box;
	t_vec3	half_axis;
	double	extent;

	half_axis = vec3_multiply(axis, height / 2.0);
	extent = radius + fabs(half_axis.x);
	box.min.x = center.x - extent;
	box.max.x = center.x + extent;
	extent = radius + fabs(half_axis.y);
	box.min.y = center.y - extent;
	box.max.y = center.y + extent;
	extent = radius + fabs(half_axis.z);
	box.min.z = center.z - extent;
	box.max.z = center.z + extent;
	return (box);
}

t_aabb	aabb_for_plane(t_vec3 point, t_vec3 normal)
{
	t_aabb	box;
	double	large;

	large = 1000000.0;
	(void)normal;
	box.min.x = point.x - large;
	box.min.y = point.y - large;
	box.min.z = point.z - large;
	box.max.x = point.x + large;
	box.max.y = point.y + large;
	box.max.z = point.z + large;
	return (box);
}

double	aabb_surface_area(t_aabb box)
{
	double	dx;
	double	dy;
	double	dz;

	dx = box.max.x - box.min.x;
	dy = box.max.y - box.min.y;
	dz = box.max.z - box.min.z;
	return (2.0 * (dx * dy + dy * dz + dz * dx));
}

int	aabb_intersect(t_aabb box, t_ray ray, double *t_min, double *t_max)
{
	double	t0;
	double	t1;
	double	tmin;
	double	tmax;

	tmin = (*t_min);
	tmax = (*t_max);
	t0 = (box.min.x - ray.origin.x) / ray.direction.x;
	t1 = (box.max.x - ray.origin.x) / ray.direction.x;
	if (t0 > t1)
	{
		tmin = max_double(tmin, t1);
		tmax = min_double(tmax, t0);
	}
	else
	{
		tmin = max_double(tmin, t0);
		tmax = min_double(tmax, t1);
	}
	t0 = (box.min.y - ray.origin.y) / ray.direction.y;
	t1 = (box.max.y - ray.origin.y) / ray.direction.y;
	if (t0 > t1)
	{
		tmin = max_double(tmin, t1);
		tmax = min_double(tmax, t0);
	}
	else
	{
		tmin = max_double(tmin, t0);
		tmax = min_double(tmax, t1);
	}
	t0 = (box.min.z - ray.origin.z) / ray.direction.z;
	t1 = (box.max.z - ray.origin.z) / ray.direction.z;
	if (t0 > t1)
	{
		tmin = max_double(tmin, t1);
		tmax = min_double(tmax, t0);
	}
	else
	{
		tmin = max_double(tmin, t0);
		tmax = min_double(tmax, t1);
	}
	*t_min = tmin;
	*t_max = tmax;
	return (tmax >= tmin && tmax > 0);
}
