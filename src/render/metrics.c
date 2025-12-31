/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metrics.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/31 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/31 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "metrics.h"
#include <sys/time.h>
#include <stdlib.h>

void	timer_start(struct timeval *tv)
{
	gettimeofday(tv, NULL);
}

long	timer_elapsed_us(struct timeval *start)
{
	struct timeval	end;
	long			seconds;
	long			microseconds;

	gettimeofday(&end, NULL);
	seconds = end.tv_sec - start->tv_sec;
	microseconds = end.tv_usec - start->tv_usec;
	return (seconds * 1000000 + microseconds);
}

double	timer_elapsed_ms(struct timeval *start)
{
	return (timer_elapsed_us(start) / 1000.0);
}

void	metrics_init(t_metrics *metrics)
{
	int	i;

	metrics->render_time_us = 0;
	metrics->frame_count = 0;
	metrics->fps = 0.0;
	metrics->rays_traced = 0;
	metrics->intersect_tests = 0;
	metrics->bvh_nodes_visited = 0;
	metrics->bvh_tests_skipped = 0;
	metrics->quality_mode = 0;
	i = 0;
	while (i < 60)
	{
		metrics->frame_times_us[i] = 0;
		i++;
	}
	metrics->frame_index = 0;
}

void	metrics_start_frame(t_metrics *metrics)
{
	metrics->rays_traced = 0;
	metrics->intersect_tests = 0;
	metrics->bvh_nodes_visited = 0;
	metrics->bvh_tests_skipped = 0;
	metrics->bvh_box_tests = 0;
	timer_start(&metrics->start_time);
}

static double	calculate_fps_internal(t_metrics *m)
{
	long	sum;
	int		i;
	int		valid_frames;

	sum = 0;
	i = 0;
	valid_frames = 0;
	while (i < 60)
	{
		if (m->frame_times_us[i] > 0)
		{
			sum += m->frame_times_us[i];
			valid_frames++;
		}
		i++;
	}
	if (valid_frames == 0 || sum == 0)
		return (0.0);
	return (1000000.0 / (sum / (double)valid_frames));
}

void	metrics_end_frame(t_metrics *metrics)
{
	metrics->render_time_us = timer_elapsed_us(&metrics->start_time);
	metrics->frame_times_us[metrics->frame_index] = metrics->render_time_us;
	metrics->frame_index = (metrics->frame_index + 1) % 60;
	metrics->frame_count++;
	metrics->fps = calculate_fps_internal(metrics);
}

void	metrics_add_ray(t_metrics *metrics)
{
	if (metrics)
		metrics->rays_traced++;
}

void	metrics_add_intersect_test(t_metrics *metrics)
{
	if (metrics)
		metrics->intersect_tests++;
}

void	metrics_add_bvh_node_visit(t_metrics *metrics)
{
	if (metrics)
		metrics->bvh_nodes_visited++;
}

void	metrics_add_bvh_skip(t_metrics *metrics)
{
	if (metrics)
		metrics->bvh_tests_skipped++;
}

void	metrics_add_bvh_box_test(t_metrics *metrics)
{
	if (metrics)
		metrics->bvh_box_tests++;
}

void	metrics_log_render(t_metrics *metrics)
{
	(void)metrics;
}
