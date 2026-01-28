/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metrics_frame.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "metrics.h"

static void	init_frame_timing(t_frame_timing *timing)
{
	int	i;

	timing->render_time_us = 0;
	timing->frame_count = 0;
	timing->fps = 0.0;
	timing->frame_index = 0;
	i = 0;
	while (i < FRAME_HISTORY_SIZE)
	{
		timing->frame_times_us[i] = 0;
		i++;
	}
}

void	metrics_init(t_metrics *metrics)
{
	init_frame_timing(&metrics->timing);
	metrics->ray.rays_traced = 0;
	metrics->ray.intersect_tests = 0;
	metrics->bvh.nodes_visited = 0;
	metrics->bvh.tests_skipped = 0;
	metrics->bvh.box_tests = 0;
	metrics->quality_mode = 0;
}

void	metrics_start_frame(t_metrics *metrics)
{
	metrics->ray.rays_traced = 0;
	metrics->ray.intersect_tests = 0;
	metrics->bvh.nodes_visited = 0;
	metrics->bvh.tests_skipped = 0;
	metrics->bvh.box_tests = 0;
	timer_start(&metrics->timing.start_time);
}

static double	calculate_fps_internal(t_frame_timing *t)
{
	long	sum;
	int		i;
	int		valid;

	sum = 0;
	valid = 0;
	i = 0;
	while (i < FRAME_HISTORY_SIZE)
	{
		if (t->frame_times_us[i] > 0)
		{
			sum += t->frame_times_us[i];
			valid++;
		}
		i++;
	}
	if (valid == 0 || sum == 0)
		return (0.0);
	return (1000000.0 / (sum / (double)valid));
}

void	metrics_end_frame(t_metrics *metrics)
{
	t_frame_timing	*t;

	t = &metrics->timing;
	t->render_time_us = timer_elapsed_us(&t->start_time);
	t->frame_times_us[t->frame_index] = t->render_time_us;
	t->frame_index = (t->frame_index + 1) % FRAME_HISTORY_SIZE;
	t->frame_count++;
	t->fps = calculate_fps_internal(t);
}

void	metrics_log_render(t_metrics *metrics)
{
	(void)metrics;
}
