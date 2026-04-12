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

#include "render/render_debounce.h"
#include "render/render.h"
#include <sys/time.h>
#include <sys/time.h>

/**
 * @brief Initialize debounce state to idle.
 *
 * @param state Debounce state to initialize.
 */
void	debounce_init(t_debounce_state *state)
{
	state->state = DEBOUNCE_IDLE;
	state->timer.is_active = 0;
	state->timer.delay_ms = DEBOUNCE_DEFAULT_DELAY_MS;
	state->last_preview_time.tv_sec = 0;
	state->last_preview_time.tv_usec = 0;
}

/**
 * @brief Handle user input by advancing or resetting the FSM.
 *
 * Transitions IDLE to ACTIVE, refreshes the timer on repeated
 * input, and triggers a low-quality preview render when the
 * throttle interval has elapsed.
 *
 * @param state Current debounce state.
 * @param render Render context for setting quality flags.
 */
void	debounce_on_input(t_debounce_state *state, t_render *render)
{
	if (state->state == DEBOUNCE_IDLE)
	{
		state->state = DEBOUNCE_ACTIVE;
		gettimeofday(&state->timer.last_input_time, NULL);
		state->timer.is_active = 1;
	}
	else if (state->state == DEBOUNCE_ACTIVE)
		gettimeofday(&state->timer.last_input_time, NULL);
	else if (state->state == DEBOUNCE_FINAL
		|| state->state == DEBOUNCE_COOLDOWN)
	{
		state->state = DEBOUNCE_ACTIVE;
		state->timer.delay_ms = DEBOUNCE_DEFAULT_DELAY_MS;
		gettimeofday(&state->timer.last_input_time, NULL);
		state->timer.is_active = 1;
	}
	if (debounce_check_preview_throttle(state))
	{
		render_set_flag(render, RENDER_LOW_QUALITY | RENDER_DIRTY);
		gettimeofday(&state->last_preview_time, NULL);
	}
}

/**
 * @brief Transition ACTIVE to FINAL when the timer expires.
 *
 * @param state Debounce state to evaluate and update.
 * @param render Render context for setting quality flags.
 */
static void	debounce_handle_active(t_debounce_state *state, t_render *render)
{
	if (!debounce_timer_expired(&state->timer))
		return ;
	state->state = DEBOUNCE_FINAL;
	render_clear_flag(render, RENDER_LOW_QUALITY);
	render_set_flag(render, RENDER_DIRTY);
	state->timer.is_active = 0;
}

/**
 * @brief Transition COOLDOWN to IDLE when the timer expires.
 *
 * @param state Debounce state to evaluate and update.
 */
static void	debounce_handle_cooldown(t_debounce_state *state)
{
	if (!debounce_timer_expired(&state->timer))
		return ;
	state->state = DEBOUNCE_IDLE;
	state->timer.is_active = 0;
}

/**
 * @brief Tick the debounce FSM each frame.
 *
 * Drives state transitions: ACTIVE to FINAL (full-quality
 * render), FINAL to COOLDOWN (after render completes), and
 * COOLDOWN to IDLE.
 *
 * @param state Current debounce state.
 * @param render Render context for checking and setting flags.
 */
void	debounce_update(t_debounce_state *state, t_render *render)
{
	if (state->state == DEBOUNCE_ACTIVE)
		debounce_handle_active(state, render);
	else if (state->state == DEBOUNCE_FINAL
		&& !render_has_flag(render, RENDER_DIRTY))
	{
		state->state = DEBOUNCE_COOLDOWN;
		state->timer.delay_ms = DEBOUNCE_COOLDOWN_MS;
		gettimeofday(&state->timer.last_input_time, NULL);
		state->timer.is_active = 1;
	}
	else if (state->state == DEBOUNCE_COOLDOWN)
		debounce_handle_cooldown(state);
}
