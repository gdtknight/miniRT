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
#include "window.h"

/**
 * @brief Initialize debounce state with defaults.
 *
 * Sets the debounce state machine to idle and configures timing and preview
 * behavior for interactive rendering.
 *
 * @param state Debounce state to initialize.
 */
void	debounce_init(t_debounce_state *state)
{
	state->state = DEBOUNCE_IDLE;
	state->timer.is_active = 0;
	state->timer.delay_ms = DEBOUNCE_DEFAULT_DELAY_MS;
	state->preview_enabled = DEBOUNCE_DEFAULT_PREVIEW;
	state->auto_upgrade = DEBOUNCE_DEFAULT_AUTO_UPGRADE;
	state->cancel_requested = 0;
}

/**
 * @brief Handle user input and update debounce state machine.
 *
 * Starts or resets the debounce timer on interaction, and if rendering is
 * already in progress, requests cancel before scheduling a new pass.
 *
 * @param state Debounce state to update.
 * @param render Render context for cancel flags and render state.
 */
void	debounce_on_input(t_debounce_state *state, t_render *render)
{
	if (state->state == DEBOUNCE_IDLE)
	{
		state->state = DEBOUNCE_ACTIVE;
		debounce_timer_start(&state->timer);
	}
	else if (state->state == DEBOUNCE_ACTIVE)
	{
		debounce_timer_reset(&state->timer);
	}
	else if (state->state == DEBOUNCE_PREVIEW
		|| state->state == DEBOUNCE_FINAL)
	{
		if (render_has_flag(render, RENDER_RENDERING))
			state->cancel_requested = 1;
		state->state = DEBOUNCE_ACTIVE;
		debounce_timer_start(&state->timer);
	}
}

/**
 * @brief Transition from active debounce to preview/final rendering.
 *
 * When the debounce timer expires, chooses preview or final mode, updates
 * render flags, and marks the frame as dirty.
 *
 * @param state Debounce state to update.
 * @param render Render context for flag updates.
 */
static void	debounce_handle_active(t_debounce_state *state, t_render *render)
{
	if (!debounce_timer_expired(&state->timer))
		return ;
	if (state->preview_enabled)
		state->state = DEBOUNCE_PREVIEW;
	else
		state->state = DEBOUNCE_FINAL;
	if (state->preview_enabled)
		render_set_flag(render, RENDER_LOW_QUALITY);
	else
		render_clear_flag(render, RENDER_LOW_QUALITY);
	render_set_flag(render, RENDER_DIRTY);
	debounce_timer_stop(&state->timer);
}

/**
 * @brief Handle preview completion and optional auto-upgrade.
 *
 * If preview rendering is done and auto-upgrade is enabled, switches to
 * final rendering; otherwise returns to idle.
 *
 * @param state Debounce state to update.
 * @param render Render context for render flags.
 */
static void	debounce_handle_preview(t_debounce_state *state, t_render *render)
{
	if (render_has_flag(render, RENDER_DIRTY))
		return ;
	if (state->auto_upgrade)
	{
		state->state = DEBOUNCE_FINAL;
		render_clear_flag(render, RENDER_LOW_QUALITY);
		render_set_flag(render, RENDER_DIRTY);
	}
	else
		state->state = DEBOUNCE_IDLE;
}

/**
 * @brief Advance debounce state machine based on timers and render flags.
 *
 * Drives transitions between active, preview, final, and idle states.
 *
 * @param state Debounce state to update.
 * @param render Render context for current render flags.
 */
void	debounce_update(t_debounce_state *state, t_render *render)
{
	if (state->state == DEBOUNCE_ACTIVE)
		debounce_handle_active(state, render);
	else if (state->state == DEBOUNCE_PREVIEW)
		debounce_handle_preview(state, render);
	else if (state->state == DEBOUNCE_FINAL
		&& !render_has_flag(render, RENDER_DIRTY))
		state->state = DEBOUNCE_IDLE;
}
