/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_debounce_timer.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 16:21:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 16:21:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render_debounce.h"
#include <stdlib.h>

/**
 * @brief Start the debounce timer.
 *
 * Marks the timer active and records the current timestamp.
 *
 * @param timer Debounce timer to start.
 */
void	debounce_timer_start(t_debounce_timer *timer)
{
	gettimeofday(&timer->last_input_time, NULL);
	timer->is_active = 1;
}

/**
 * @brief Reset the debounce timer timestamp.
 *
 * Updates last_input_time to the current time without changing active state.
 *
 * @param timer Debounce timer to reset.
 */
void	debounce_timer_reset(t_debounce_timer *timer)
{
	gettimeofday(&timer->last_input_time, NULL);
}

/**
 * @brief Stop the debounce timer.
 *
 * Marks the timer inactive so expiration checks fail until restarted.
 *
 * @param timer Debounce timer to stop.
 */
void	debounce_timer_stop(t_debounce_timer *timer)
{
	timer->is_active = 0;
}

/**
 * @brief Check whether the debounce delay has elapsed.
 *
 * Computes elapsed time since last_input_time and compares it to delay_ms.
 *
 * @param timer Debounce timer to query.
 * @return int 1 if expired, 0 otherwise.
 */
int	debounce_check_preview_throttle(t_debounce_state *state)
{
	struct timeval	now;
	long			elapsed_ms;

	gettimeofday(&now, NULL);
	elapsed_ms = (now.tv_sec - state->last_preview_time.tv_sec) * 1000;
	elapsed_ms += (now.tv_usec - state->last_preview_time.tv_usec) / 1000;
	return (elapsed_ms >= DEBOUNCE_PREVIEW_MIN_INTERVAL_MS);
}

int	debounce_timer_expired(t_debounce_timer *timer)
{
	struct timeval	now;
	long			elapsed_ms;

	if (!timer->is_active)
		return (0);
	gettimeofday(&now, NULL);
	elapsed_ms = (now.tv_sec - timer->last_input_time.tv_sec) * 1000;
	elapsed_ms += (now.tv_usec - timer->last_input_time.tv_usec) / 1000;
	return (elapsed_ms >= timer->delay_ms);
}
