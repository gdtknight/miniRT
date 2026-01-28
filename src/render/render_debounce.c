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

void	debounce_init(t_debounce_state *state)
{
	state->state = DEBOUNCE_IDLE;
	state->timer.is_active = 0;
	state->timer.delay_ms = DEBOUNCE_DEFAULT_DELAY_MS;
	state->preview_enabled = DEBOUNCE_DEFAULT_PREVIEW;
	state->auto_upgrade = DEBOUNCE_DEFAULT_AUTO_UPGRADE;
	state->cancel_requested = 0;
}

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
