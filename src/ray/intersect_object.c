/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersect_object.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "ray.h"
#include "vec3.h"
#include <math.h>

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
	if (d < 0)
		return (0);
	d = (-b - sqrt(d)) / (2.0 * a);
	if (d < 0.001)
		d = (-b + sqrt(b * b - 4 * a * c)) / (2.0 * a);
	if (d < 0.001 || d > hit->distance)
		return (0);
	hit->distance = d;
	hit->point = vec3_add(ray->origin, vec3_multiply(ray->direction, d));
	hit->normal = vec3_normalize(vec3_subtract(hit->point, s->center));
	hit->color = color;
	return (1);
}

static int	intersect_plane_new(t_ray *ray, t_plane_data *p, t_color color,
		t_hit *hit)
{
	double	denom;
	double	t;
	t_vec3	p0l0;

	denom = vec3_dot(p->normal, ray->direction);
	if (fabs(denom) < 0.0001)
		return (0);
	p0l0 = vec3_subtract(p->point, ray->origin);
	t = vec3_dot(p0l0, p->normal) / denom;
	if (t < 0.001 || t > hit->distance)
		return (0);
	hit->distance = t;
	hit->point = vec3_add(ray->origin, vec3_multiply(ray->direction, t));
	hit->normal = p->normal;
	if (vec3_dot(ray->direction, hit->normal) > 0)
		hit->normal = vec3_multiply(hit->normal, -1);
	hit->color = color;
	return (1);
}

int	intersect_cylinder_new(t_ray *ray, t_cylinder_data *c, t_color color,
		t_hit *hit);

int	intersect_object_new(t_ray *ray, t_object *obj, t_hit_record *hit)
{
	if (obj->type == OBJ_SPHERE)
		return (intersect_sphere_new(ray, &obj->data.sphere, obj->color, hit));
	else if (obj->type == OBJ_PLANE)
		return (intersect_plane_new(ray, &obj->data.plane, obj->color, hit));
	else if (obj->type == OBJ_CYLINDER)
		return (intersect_cylinder_new(ray, &obj->data.cylinder, obj->color,
				hit));
	return (0);
}
