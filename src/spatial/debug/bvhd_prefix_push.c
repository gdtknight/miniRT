/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvhd_prefix_push.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/03/24 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial/bvh_debug.h"
#include "libft.h"
#include <stdlib.h>
#include <limits.h>

/**
 * @brief Grow prefix buffer capacity when needed.
 *
 * @param state Prefix state to expand.
 * @return int 1 on success, 0 on allocation failure.
 */
static int	prefix_grow(t_prefix_state *state)
{
	char	*new_buffer;

	if (state->capacity > INT_MAX / 2)
		return (0);
	new_buffer = malloc(state->capacity * 2);
	if (!new_buffer)
		return (0);
	ft_memcpy(new_buffer, state->buffer, state->length);
	free(state->buffer);
	state->buffer = new_buffer;
	state->capacity *= 2;
	return (1);
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
int	bvhd_prefix_push(t_prefix_state *state, int is_last)
{
	const char	*prefix;
	int			new_len;

	if (is_last)
		prefix = "      ";
	else
		prefix = "│   ";
	new_len = state->length + PREFIX_SEGMENT_LEN;
	if (new_len >= state->capacity && !prefix_grow(state))
		return (0);
	ft_memcpy(state->buffer + state->length, prefix, PREFIX_SEGMENT_LEN);
	state->length = new_len;
	state->buffer[state->length] = '\0';
	state->level++;
	return (1);
}
