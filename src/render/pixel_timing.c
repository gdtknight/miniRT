/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pixel_timing.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 01:28:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:33:20 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pixel_timing.h"
#include <stdlib.h>
#include <limits.h>

/**
 * @brief Initialize pixel timing statistics storage.
 *
 * Allocates a sample buffer, resets counters, and enables timing collection.
 *
 * @param timing Pixel timing structure to initialize.
 */
void	pixel_timing_init(t_pixel_timing *timing)
{
	timing->capacity = MAX_PIXEL_SAMPLES;
	timing->samples = malloc(sizeof(long) * timing->capacity);
	timing->count = 0;
	timing->min_time = LONG_MAX;
	timing->max_time = 0;
	timing->total_time = 0;
	timing->enabled = (timing->samples != NULL);
}

/**
 * @brief Release pixel timing resources and disable collection.
 *
 * Frees the sample buffer and resets counters to a safe state.
 *
 * @param timing Pixel timing structure to clean up.
 */
void	pixel_timing_cleanup(t_pixel_timing *timing)
{
	if (timing->samples)
	{
		free(timing->samples);
		timing->samples = NULL;
	}
	timing->count = 0;
	timing->enabled = 0;
}

/**
 * @brief Record a per-pixel timing sample.
 *
 * Appends the sample if capacity allows and updates min/max/total stats.
 * No-op when timing is disabled or buffer is missing.
 *
 * @param timing Pixel timing structure to update.
 * @param time_ns Elapsed time in nanoseconds for a pixel.
 */
void	pixel_timing_add_sample(t_pixel_timing *timing, long time_ns)
{
	if (!timing->enabled || !timing->samples)
		return ;
	if (timing->count < timing->capacity)
	{
		timing->samples[timing->count] = time_ns;
		timing->count++;
		timing->total_time += time_ns;
	}
	if (time_ns < timing->min_time)
		timing->min_time = time_ns;
	if (time_ns > timing->max_time)
		timing->max_time = time_ns;
}

/**
 * @brief Comparator for long values used with qsort.
 *
 * @param a Pointer to first element.
 * @param b Pointer to second element.
 * @return int Negative if a<b, positive if a>b, zero if equal.
 */
static int	compare_long(const void *a, const void *b)
{
	long	la;
	long	lb;

	la = *(const long *)a;
	lb = *(const long *)b;
	if (la < lb)
		return (-1);
	if (la > lb)
		return (1);
	return (0);
}

/**
 * @brief Prepare timing samples for statistical reporting.
 *
 * Sorts the samples in ascending order if available, enabling percentile
 * calculations elsewhere.
 *
 * @param timing Pixel timing structure to process.
 */
void	pixel_timing_calculate_stats(t_pixel_timing *timing)
{
	if (timing->count > 0 && timing->samples)
		qsort(timing->samples, timing->count, sizeof(long), compare_long);
}
