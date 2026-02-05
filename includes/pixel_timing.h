/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pixel_timing.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 01:28:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 01:28:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIXEL_TIMING_H
# define PIXEL_TIMING_H

# include <sys/time.h>
# include <stddef.h>

# define MAX_PIXEL_SAMPLES 480000

typedef struct s_pixel_timing
{
	long	*samples;
	size_t	count;
	size_t	capacity;
	long	min_time;
	long	max_time;
	long	total_time;
	int		enabled;
}	t_pixel_timing;

void	pixel_timing_init(t_pixel_timing *timing);
void	pixel_timing_cleanup(t_pixel_timing *timing);
void	pixel_timing_reset(t_pixel_timing *timing);
void	pixel_timing_add_sample(t_pixel_timing *timing, long time_ns);
void	pixel_timing_calculate_stats(t_pixel_timing *timing);
void	pixel_timing_print_stats(t_pixel_timing *timing);
long	get_time_ns(void);

#endif
