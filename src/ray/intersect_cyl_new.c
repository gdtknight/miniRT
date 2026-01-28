/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersect_cyl_new.c                                :+:      :+:    :+:   */
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

static int	calc_cyl_intersect(t_ray *ray, t_cylinder_data *c, t_cyl_calc *calc)
{
	t_vec3	oc;
	double	dir_axis;
	double	oc_axis;

	oc = vec3_subtract(ray->origin, c->center);
	dir_axis = vec3_dot(ray->direction, c->axis);
	oc_axis = vec3_dot(oc, c->axis);
	calc->a = vec3_dot(ray->direction, ray->direction) - dir_axis * dir_axis;
	calc->b = 2.0 * (vec3_dot(ray->direction, oc) - dir_axis * oc_axis);
	calc->c = vec3_dot(oc, oc) - oc_axis * oc_axis - c->radius_sq;
	calc->discriminant = calc->b * calc->b - 4 * calc->a * calc->c;
	if (calc->discriminant < 0 || calc->a < EPSILON)
		return (0);
	calc->t = (-calc->b - sqrt(calc->discriminant)) / (2.0 * calc->a);
	if (calc->t < 0.001)
		calc->t = (-calc->b + sqrt(calc->discriminant)) / (2.0 * calc->a);
	calc->m = dir_axis * calc->t + oc_axis;
	return (1);
}

static int	intersect_cyl_cap_new(t_ray *ray, t_cylinder_data *c, t_hit *hit,
		double cap_m)
{
	t_vec3	cap_center;
	double	denom;
	double	t;
	t_vec3	p;

	denom = vec3_dot(ray->direction, c->axis);
	if (fabs(denom) < EPSILON)
		return (0);
	cap_center = vec3_add(c->center, vec3_multiply(c->axis, cap_m));
	t = vec3_dot(vec3_subtract(cap_center, ray->origin), c->axis) / denom;
	if (t < 0.001 || t > hit->distance)
		return (0);
	p = vec3_add(ray->origin, vec3_multiply(ray->direction, t));
	if (vec3_dot(vec3_subtract(p, cap_center),
			vec3_subtract(p, cap_center)) > c->radius_sq)
		return (0);
	hit->distance = t;
	hit->point = p;
	hit->normal = c->axis;
	if (vec3_dot(ray->direction, hit->normal) > 0)
		hit->normal = vec3_multiply(hit->normal, -1.0);
	return (1);
}

static int	intersect_cyl_body_new(t_ray *ray, t_cylinder_data *c, t_hit *hit)
{
	t_cyl_calc	calc;
	t_vec3		axis_pt;

	if (!calc_cyl_intersect(ray, c, &calc))
		return (0);
	if (calc.t < 0.001 || calc.t > hit->distance)
		return (0);
	if (calc.m < -c->half_height || calc.m > c->half_height)
		return (0);
	hit->distance = calc.t;
	hit->point = vec3_add(ray->origin, vec3_multiply(ray->direction, calc.t));
	axis_pt = vec3_add(c->center, vec3_multiply(c->axis, calc.m));
	hit->normal = vec3_normalize(vec3_subtract(hit->point, axis_pt));
	if (vec3_dot(ray->direction, hit->normal) > 0)
		hit->normal = vec3_multiply(hit->normal, -1.0);
	return (1);
}

static void	apply_cyl_hit(t_hit *hit, t_hit *temp, t_color color, int *found)
{
	temp->color = color;
	*hit = *temp;
	*found = 1;
}

int	intersect_cylinder_new(t_ray *ray, t_cylinder_data *c, t_color color,
		t_hit *hit)
{
	int		found;
	t_hit	temp;

	found = 0;
	temp.distance = hit->distance;
	if (intersect_cyl_body_new(ray, c, &temp))
		apply_cyl_hit(hit, &temp, color, &found);
	temp.distance = hit->distance;
	if (intersect_cyl_cap_new(ray, c, &temp, c->half_height))
		apply_cyl_hit(hit, &temp, color, &found);
	temp.distance = hit->distance;
	if (intersect_cyl_cap_new(ray, c, &temp, -c->half_height))
		apply_cyl_hit(hit, &temp, color, &found);
	return (found);
}
