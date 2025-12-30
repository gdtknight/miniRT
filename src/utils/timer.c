/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 21:30:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "metrics.h"
#include <stdlib.h>

void	timer_start(struct timeval *tv)
{
	gettimeofday(tv, 0);
}

long	timer_elapsed_us(struct timeval *start)
{
	struct timeval	now;
	long			elapsed;

	gettimeofday(&now, 0);
	elapsed = (now.tv_sec - start->tv_sec) * 1000000L;
	elapsed += (now.tv_usec - start->tv_usec);
	return (elapsed);
}

double	timer_elapsed_ms(struct timeval *start)
{
	return (timer_elapsed_us(start) / 1000.0);
}

void	metrics_init(t_metrics *metrics)
{
	metrics->render_time_us = 0;
	metrics->frame_count = 0;
	metrics->fps = 0.0;
	metrics->rays_traced = 0;
	metrics->intersect_tests = 0;
	metrics->bvh_nodes_visited = 0;
	metrics->bvh_tests_skipped = 0;
	metrics->quality_mode = 1;
}

void	metrics_start_frame(t_metrics *metrics)
{
	timer_start(&metrics->start_time);
}

void	metrics_end_frame(t_metrics *metrics)
{
	metrics->render_time_us = timer_elapsed_us(&metrics->start_time);
	metrics->frame_count++;
	if (metrics->render_time_us > 0)
		metrics->fps = 1000000.0 / metrics->render_time_us;
}

void	metrics_log_render(t_metrics *metrics)
{
	(void)metrics;
}

void	metrics_add_ray(t_metrics *metrics)
{
	metrics->rays_traced++;
}

void	metrics_add_intersect_test(t_metrics *metrics)
{
	metrics->intersect_tests++;
}

void	metrics_add_bvh_node_visit(t_metrics *metrics)
{
	metrics->bvh_nodes_visited++;
}

void	metrics_add_bvh_skip(t_metrics *metrics)
{
	metrics->bvh_tests_skipped++;
}
