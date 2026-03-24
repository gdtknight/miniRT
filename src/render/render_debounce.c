/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_debounce.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 16:21:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 16:21:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render_debounce.h"
#include "render.h"

void	debounce_init(t_debounce_state *state)
{
	state->state = DEBOUNCE_IDLE;
	state->timer.is_active = 0;
	state->timer.delay_ms = DEBOUNCE_DEFAULT_DELAY_MS;
	state->last_preview_time.tv_sec = 0;
	state->last_preview_time.tv_usec = 0;
}

void	debounce_on_input(t_debounce_state *state, t_render *render)
{
	if (state->state == DEBOUNCE_IDLE)
	{
		state->state = DEBOUNCE_ACTIVE;
		debounce_timer_start(&state->timer);
	}
	else if (state->state == DEBOUNCE_ACTIVE)
		debounce_timer_reset(&state->timer);
	else if (state->state == DEBOUNCE_FINAL
		|| state->state == DEBOUNCE_COOLDOWN)
	{
		state->state = DEBOUNCE_ACTIVE;
		state->timer.delay_ms = DEBOUNCE_DEFAULT_DELAY_MS;
		debounce_timer_start(&state->timer);
	}
	if (debounce_check_preview_throttle(state))
	{
		render_set_flag(render, RENDER_LOW_QUALITY | RENDER_DIRTY);
		gettimeofday(&state->last_preview_time, NULL);
	}
}

static void	debounce_handle_active(t_debounce_state *state, t_render *render)
{
	if (!debounce_timer_expired(&state->timer))
		return ;
	state->state = DEBOUNCE_FINAL;
	render_clear_flag(render, RENDER_LOW_QUALITY);
	render_set_flag(render, RENDER_DIRTY);
	debounce_timer_stop(&state->timer);
}

static void	debounce_handle_cooldown(t_debounce_state *state)
{
	if (!debounce_timer_expired(&state->timer))
		return ;
	state->state = DEBOUNCE_IDLE;
	debounce_timer_stop(&state->timer);
}

void	debounce_update(t_debounce_state *state, t_render *render)
{
	if (state->state == DEBOUNCE_ACTIVE)
		debounce_handle_active(state, render);
	else if (state->state == DEBOUNCE_FINAL
		&& !render_has_flag(render, RENDER_DIRTY))
	{
		state->state = DEBOUNCE_COOLDOWN;
		state->timer.delay_ms = DEBOUNCE_COOLDOWN_MS;
		debounce_timer_start(&state->timer);
	}
	else if (state->state == DEBOUNCE_COOLDOWN)
		debounce_handle_cooldown(state);
}
