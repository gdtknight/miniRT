/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersect_cylinder.c                               :+:      :+:    :+:   */
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

/**
 * @brief Compute cylinder intersection quadratic parameters.
 *
 * Calculates coefficients and discriminant for the infinite cylinder and
 * stores intermediate values in the calc struct.
 *
 * @param ray Ray to test.
 * @param c Cylinder data.
 * @param calc Output calculation structure.
 * @return int 1 if solvable, 0 if no intersection.
 */
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
	if (calc->discriminant < 0 || fabs(calc->a) < EPSILON)
		return (0);
	calc->t = (-calc->b - sqrt(calc->discriminant)) / (2.0 * calc->a);
	calc->t2 = (-calc->b + sqrt(calc->discriminant)) / (2.0 * calc->a);
	calc->m = dir_axis * calc->t + oc_axis;
	calc->m2 = dir_axis * calc->t2 + oc_axis;
	return (1);
}

/**
 * @brief Intersect a ray with a cylinder end cap.
 *
 * Computes intersection with the cap plane and checks radial bounds.
 *
 * @param ray Ray to test.
 * @param c Cylinder data.
 * @param hit Hit record to update.
 * @param cap_m Offset along the axis for the cap center.
 * @return int 1 if a valid cap hit is found, 0 otherwise.
 */
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
	if (t < RAY_T_MIN || t > hit->distance)
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

/**
 * @brief Intersect a ray with the finite cylinder body.
 *
 * Solves the quadratic for the infinite cylinder and validates that the hit
 * lies within the cylinder's height bounds.
 *
 * @param ray Ray to test.
 * @param c Cylinder data.
 * @param hit Hit record to update.
 * @return int 1 if a valid body hit is found, 0 otherwise.
 */
static int	intersect_cyl_body_new(t_ray *ray, t_cylinder_data *c, t_hit *hit)
{
	t_cyl_calc	calc;

	if (!calc_cyl_intersect(ray, c, &calc))
		return (0);
	if (calc.t < RAY_T_MIN || calc.t > hit->distance
		|| calc.m < -c->half_height || calc.m > c->half_height)
	{
		calc.t = calc.t2;
		calc.m = calc.m2;
		if (calc.t < RAY_T_MIN || calc.t > hit->distance
			|| calc.m < -c->half_height || calc.m > c->half_height)
			return (0);
	}
	hit->distance = calc.t;
	hit->point = vec3_add(ray->origin, vec3_multiply(ray->direction, calc.t));
	hit->normal = vec3_subtract(hit->point,
			vec3_add(c->center, vec3_multiply(c->axis, calc.m)));
	if (vec3_dot(hit->normal, hit->normal) < EPSILON * EPSILON)
		return (0);
	hit->normal = vec3_normalize(hit->normal);
	if (vec3_dot(ray->direction, hit->normal) > 0)
		hit->normal = vec3_multiply(hit->normal, -1.0);
	return (1);
}

/**
 * @brief Apply a cylinder hit result to the final hit record.
 *
 * Copies the temporary hit, applies color, and marks the found flag.
 *
 * @param hit Output hit record.
 * @param temp Temporary hit record.
 * @param color Surface color.
 * @param found Output flag indicating a hit was found.
 */
static void	apply_cyl_hit(t_hit *hit, t_hit *temp, t_color color, int *found)
{
	temp->color = color;
	*hit = *temp;
	*found = 1;
}

/**
 * @brief Intersect a ray with a finite cylinder (body + caps).
 *
 * Tests the cylindrical body and both end caps, updating the closest hit.
 *
 * @param ray Ray to test.
 * @param c Cylinder data.
 * @param color Surface color.
 * @param hit Hit record to update.
 * @return int 1 if any intersection is found, 0 otherwise.
 */
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
