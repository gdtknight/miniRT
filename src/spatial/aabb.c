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
#include <math.h>

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
 * @brief Compute slab intersection for one axis with div-by-zero safety.
 *
 * When the ray direction component is near zero, the ray is parallel to
 * the slab. If origin is outside the slab, force a miss.
 *
 * @param ac Axis check parameters.
 */
static void	safe_slab_axis(t_axis_check *ac)
{
	double	inv_d;
	double	t0;
	double	t1;

	if (fabs(ac->ray_dir) < 1e-8)
	{
		if (ac->ray_origin < ac->box_min || ac->ray_origin > ac->box_max)
		{
			*ac->tmin = 1e30;
			*ac->tmax = -1e30;
		}
		return ;
	}
	inv_d = 1.0 / ac->ray_dir;
	t0 = (ac->box_min - ac->ray_origin) * inv_d;
	t1 = (ac->box_max - ac->ray_origin) * inv_d;
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
		ray.direction.x, &tmin, &tmax};
	safe_slab_axis(&ac);
	ac = (t_axis_check){box.min.y, box.max.y, ray.origin.y,
		ray.direction.y, &tmin, &tmax};
	safe_slab_axis(&ac);
	ac = (t_axis_check){box.min.z, box.max.z, ray.origin.z,
		ray.direction.z, &tmin, &tmax};
	safe_slab_axis(&ac);
	*t_min = tmin;
	*t_max = tmax;
	return (tmax >= tmin && tmax > 0);
}
