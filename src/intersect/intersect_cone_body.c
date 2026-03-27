/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersect_cone_body.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/10 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "intersect.h"
#include "vec3.h"
#include "scene.h"
#include <math.h>

static int	calc_cone_intersect(t_ray *ray, t_cone_data *c, t_cyl_calc *calc)
{
	t_vec3	d;
	double	dir_axis;
	double	d_axis;
	double	k2;

	k2 = c->radius / (2.0 * c->half_height);
	if (k2 > 1000.0)
		k2 = 1000.0;
	k2 = 1.0 + k2 * k2;
	d = vec3_subtract(ray->origin, vec3_add(c->center,
				vec3_multiply(c->axis, c->half_height)));
	dir_axis = vec3_dot(ray->direction, c->axis);
	d_axis = vec3_dot(d, c->axis);
	calc->a = vec3_dot(ray->direction, ray->direction)
		- k2 * dir_axis * dir_axis;
	calc->b = 2.0 * (vec3_dot(ray->direction, d) - k2 * dir_axis * d_axis);
	calc->c = vec3_dot(d, d) - k2 * d_axis * d_axis;
	calc->discriminant = calc->b * calc->b - 4 * calc->a * calc->c;
	if (calc->discriminant < 0 || fabs(calc->a) < EPSILON)
		return (0);
	calc->t = (-calc->b - sqrt(calc->discriminant)) / (2.0 * calc->a);
	calc->t2 = (-calc->b + sqrt(calc->discriminant)) / (2.0 * calc->a);
	calc->m = d_axis + calc->t * dir_axis;
	calc->m2 = d_axis + calc->t2 * dir_axis;
	return (1);
}

static int	set_cone_hit(t_ray *ray, t_cone_data *c,
				t_hit *hit, t_cyl_calc *calc)
{
	t_vec3	apex;
	t_vec3	normal;
	double	k;

	hit->distance = calc->t;
	hit->point = vec3_add(ray->origin,
			vec3_multiply(ray->direction, calc->t));
	apex = vec3_add(c->center, vec3_multiply(c->axis, c->half_height));
	normal = vec3_subtract(hit->point, apex);
	k = c->radius / (2.0 * c->half_height);
	normal = vec3_subtract(normal,
			vec3_multiply(c->axis, calc->m * (1.0 + k * k)));
	if (vec3_dot(normal, normal) < EPSILON * EPSILON)
		return (0);
	hit->normal = vec3_normalize(normal);
	if (vec3_dot(ray->direction, hit->normal) > 0)
		hit->normal = vec3_multiply(hit->normal, -1.0);
	return (1);
}

static int	validate_cone_hit(t_ray *ray, t_cone_data *c,
				t_hit *hit, t_cyl_calc *calc)
{
	if (calc->t2 < RAY_T_MIN || calc->t2 > hit->distance)
		return (0);
	if (calc->m2 < -2.0 * c->half_height || calc->m2 > 0.0)
		return (0);
	calc->t = calc->t2;
	calc->m = calc->m2;
	if (calc->m > -RAY_T_MIN)
		calc->m = -RAY_T_MIN;
	return (set_cone_hit(ray, c, hit, calc));
}

int	intersect_cone_body(t_ray *ray, t_cone_data *c, t_hit *hit)
{
	t_cyl_calc	calc;

	if (c->half_height < EPSILON)
		return (0);
	if (!calc_cone_intersect(ray, c, &calc))
		return (0);
	if (calc.t < RAY_T_MIN || calc.t > hit->distance)
		return (validate_cone_hit(ray, c, hit, &calc));
	if (calc.m < -2.0 * c->half_height || calc.m > 0.0)
		return (validate_cone_hit(ray, c, hit, &calc));
	if (calc.m > -RAY_T_MIN)
		calc.m = -RAY_T_MIN;
	return (set_cone_hit(ray, c, hit, &calc));
}
