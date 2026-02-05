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
	long	box_tests;
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
	int				quality_mode;
}	t_metrics;

/* Timer utilities */
void	timer_start(struct timeval *tv);
long	timer_elapsed_us(struct timeval *start);
double	timer_elapsed_ms(struct timeval *start);
long	get_time_ns(void);

/* Metrics operations */
void	metrics_init(t_metrics *metrics);
void	metrics_start_frame(t_metrics *metrics);
void	metrics_end_frame(t_metrics *metrics);
void	metrics_add_ray(t_metrics *metrics);
void	metrics_add_intersect_test(t_metrics *metrics);
void	metrics_add_shadow_intersect(t_metrics *metrics);

/* BVH metrics operations */
void	metrics_add_bvh_node_visit(t_metrics *metrics);
void	metrics_add_bvh_skip(t_metrics *metrics);
void	metrics_add_bvh_box_test(t_metrics *metrics);
void	metrics_reset_bvh(t_bvh_metrics *bvh);

/* Metrics calculations */
double	calculate_fps(t_metrics *m);
double	calculate_bvh_efficiency(t_metrics *m, int object_count);
double	calculate_avg_tests_per_ray(t_metrics *m);
void	metrics_print_summary(t_metrics *m, int object_count);

#endif
