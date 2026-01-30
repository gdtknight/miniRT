/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:32:29 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"

/**
 * @brief Update ray slab intersection bounds for one axis.
 *
 * Swaps and clamps tmin/tmax based on the current axis intersection values.
 *
 * @param tmin Current minimum t value (in/out).
 * @param tmax Current maximum t value (in/out).
 * @param t0 Intersection t for slab near plane.
 * @param t1 Intersection t for slab far plane.
 */
static void	update_bounds(double *tmin, double *tmax, double t0, double t1)
{
	if (t0 > t1)
	{
		*tmin = max_double(*tmin, t1);
		*tmax = min_double(*tmax, t0);
	}
	else
	{
		*tmin = max_double(*tmin, t0);
		*tmax = min_double(*tmax, t1);
	}
}

/**
 * @brief Test a ray for intersection with an axis-aligned bounding box.
 *
 * Uses the slab method to compute intersection interval and updates
 * t_min/t_max to the overlapping range.
 *
 * @param box AABB to test.
 * @param ray Ray to test.
 * @param t_min Input/output minimum t value.
 * @param t_max Input/output maximum t value.
 * @return int 1 if the ray intersects the box, 0 otherwise.
 */
int	aabb_intersect(t_aabb box, t_ray ray, double *t_min, double *t_max)
{
	double	t[2];
	double	tmin;
	double	tmax;

	tmin = *t_min;
	tmax = *t_max;
	t[0] = (box.min.x - ray.origin.x) / ray.direction.x;
	t[1] = (box.max.x - ray.origin.x) / ray.direction.x;
	update_bounds(&tmin, &tmax, t[0], t[1]);
	t[0] = (box.min.y - ray.origin.y) / ray.direction.y;
	t[1] = (box.max.y - ray.origin.y) / ray.direction.y;
	update_bounds(&tmin, &tmax, t[0], t[1]);
	t[0] = (box.min.z - ray.origin.z) / ray.direction.z;
	t[1] = (box.max.z - ray.origin.z) / ray.direction.z;
	update_bounds(&tmin, &tmax, t[0], t[1]);
	*t_min = tmin;
	*t_max = tmax;
	return (tmax >= tmin && tmax > 0);
}
