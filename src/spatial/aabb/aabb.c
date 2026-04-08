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

#include "spatial/spatial.h"

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
 * @brief Compute slab intersection for one axis using precomputed inv_dir.
 *
 * Uses IEEE 754 infinity for parallel rays: when direction is zero,
 * inv_dir is ±inf and the math naturally handles hit/miss cases.
 *
 * @param ac Axis check parameters with precomputed inv_dir.
 */
static void	safe_slab_axis(t_axis_check *ac)
{
	double	t0;
	double	t1;

	t0 = (ac->box_min - ac->ray_origin) * ac->inv_dir;
	t1 = (ac->box_max - ac->ray_origin) * ac->inv_dir;
	update_bounds(ac->tmin, ac->tmax, t0, t1);
}

/**
 * @brief Test a ray for intersection with an axis-aligned bounding box.
 *
 * Uses the slab method to compute intersection interval and updates
 * t_min/t_max to the overlapping range. Handles zero direction components.
 *
 * @param box AABB to test.
 * @param ray Ray to test.
 * @param t_min Input/output minimum t value.
 * @param t_max Input/output maximum t value.
 * @return int 1 if the ray intersects the box, 0 otherwise.
 */
int	aabb_intersect(t_aabb box, t_ray ray, double *t_min, double *t_max)
{
	t_axis_check	ac;
	double			tmin;
	double			tmax;

	tmin = *t_min;
	tmax = *t_max;
	ac = (t_axis_check){box.min.x, box.max.x, ray.origin.x,
		ray.inv_dir.x, &tmin, &tmax};
	safe_slab_axis(&ac);
	ac = (t_axis_check){box.min.y, box.max.y, ray.origin.y,
		ray.inv_dir.y, &tmin, &tmax};
	safe_slab_axis(&ac);
	ac = (t_axis_check){box.min.z, box.max.z, ray.origin.z,
		ray.inv_dir.z, &tmin, &tmax};
	safe_slab_axis(&ac);
	*t_min = tmin;
	*t_max = tmax;
	return (tmax >= tmin && tmax > 0);
}
