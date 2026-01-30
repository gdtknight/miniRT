/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metrics_calc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/31 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "metrics.h"

/**
 * @brief Return the current FPS estimate.
 *
 * @param m Metrics structure containing timing data.
 * @return double FPS value.
 */
double	calculate_fps(t_metrics *m)
{
	return (m->timing.fps);
}

/**
 * @brief Calculate BVH efficiency as a percentage.
 *
 * Compares actual intersection tests against the naive tests (rays * objects)
 * and returns a higher percentage for better efficiency.
 *
 * @param m Metrics structure containing ray and intersect counts.
 * @param object_count Number of objects in the scene.
 * @return double Efficiency percentage in [0, 100].
 */
double	calculate_bvh_efficiency(t_metrics *m, int object_count)
{
	long	naive_tests;
	double	efficiency;

	if (object_count == 0 || m->ray.rays_traced == 0)
		return (0.0);
	naive_tests = m->ray.rays_traced * object_count;
	if (naive_tests == 0)
		return (0.0);
	efficiency = 1.0 - ((double)m->ray.intersect_tests / naive_tests);
	if (efficiency < 0.0)
		return (0.0);
	return (efficiency * 100.0);
}

/**
 * @brief Calculate average intersection tests per traced ray.
 *
 * @param m Metrics structure containing ray and intersect counts.
 * @return double Average tests per ray (0 if no rays).
 */
double	calculate_avg_tests_per_ray(t_metrics *m)
{
	if (m->ray.rays_traced == 0)
		return (0.0);
	return ((double)m->ray.intersect_tests / m->ray.rays_traced);
}

/**
 * @brief Reset BVH-related metrics counters.
 *
 * @param bvh BVH metrics structure to reset.
 */
void	metrics_reset_bvh(t_bvh_metrics *bvh)
{
	if (bvh)
	{
		bvh->nodes_visited = 0;
		bvh->tests_skipped = 0;
		bvh->box_tests = 0;
	}
}
