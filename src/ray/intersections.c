/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersections.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:51 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:52 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "ray.h"
#include "vec3.h"
#include <math.h>

/*
** Calculate ray-sphere intersection distance using quadratic formula.
** Solves: ||ray.origin + t*ray.direction - sphere.center||² = radius²
** Returns closest positive t value, or -1 if no intersection.
*/
static double	calculate_sphere_t(t_vec3 oc, t_ray *ray, double radius_squared)
{
	double	a;
	double	b;
	double	c;
	double	discriminant;
	double	t;

	a = vec3_dot(ray->direction, ray->direction);
	b = 2.0 * vec3_dot(oc, ray->direction);
	c = vec3_dot(oc, oc) - radius_squared;
	discriminant = b * b - 4 * a * c;
	if (discriminant < 0)
		return (-1);
	t = (-b - sqrt(discriminant)) / (2.0 * a);
	if (t < 0.001)
		t = (-b + sqrt(discriminant)) / (2.0 * a);
	return (t);
}

/*
** Test ray-sphere intersection and update hit info if closer.
** Uses quadratic equation to find intersection points.
** Returns 1 if hit and closer than current hit->distance, 0 otherwise.
*/
int	intersect_sphere(t_ray *ray, t_sphere *sphere, t_hit *hit)
{
	t_vec3	oc;
	double	t;

	oc = vec3_subtract(ray->origin, sphere->center);
	t = calculate_sphere_t(oc, ray, sphere->radius_squared);
	if (t < 0.001 || t > hit->distance)
		return (0);
	hit->distance = t;
	hit->point = vec3_add(ray->origin, vec3_multiply(ray->direction, t));
	hit->normal = vec3_normalize(vec3_subtract(hit->point, sphere->center));
	hit->color = sphere->color;
	return (1);
}

/*
** Test ray-plane intersection using plane equation.
** Plane equation: dot(point - plane.point, plane.normal) = 0
** Returns 1 if hit and closer than current hit->distance, 0 otherwise.
*/
int	intersect_plane(t_ray *ray, t_plane *plane, t_hit *hit)
{
	double	denom;
	double	t;
	t_vec3	p0l0;

	denom = vec3_dot(plane->normal, ray->direction);
	if (fabs(denom) < 0.0001)
		return (0);
	p0l0 = vec3_subtract(plane->point, ray->origin);
	t = vec3_dot(p0l0, plane->normal) / denom;
	if (t < 0.001 || t > hit->distance)
		return (0);
	hit->distance = t;
	hit->point = vec3_add(ray->origin, vec3_multiply(ray->direction, t));
	hit->normal = plane->normal;
	if (vec3_dot(ray->direction, hit->normal) > 0)
		hit->normal = vec3_multiply(hit->normal, -1);
	hit->color = plane->color;
	return (1);
}
