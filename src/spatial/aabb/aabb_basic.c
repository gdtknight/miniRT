/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb_basic.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:32:24 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial/spatial.h"

/**
 * @brief Return the smaller of two doubles (NaN-safe).
 *
 * If either operand is NaN (detected via self-comparison), the other
 * operand is returned.  This avoids propagating NaN through AABB merges.
 *
 * @param a First value.
 * @param b Second value.
 * @return double Minimum of a and b.
 */
double	min_double(double a, double b)
{
	if (a != a)
		return (b);
	if (b != b)
		return (a);
	if (a < b)
		return (a);
	return (b);
}

/**
 * @brief Return the larger of two doubles (NaN-safe).
 *
 * If either operand is NaN (detected via self-comparison), the other
 * operand is returned.  This avoids propagating NaN through AABB merges.
 *
 * @param a First value.
 * @param b Second value.
 * @return double Maximum of a and b.
 */
double	max_double(double a, double b)
{
	if (a != a)
		return (b);
	if (b != b)
		return (a);
	if (a > b)
		return (a);
	return (b);
}

/**
 * @brief Create an AABB from min and max corners.
 *
 * @param min Minimum corner.
 * @param max Maximum corner.
 * @return t_aabb Constructed bounding box.
 */
t_aabb	aabb_create(t_vec3 min, t_vec3 max)
{
	t_aabb	box;

	box.min = min;
	box.max = max;
	return (box);
}

/**
 * @brief Merge two AABBs into a bounding box containing both.
 *
 * @param a First AABB.
 * @param b Second AABB.
 * @return t_aabb Merged bounding box.
 */
t_aabb	aabb_merge(t_aabb a, t_aabb b)
{
	t_aabb	result;

	result.min.x = min_double(a.min.x, b.min.x);
	result.min.y = min_double(a.min.y, b.min.y);
	result.min.z = min_double(a.min.z, b.min.z);
	result.max.x = max_double(a.max.x, b.max.x);
	result.max.y = max_double(a.max.y, b.max.y);
	result.max.z = max_double(a.max.z, b.max.z);
	return (result);
}
