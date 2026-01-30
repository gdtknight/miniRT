/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb_shapes.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:32:20 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include <math.h>

/**
 * @brief Compute an AABB for a sphere.
 *
 * @param center Sphere center.
 * @param radius Sphere radius.
 * @return t_aabb Bounding box enclosing the sphere.
 */
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

/**
 * @brief Compute an AABB for a finite cylinder.
 *
 * Uses the axis and half-height to compute extents in each axis.
 *
 * @param center Cylinder center.
 * @param axis Cylinder axis (normalized).
 * @param radius Cylinder radius.
 * @param height Cylinder full height.
 * @return t_aabb Bounding box enclosing the cylinder.
 */
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

/**
 * @brief Compute a large AABB for an infinite plane.
 *
 * Approximates the plane with a large bounding box centered at the point.
 *
 * @param point A point on the plane.
 * @param normal Plane normal (unused in this approximation).
 * @return t_aabb Large bounding box representing the plane.
 */
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
