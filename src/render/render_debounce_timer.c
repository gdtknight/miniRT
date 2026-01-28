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

void	debounce_cancel(t_debounce_state *state)
{
	state->cancel_requested = 0;
}

/*
** debounce_timer_start - Start the debounce timer
** Sets the timer to active and records the current time
*/
void	debounce_timer_start(t_debounce_timer *timer)
{
	gettimeofday(&timer->last_input_time, NULL);
	timer->is_active = 1;
}

/*
** debounce_timer_reset - Reset the timer to current time
** Updates the last_input_time without changing active state
*/
void	debounce_timer_reset(t_debounce_timer *timer)
{
	gettimeofday(&timer->last_input_time, NULL);
}

/*
** debounce_timer_stop - Stop the debounce timer
** Sets the timer to inactive
*/
void	debounce_timer_stop(t_debounce_timer *timer)
{
	timer->is_active = 0;
}

/*
** debounce_timer_expired - Check if the timer delay has elapsed
** Returns 1 if delay_ms has passed since last_input_time, 0 otherwise
*/
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
