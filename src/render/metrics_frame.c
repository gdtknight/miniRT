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

/**
 * @brief Initialize frame timing history and counters.
 *
 * Clears timing statistics and zeroes the frame time history buffer.
 *
 * @param timing Frame timing structure to initialize.
 */
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

/**
 * @brief Initialize all render metrics counters and timing.
 *
 * Resets timing history, ray and BVH counters, and quality mode.
 *
 * @param metrics Metrics structure to initialize.
 */
void	metrics_init(t_metrics *metrics)
{
	init_frame_timing(&metrics->timing);
	metrics->ray.rays_traced = 0;
	metrics->ray.intersect_tests = 0;
	metrics->ray.shadow_intersect_tests = 0;
	metrics->bvh.nodes_visited = 0;
	metrics->bvh.tests_skipped = 0;
	metrics->bvh.box_tests = 0;
	metrics->quality_mode = 0;
}

/**
 * @brief Begin a new frame timing and reset per-frame counters.
 *
 * Resets ray/BVH counters and starts the frame timer.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_start_frame(t_metrics *metrics)
{
	metrics->ray.rays_traced = 0;
	metrics->ray.intersect_tests = 0;
	metrics->ray.shadow_intersect_tests = 0;
	metrics->bvh.nodes_visited = 0;
	metrics->bvh.tests_skipped = 0;
	metrics->bvh.box_tests = 0;
	timer_start(&metrics->timing.start_time);
}

/**
 * @brief Calculate FPS from recent frame time history.
 *
 * Computes the mean of valid frame times and converts to frames per second.
 *
 * @param t Frame timing history.
 * @return double Estimated FPS over the history window.
 */
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

/**
 * @brief Finalize frame timing and update FPS.
 *
 * Records the elapsed frame time, advances the history index, and updates
 * the FPS estimate.
 *
 * @param metrics Metrics structure to update.
 */
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
