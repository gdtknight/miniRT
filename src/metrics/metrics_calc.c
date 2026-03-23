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
 * @brief Calculate BVH skip rate as a percentage.
 *
 * Returns the ratio of skipped nodes to total visited nodes, expressed
 * as a percentage. Higher values indicate more effective early-out.
 *
 * @param m Metrics structure containing BVH traversal counts.
 * @return double Skip rate percentage in [0, 100].
 */
double	calculate_bvh_efficiency(t_metrics *m)
{
	if (m->bvh.nodes_visited == 0)
		return (0.0);
	return ((double)m->bvh.tests_skipped / m->bvh.nodes_visited * 100.0);
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
	bvh_eff = calculate_bvh_efficiency(m);
	printf("\n=== Render Metrics ===\n");
	printf("Frame time:        %.1f ms\n", m->timing.render_time_us / 1000.0);
	printf("FPS:               %.4f\n", m->timing.fps);
	printf("Rays traced:       %ld\n", m->ray.rays_traced);
	printf("Primary tests:     %ld\n", m->ray.intersect_tests);
	printf("Shadow tests:      %ld\n", m->ray.shadow_intersect_tests);
	printf("Primary tests/ray: %.1f\n", tests_per_ray);
	printf("BVH nodes visited: %ld\n", m->bvh.nodes_visited);
	printf("BVH skip rate:     %.1f%%\n", bvh_eff);
	printf("Objects:           %d\n", object_count);
	printf("=======================\n\n");
	fflush(stdout);
}
