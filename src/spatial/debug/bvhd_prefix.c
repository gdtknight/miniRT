/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvhd_prefix.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:10:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 15:10:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial/bvh_debug.h"
#include "libft.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Initialize the prefix state used for tree printing.
 *
 * Allocates a buffer and resets prefix metadata.
 *
 * @return t_prefix_state Initialized prefix state.
 */
t_prefix_state	bvhd_prefix_init(void)
{
	t_prefix_state	state;

	state.buffer = malloc(40);
	if (!state.buffer)
	{
		state.capacity = 0;
		state.length = 0;
		state.level = 0;
		return (state);
	}
	state.capacity = 40;
	state.length = 0;
	state.level = 0;
	state.buffer[0] = '\0';
	return (state);
}

/**
 * @brief Remove the last prefix segment.
 *
 * @param state Prefix state to update.
 */
void	bvhd_prefix_pop(t_prefix_state *state)
{
	if (state->length >= PREFIX_SEGMENT_LEN)
	{
		state->length -= PREFIX_SEGMENT_LEN;
		state->buffer[state->length] = '\0';
	}
	if (state->level > 0)
		state->level--;
}

/**
 * @brief Print the current prefix to stdout.
 *
 * @param state Prefix state to print.
 */
void	bvhd_prefix_print(t_prefix_state *state)
{
	if (state && state->buffer && state->length > 0)
		printf("%s", state->buffer);
}

/**
 * @brief Free prefix buffer and reset state.
 *
 * @param state Prefix state to destroy.
 */
void	bvhd_prefix_destroy(t_prefix_state *state)
{
	if (!state)
		return ;
	if (state->buffer)
	{
		free(state->buffer);
		state->buffer = NULL;
	}
	state->capacity = 0;
	state->length = 0;
	state->level = 0;
}
