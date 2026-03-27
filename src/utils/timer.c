/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:31:46 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

/**
 * @brief Compute elapsed time in microseconds since start.
 *
 * @param start Start time.
 * @return long Elapsed microseconds.
 */
long	timer_elapsed_us(struct timeval *start)
{
	struct timeval	now;
	long			elapsed;

	gettimeofday(&now, 0);
	elapsed = (now.tv_sec - start->tv_sec) * 1000000L;
	elapsed += (now.tv_usec - start->tv_usec);
	return (elapsed);
}
