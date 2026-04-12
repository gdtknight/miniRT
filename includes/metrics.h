/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metrics.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef METRICS_H
# define METRICS_H

# include <sys/time.h>

# define FRAME_HISTORY_SIZE 60

/* BVH-specific performance metrics */
typedef struct s_bvh_metrics
{
	long	nodes_visited;
	long	tests_skipped;
}	t_bvh_metrics;

/* Ray tracing metrics */
typedef struct s_ray_metrics
{
	long	rays_traced;
	long	intersect_tests;
	long	shadow_intersect_tests;
}	t_ray_metrics;

/* Frame timing data */
typedef struct s_frame_timing
{
	struct timeval	start_time;
	struct timeval	end_time;
	long			render_time_us;
	long			frame_times_us[FRAME_HISTORY_SIZE];
	int				frame_index;
	long			frame_count;
	double			fps;
}	t_frame_timing;

/* Complete performance metrics */
typedef struct s_metrics
{
	t_frame_timing	timing;
	t_ray_metrics	ray;
	t_bvh_metrics	bvh;
}	t_metrics;

/* Metrics operations */

/**
 * @brief Initialize all render metrics counters and timing.
 *
 * Resets timing history, ray and BVH counters.
 *
 * @param metrics Metrics structure to initialize.
 */
void	metrics_init(t_metrics *metrics);

/**
 * @brief Begin a new frame timing and reset per-frame counters.
 *
 * Resets ray/BVH counters and starts the frame timer.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_start_frame(t_metrics *metrics);

/**
 * @brief Finalize frame timing and update FPS.
 *
 * Records the elapsed frame time, advances the history index, and
 * updates the FPS estimate.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_end_frame(t_metrics *metrics);

/**
 * @brief Increment the traced ray counter.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_add_ray(t_metrics *metrics);

/**
 * @brief Increment the intersection test counter.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_add_intersect_test(t_metrics *metrics);

/**
 * @brief Increment the shadow intersection test counter.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_add_shadow_intersect(t_metrics *metrics);

/* BVH metrics operations */

/**
 * @brief Increment the BVH node visit counter.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_add_bvh_node_visit(t_metrics *metrics);

/**
 * @brief Increment the BVH skip counter.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_add_bvh_skip(t_metrics *metrics);

/**
 * @brief Print a summary of all render metrics to stdout.
 *
 * Outputs frame timing, ray counts, intersection stats, and BVH
 * counters for benchmarking and profiling purposes.
 *
 * @param m Metrics structure containing all counter data.
 * @param object_count Number of objects in the scene.
 */
void	metrics_print_summary(t_metrics *m, int object_count);

#endif
