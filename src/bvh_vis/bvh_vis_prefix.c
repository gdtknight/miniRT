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

int	prefix_push(t_prefix_state *state, int is_last)
{
	const char	*prefix;
	int			new_len;
	char		*new_buffer;

	if (is_last)
		prefix = "    ";
	else
		prefix = "│   ";
	new_len = state->length + 4;
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
	ft_memcpy(state->buffer + state->length, prefix, 4);
	state->length = new_len;
	state->buffer[state->length] = '\0';
	state->level++;
	return (1);
}

void	prefix_pop(t_prefix_state *state)
{
	if (state->length >= 4)
	{
		state->length -= 4;
		state->buffer[state->length] = '\0';
	}
	if (state->level > 0)
		state->level--;
}

void	prefix_print(t_prefix_state *state)
{
	if (state && state->buffer && state->length > 0)
		printf("%s", state->buffer);
}

void	prefix_destroy(t_prefix_state *state)
{
	if (state && state->buffer)
	{
		free(state->buffer);
		state->buffer = NULL;
	}
	state->capacity = 0;
	state->length = 0;
	state->level = 0;
}
