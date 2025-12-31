/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metrics.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 21:30:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef METRICS_H
# define METRICS_H

# include <sys/time.h>

/* Performance metrics for rendering */
typedef struct s_metrics
{
	struct timeval	start_time;
	struct timeval	end_time;
	long			render_time_us;
	long			frame_times_us[60];
	int				frame_index;
	long			frame_count;
	double			fps;
	long			rays_traced;
	long			intersect_tests;
	long			bvh_nodes_visited;
	long			bvh_tests_skipped;
	long			bvh_box_tests;
	int				quality_mode;
}	t_metrics;

/* Timer utilities */
void	timer_start(struct timeval *tv);
long	timer_elapsed_us(struct timeval *start);
double	timer_elapsed_ms(struct timeval *start);

/* Metrics operations */
void	metrics_init(t_metrics *metrics);
void	metrics_start_frame(t_metrics *metrics);
void	metrics_end_frame(t_metrics *metrics);
void	metrics_log_render(t_metrics *metrics);
void	metrics_add_ray(t_metrics *metrics);
void	metrics_add_intersect_test(t_metrics *metrics);
void	metrics_add_bvh_node_visit(t_metrics *metrics);
void	metrics_add_bvh_skip(t_metrics *metrics);
void	metrics_add_bvh_box_test(t_metrics *metrics);

/* Metrics calculations */
double	calculate_fps(t_metrics *m);
double	calculate_bvh_efficiency(t_metrics *m, int object_count);
double	calculate_avg_tests_per_ray(t_metrics *m);

#endif
