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
#include "utils.h"

int	debounce_check_preview_throttle(t_debounce_state *state)
{
	long	elapsed_ms;

	elapsed_ms = timer_elapsed_us(&state->last_preview_time) / 1000;
	return (elapsed_ms >= DEBOUNCE_PREVIEW_MIN_INTERVAL_MS);
}

int	debounce_timer_expired(t_debounce_timer *timer)
{
	long	elapsed_ms;

	if (!timer->is_active)
		return (0);
	elapsed_ms = timer_elapsed_us(&timer->last_input_time) / 1000;
	return (elapsed_ms >= timer->delay_ms);
}
