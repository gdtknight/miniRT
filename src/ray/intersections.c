/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersections.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/15 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "ray.h"
#include "vec3.h"
#include <math.h>

int	intersect_sphere(t_ray *ray, t_sphere *sphere, t_hit *hit)
{
	t_vec3	oc;
	double	a;
	double	b;
	double	c;
	double	discriminant;
	double	t;
	double	radius;

	radius = sphere->diameter / 2.0;
	oc = vec3_subtract(ray->origin, sphere->center);
	a = vec3_dot(ray->direction, ray->direction);
	b = 2.0 * vec3_dot(oc, ray->direction);
	c = vec3_dot(oc, oc) - radius * radius;
	discriminant = b * b - 4 * a * c;
	if (discriminant < 0)
		return (0);
	t = (-b - sqrt(discriminant)) / (2.0 * a);
	if (t < 0.001)
		t = (-b + sqrt(discriminant)) / (2.0 * a);
	if (t < 0.001 || t > hit->distance)
		return (0);
	hit->distance = t;
	hit->point = vec3_add(ray->origin, vec3_multiply(ray->direction, t));
	hit->normal = vec3_normalize(vec3_subtract(hit->point, sphere->center));
	hit->color = sphere->color;
	return (1);
}

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

int	intersect_cylinder(t_ray *ray, t_cylinder *cylinder, t_hit *hit)
{
	t_vec3	oc;
	t_vec3	axis;
	double	radius;
	double	a;
	double	b;
	double	c;
	double	discriminant;
	double	t;
	double	m;
	t_vec3	hit_point;

	radius = cylinder->diameter / 2.0;
	oc = vec3_subtract(ray->origin, cylinder->center);
	axis = cylinder->axis;
	a = vec3_dot(ray->direction, ray->direction) - \
		vec3_dot(ray->direction, axis) * vec3_dot(ray->direction, axis);
	b = 2.0 * (vec3_dot(ray->direction, oc) - \
		vec3_dot(ray->direction, axis) * vec3_dot(oc, axis));
	c = vec3_dot(oc, oc) - vec3_dot(oc, axis) * vec3_dot(oc, axis) - \
		radius * radius;
	discriminant = b * b - 4 * a * c;
	if (discriminant < 0 || a < EPSILON)
		return (0);
	t = (-b - sqrt(discriminant)) / (2.0 * a);
	if (t < 0.001)
		t = (-b + sqrt(discriminant)) / (2.0 * a);
	if (t < 0.001 || t > hit->distance)
		return (0);
	hit_point = vec3_add(ray->origin, vec3_multiply(ray->direction, t));
	m = vec3_dot(ray->direction, axis) * t + vec3_dot(oc, axis);
	if (m < 0 || m > cylinder->height)
		return (0);
	hit->distance = t;
	hit->point = hit_point;
	hit->normal = vec3_normalize(vec3_subtract(vec3_subtract(hit->point, \
		cylinder->center), vec3_multiply(axis, m)));
	hit->color = cylinder->color;
	return (1);
}
