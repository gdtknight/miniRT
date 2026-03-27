/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersect_cone_cap.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/03/22 16:23:18 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "intersect.h"
#include "scene.h"
#include <math.h>

static int	intersect_cone_cap(t_ray *ray, t_cone_data *c, t_hit *hit)
{
	t_vec3	cap_center;
	double	denom;
	double	t;
	t_vec3	p;

	denom = vec3_dot(ray->direction, c->axis);
	if (fabs(denom) < EPSILON)
		return (0);
	cap_center = vec3_subtract(c->center,
			vec3_multiply(c->axis, c->half_height));
	t = vec3_dot(vec3_subtract(cap_center, ray->origin), c->axis) / denom;
	if (t < RAY_T_MIN || t > hit->distance)
		return (0);
	p = vec3_add(ray->origin, vec3_multiply(ray->direction, t));
	if (vec3_dot(vec3_subtract(p, cap_center),
			vec3_subtract(p, cap_center)) > c->radius_sq)
		return (0);
	hit->distance = t;
	hit->point = p;
	hit->normal = vec3_multiply(c->axis, -1.0);
	if (vec3_dot(ray->direction, hit->normal) > 0)
		hit->normal = vec3_multiply(hit->normal, -1.0);
	return (1);
}

static void	apply_cone_hit(t_hit *hit, t_hit *temp, t_color color, int *found)
{
	temp->color = color;
	*hit = *temp;
	*found = 1;
}

int	intersect_cone(t_ray *ray, t_object *obj, t_hit *hit)
{
	int		found;
	t_hit	temp;

	found = 0;
	temp.distance = hit->distance;
	if (intersect_cone_body(ray, &obj->data.cone, &temp))
		apply_cone_hit(hit, &temp, obj->color, &found);
	temp.distance = hit->distance;
	if (intersect_cone_cap(ray, &obj->data.cone, &temp))
		apply_cone_hit(hit, &temp, obj->color, &found);
	return (found);
}
