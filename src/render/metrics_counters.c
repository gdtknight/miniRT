/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metrics_counters.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "metrics.h"

/**
 * @brief Increment the traced ray counter.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_add_ray(t_metrics *metrics)
{
	if (metrics)
		metrics->ray.rays_traced++;
}

/**
 * @brief Increment the intersection test counter.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_add_intersect_test(t_metrics *metrics)
{
	if (metrics)
		metrics->ray.intersect_tests++;
}

/**
 * @brief Increment the BVH node visit counter.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_add_bvh_node_visit(t_metrics *metrics)
{
	if (metrics)
		metrics->bvh.nodes_visited++;
}

/**
 * @brief Increment the BVH skip counter.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_add_bvh_skip(t_metrics *metrics)
{
	if (metrics)
		metrics->bvh.tests_skipped++;
}

/**
 * @brief Increment the BVH bounding box test counter.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_add_bvh_box_test(t_metrics *metrics)
{
	if (metrics)
		metrics->bvh.box_tests++;
}
