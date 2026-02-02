/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_quality.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:15:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/02 14:15:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render_quality.h"
#include <sys/time.h>
#include <stdlib.h>

/**
 * @brief Compute elapsed time in microseconds since a start timestamp.
 *
 * Uses gettimeofday to read the current time and returns the delta relative
 * to the provided start value.
 *
 * @param start Pointer to the start timeval.
 * @return long Elapsed microseconds since start.
 */
static long	get_elapsed_us(struct timeval *start)
{
	struct timeval	now;

	gettimeofday(&now, NULL);
	return ((now.tv_sec - start->tv_sec) * 1000000
		+ (now.tv_usec - start->tv_usec));
}

/**
 * @brief Set render quality mode and target quality.
 *
 * Forces both the current and target quality to the provided mode, disabling
 * any pending upgrade/downgrade logic.
 *
 * @param state Render state to update.
 * @param mode Target quality mode.
 */
void	quality_set_mode(t_render_state *state, t_quality_mode mode)
{
	state->quality = mode;
	state->target_quality = mode;
}

/**
 * @brief Decide whether adaptive quality should upgrade to target quality.
 *
 * Returns true only when the user is not interacting, the current quality
 * differs from the target, and the interaction cooldown has elapsed.
 *
 * @param state Render state containing interaction timing.
 * @return int 1 if an upgrade should occur, 0 otherwise.
 */
int	quality_should_upgrade(t_render_state *state)
{
	long	elapsed;

	if (state->interaction.is_interacting)
		return (0);
	if (state->quality == state->target_quality)
		return (0);
	elapsed = get_elapsed_us(&state->interaction.last_interaction);
	return (elapsed > 1000000);
}

/**
 * @brief Record an interaction and optionally drop to low quality.
 *
 * Marks the render as interacting, updates interaction counters/timestamp,
 * and if adaptive quality is enabled, switches to low quality for
 * responsiveness.
 *
 * @param state Render state to update.
 */
void	quality_handle_interaction(t_render_state *state)
{
	state->interaction.is_interacting = 1;
	state->interaction.interaction_count++;
	gettimeofday(&state->interaction.last_interaction, NULL);
	if (state->adaptive_enabled && state->target_quality == QUALITY_HIGH)
		state->quality = QUALITY_LOW;
}
