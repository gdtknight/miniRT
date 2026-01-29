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

void	metrics_add_ray(t_metrics *metrics)
{
	if (metrics)
		metrics->ray.rays_traced++;
}

void	metrics_add_intersect_test(t_metrics *metrics)
{
	if (metrics)
		metrics->ray.intersect_tests++;
}

void	metrics_add_bvh_node_visit(t_metrics *metrics)
{
	if (metrics)
		metrics->bvh.nodes_visited++;
}

void	metrics_add_bvh_skip(t_metrics *metrics)
{
	if (metrics)
		metrics->bvh.tests_skipped++;
}

void	metrics_add_bvh_box_test(t_metrics *metrics)
{
	if (metrics)
		metrics->bvh.box_tests++;
}
