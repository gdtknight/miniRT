/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_quality.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:15:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render_quality.h"
#include <sys/time.h>

static long	get_elapsed_us(struct timeval *start)
{
	struct timeval	now;

	gettimeofday(&now, NULL);
	return ((now.tv_sec - start->tv_sec) * 1000000
		+ (now.tv_usec - start->tv_usec));
}

void	quality_set_mode(t_render_state *state, t_quality_mode mode)
{
	state->quality = mode;
	state->target_quality = mode;
}

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

void	quality_handle_interaction(t_render_state *state)
{
	state->interaction.is_interacting = 1;
	state->interaction.interaction_count++;
	gettimeofday(&state->interaction.last_interaction, NULL);
	if (state->adaptive_enabled && state->target_quality == QUALITY_HIGH)
		state->quality = QUALITY_LOW;
}
