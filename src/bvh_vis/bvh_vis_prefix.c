/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_vis_prefix.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:10:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 15:10:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh_vis.h"
#include "utils.h"
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
t_prefix_state	prefix_init(void)
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
 * @brief Append a new prefix segment for a child node.
 *
 * Expands the buffer if needed and writes either a branch or space segment.
 *
 * @param state Prefix state to update.
 * @param is_last Non-zero if the current node is the last sibling.
 * @return int 1 on success, 0 on allocation failure.
 */
int	prefix_push(t_prefix_state *state, int is_last)
{
	const char	*prefix;
	int			new_len;
	char		*new_buffer;

	if (is_last)
		prefix = "      ";
	else
		prefix = "│   ";
	new_len = state->length + 6;
	if (new_len >= state->capacity)
	{
		new_buffer = malloc(state->capacity * 2);
		if (!new_buffer)
			return (0);
		ft_memcpy(new_buffer, state->buffer, state->length);
		free(state->buffer);
		state->buffer = new_buffer;
		state->capacity *= 2;
	}
	ft_memcpy(state->buffer + state->length, prefix, 6);
	state->length = new_len;
	state->buffer[state->length] = '\0';
	state->level++;
	return (1);
}

/**
 * @brief Remove the last prefix segment.
 *
 * @param state Prefix state to update.
 */
void	prefix_pop(t_prefix_state *state)
{
	if (state->length >= 6)
	{
		state->length -= 6;
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
void	prefix_print(t_prefix_state *state)
{
	if (state && state->buffer && state->length > 0)
		printf("%s", state->buffer);
}

/**
 * @brief Free prefix buffer and reset state.
 *
 * @param state Prefix state to destroy.
 */
void	prefix_destroy(t_prefix_state *state)
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
