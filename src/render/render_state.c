/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_state.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 21:30:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render_state.h"
#include "window.h"
#include <stdlib.h>

void	render_state_init(t_render_state *state)
{
	state->quality = QUALITY_HIGH;
	state->target_quality = QUALITY_HIGH;
	state->interaction.is_interacting = 0;
	state->interaction.interaction_count = 0;
	timer_start(&state->interaction.last_interaction);
	state->progressive.enabled = 0;
	state->progressive.current_tile = 0;
	state->progressive.total_tiles = 0;
	state->progressive.tiles_completed = 0;
	state->progressive.tile_size = 32;
	metrics_init(&state->metrics);
	state->bvh = NULL;
	state->adaptive_enabled = 0;
	state->bvh_enabled = 0;
	state->show_info = 1;
}

void	render_state_cleanup(t_render_state *state)
{
	if (state->bvh)
	{
		bvh_destroy(state->bvh);
		state->bvh = NULL;
	}
}

void	render_state_update(t_render_state *state)
{
	if (state->adaptive_enabled && quality_should_upgrade(state))
	{
		state->quality = state->target_quality;
	}
}

void	quality_set_mode(t_render_state *state, t_quality_mode mode)
{
	state->quality = mode;
	state->target_quality = mode;
	state->metrics.quality_mode = (mode == QUALITY_HIGH) ? 1 : 0;
}

int	quality_should_upgrade(t_render_state *state)
{
	long	elapsed;

	if (state->interaction.is_interacting)
		return (0);
	if (state->quality == state->target_quality)
		return (0);
	elapsed = timer_elapsed_us(&state->interaction.last_interaction);
	return (elapsed > 1000000);
}

void	quality_handle_interaction(t_render_state *state)
{
	state->interaction.is_interacting = 1;
	state->interaction.interaction_count++;
	timer_start(&state->interaction.last_interaction);
	if (state->adaptive_enabled && state->target_quality == QUALITY_HIGH)
		state->quality = QUALITY_LOW;
}

void	progressive_init(t_progressive_state *prog, int width, int height,
		int tile_size)
{
	int	tiles_x;
	int	tiles_y;

	prog->tile_size = tile_size;
	tiles_x = (width + tile_size - 1) / tile_size;
	tiles_y = (height + tile_size - 1) / tile_size;
	prog->total_tiles = tiles_x * tiles_y;
	prog->current_tile = 0;
	prog->tiles_completed = 0;
	prog->enabled = 1;
}

int	progressive_next_tile(t_progressive_state *prog, int *x, int *y, int *w,
		int *h)
{
	int	tiles_x;
	int	tile_x;
	int	tile_y;

	if (!prog->enabled || prog->current_tile >= prog->total_tiles)
		return (0);
	tiles_x = (800 + prog->tile_size - 1) / prog->tile_size;
	tile_x = prog->current_tile % tiles_x;
	tile_y = prog->current_tile / tiles_x;
	*x = tile_x * prog->tile_size;
	*y = tile_y * prog->tile_size;
	*w = prog->tile_size;
	*h = prog->tile_size;
	if (*x + *w > WINDOW_WIDTH)
		*w = WINDOW_WIDTH - *x;
	if (*y + *h > WINDOW_HEIGHT)
		*h = WINDOW_HEIGHT - *y;
	prog->current_tile++;
	return (1);
}

void	progressive_reset(t_progressive_state *prog)
{
	prog->current_tile = 0;
	prog->tiles_completed = 0;
}
