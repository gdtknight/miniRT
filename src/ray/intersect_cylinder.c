/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersect_cylinder.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 16:20:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 16:20:01 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "ray.h"
#include "vec3.h"
#include <math.h>

/* Helper structure for cylinder intersection calculations */
typedef struct s_cyl_calc
{
	double	a;
	double	b;
	double	c;
	double	discriminant;
	double	t;
	double	m;
}	t_cyl_calc;

/*
** Calculate ray-cylinder infinite surface intersection.
** Uses quadratic equation excluding cylinder caps.
** Returns 1 if intersection found within cylinder height, 0 otherwise.
*/
static int	calculate_cylinder_intersection(t_ray *ray, t_cylinder *cyl, \
		t_cyl_calc *calc)
{
	t_vec3	oc;
	double	dir_dot_axis;
	double	oc_dot_axis;
	double	dir_dot_dir;
	double	oc_dot_oc;

	oc = vec3_subtract(ray->origin, cyl->center);
	dir_dot_axis = vec3_dot(ray->direction, cyl->axis);
	oc_dot_axis = vec3_dot(oc, cyl->axis);
	dir_dot_dir = vec3_dot(ray->direction, ray->direction);
	oc_dot_oc = vec3_dot(oc, oc);
	calc->a = dir_dot_dir - dir_dot_axis * dir_dot_axis;
	calc->b = 2.0 * (vec3_dot(ray->direction, oc) - \
		dir_dot_axis * oc_dot_axis);
	calc->c = oc_dot_oc - oc_dot_axis * oc_dot_axis - cyl->radius_squared;
	calc->discriminant = calc->b * calc->b - 4 * calc->a * calc->c;
	if (calc->discriminant < 0 || calc->a < EPSILON)
		return (0);
	calc->t = (-calc->b - sqrt(calc->discriminant)) / (2.0 * calc->a);
	if (calc->t < 0.001)
		calc->t = (-calc->b + sqrt(calc->discriminant)) / (2.0 * calc->a);
	calc->m = dir_dot_axis * calc->t + oc_dot_axis;
	return (1);
}

/*
** Check ray intersection with cylinder cap (top or bottom disc).
** Returns 1 if hit and updates hit info, 0 otherwise.
*/
int	intersect_cylinder_cap(t_ray *ray, t_cylinder *cyl, t_hit *hit, \
		double cap_m)
{
	t_vec3	cap_center;
	double	denom;
	double	t;
	t_vec3	p;
	double	dist_sq;

	denom = vec3_dot(ray->direction, cyl->axis);
	if (fabs(denom) < EPSILON)
		return (0);
	cap_center = vec3_add(cyl->center, vec3_multiply(cyl->axis, cap_m));
	t = vec3_dot(vec3_subtract(cap_center, ray->origin), cyl->axis) / denom;
	if (t < 0.001 || t > hit->distance)
		return (0);
	p = vec3_add(ray->origin, vec3_multiply(ray->direction, t));
	dist_sq = vec3_dot(vec3_subtract(p, cap_center), \
		vec3_subtract(p, cap_center));
	if (dist_sq > cyl->radius_squared)
		return (0);
	hit->distance = t;
	hit->point = p;
	hit->normal = cyl->axis;
	if (vec3_dot(ray->direction, hit->normal) > 0)
		hit->normal = vec3_multiply(hit->normal, -1.0);
	hit->color = cyl->color;
	return (1);
}

/*
** Test ray-cylinder body intersection (side surface).
** Returns 1 if hit and closer than current hit->distance, 0 otherwise.
*/
int	intersect_cylinder_body(t_ray *ray, t_cylinder *cyl, t_hit *hit)
{
	t_cyl_calc	calc;
	t_vec3		hit_point;
	t_vec3		axis_point;

	if (!calculate_cylinder_intersection(ray, cyl, &calc))
		return (0);
	if (calc.t < 0.001 || calc.t > hit->distance)
		return (0);
	hit_point = vec3_add(ray->origin, vec3_multiply(ray->direction, calc.t));
	if (calc.m < -cyl->half_height || calc.m > cyl->half_height)
		return (0);
	hit->distance = calc.t;
	hit->point = hit_point;
	axis_point = vec3_add(cyl->center, vec3_multiply(cyl->axis, calc.m));
	hit->normal = vec3_normalize(vec3_subtract(hit->point, axis_point));
	hit->color = cyl->color;
	return (1);
}

/*
** Test ray-cylinder intersection including body and caps.
** Checks body, top cap, and bottom cap, returns closest hit.
** Returns 1 if hit and closer than current hit->distance, 0 otherwise.
*/
int	intersect_cylinder(t_ray *ray, t_cylinder *cylinder, t_hit *hit)
{
	int		hit_found;
	t_hit	temp_hit;

	hit_found = 0;
	temp_hit.distance = hit->distance;
	if (intersect_cylinder_body(ray, cylinder, &temp_hit))
	{
		*hit = temp_hit;
		hit_found = 1;
	}
	temp_hit.distance = hit->distance;
	if (intersect_cylinder_cap(ray, cylinder, &temp_hit, \
		cylinder->half_height))
	{
		*hit = temp_hit;
		hit_found = 1;
	}
	temp_hit.distance = hit->distance;
	if (intersect_cylinder_cap(ray, cylinder, &temp_hit, \
		-cylinder->half_height))
	{
		*hit = temp_hit;
		hit_found = 1;
	}
	return (hit_found);
}
