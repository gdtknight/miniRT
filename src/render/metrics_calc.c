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

double	calculate_fps(t_metrics *m)
{
	return (m->timing.fps);
}

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

double	calculate_avg_tests_per_ray(t_metrics *m)
{
	if (m->ray.rays_traced == 0)
		return (0.0);
	return ((double)m->ray.intersect_tests / m->ray.rays_traced);
}
