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
#include <stdio.h>

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

/**
 * @brief Print a summary of all render metrics to stdout.
 *
 * Outputs frame timing, ray counts, intersection stats, and BVH counters
 * for benchmarking and profiling purposes.
 *
 * @param m Metrics structure containing all counter data.
 * @param object_count Number of objects in the scene.
 */
void	metrics_print_summary(t_metrics *m, int object_count)
{
	double	tests_per_ray;
	double	bvh_eff;

	tests_per_ray = calculate_avg_tests_per_ray(m);
	bvh_eff = calculate_bvh_efficiency(m, object_count);
	printf("\n=== Render Metrics ===\n");
	printf("Frame time:        %.1f ms\n", m->timing.render_time_us / 1000.0);
	printf("FPS:               %.1f\n", m->timing.fps);
	printf("Rays traced:       %ld\n", m->ray.rays_traced);
	printf("Intersect tests:   %ld\n", m->ray.intersect_tests);
	printf("Tests/ray:         %.1f\n", tests_per_ray);
	printf("BVH nodes visited: %ld\n", m->bvh.nodes_visited);
	printf("BVH box tests:     %ld\n", m->bvh.box_tests);
	printf("BVH efficiency:    %.1f%%\n", bvh_eff);
	printf("Objects:           %d\n", object_count);
	printf("=======================\n\n");
	fflush(stdout);
}
