/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersect_object.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:46:16 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ray.h"
#include "vec3.h"
#include "minirt.h"
#include <math.h>

/**
 * @brief Intersect a ray with a sphere and update hit record.
 *
 * Solves the quadratic equation for sphere intersection and records the
 * nearest valid hit if within the current hit distance.
 *
 * @param ray Ray to test.
 * @param s Sphere data.
 * @param color Surface color of the sphere.
 * @param hit Hit record to update.
 * @return int 1 if an intersection is found, 0 otherwise.
 */
static int	intersect_sphere_new(t_ray *ray, t_sphere_data *s, t_color color,
		t_hit *hit)
{
	t_vec3	oc;
	double	a;
	double	b;
	double	c;
	double	d;

	oc = vec3_subtract(ray->origin, s->center);
	a = vec3_dot(ray->direction, ray->direction);
	b = 2.0 * vec3_dot(oc, ray->direction);
	c = vec3_dot(oc, oc) - s->radius_sq;
	d = b * b - 4 * a * c;
	if (d < 0 || a < EPSILON)
		return (0);
	c = sqrt(d);
	d = (-b - c) / (2.0 * a);
	if (d < RAY_T_MIN)
		d = (-b + c) / (2.0 * a);
	if (d < RAY_T_MIN || d > hit->distance)
		return (0);
	hit->distance = d;
	hit->point = vec3_add(ray->origin, vec3_multiply(ray->direction, d));
	hit->normal = vec3_normalize(vec3_subtract(hit->point, s->center));
	hit->color = color;
	return (1);
}

/**
 * @brief Intersect a ray with a plane and update hit record.
 *
 * Computes ray-plane intersection and updates the hit record when a valid
 * nearer hit is found.
 *
 * @param ray Ray to test.
 * @param p Plane data.
 * @param color Surface color of the plane.
 * @param hit Hit record to update.
 * @return int 1 if an intersection is found, 0 otherwise.
 */
static int	intersect_plane_new(t_ray *ray, t_plane_data *p, t_color color,
		t_hit *hit)
{
	double	denom;
	double	t;
	t_vec3	p0l0;

	denom = vec3_dot(p->normal, ray->direction);
	if (fabs(denom) < COEFF_EPSILON)
		return (0);
	p0l0 = vec3_subtract(p->point, ray->origin);
	t = vec3_dot(p0l0, p->normal) / denom;
	if (t < RAY_T_MIN || t > hit->distance)
		return (0);
	hit->distance = t;
	hit->point = vec3_add(ray->origin, vec3_multiply(ray->direction, t));
	hit->normal = p->normal;
	if (vec3_dot(ray->direction, hit->normal) > 0)
		hit->normal = vec3_multiply(hit->normal, -1);
	hit->color = color;
	return (1);
}

/**
 * @brief Dispatch ray intersection based on object type.
 *
 * Calls the appropriate intersection routine for the object's geometry.
 *
 * @param ray Ray to test.
 * @param obj Scene object to intersect.
 * @param hit Hit record to update.
 * @return int 1 if an intersection is found, 0 otherwise.
 */
int	intersect_object_new(t_ray *ray, t_object *obj, t_hit_record *hit)
{
	int	result;

	result = 0;
	if (obj->type == OBJ_SPHERE)
		result = intersect_sphere_new(ray, &obj->data.sphere, obj->color, hit);
	else if (obj->type == OBJ_PLANE)
		result = intersect_plane_new(ray, &obj->data.plane, obj->color, hit);
	else if (obj->type == OBJ_CYLINDER)
		result = intersect_cylinder_new(ray, &obj->data.cylinder, obj->color,
				hit);
	else if (obj->type == OBJ_CONE)
		result = intersect_cone_new(ray, obj, hit);
	if (result)
		hit->obj = obj;
	return (result);
}
