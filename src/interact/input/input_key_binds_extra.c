/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_key_binds_extra.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/03/24 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "interact/input.h"

/**
 * @brief Register a single key binding entry.
 */
static void	bind_key(t_key_binds *binds, int keycode, t_key_handler handler)
{
	if (binds->count >= KEY_BIND_COUNT)
		return ;
	binds->entries[binds->count].keycode = keycode;
	binds->entries[binds->count].handler = handler;
	binds->count++;
}

/**
 * @brief Register resize and rotation key bindings.
 *
 * @param binds Key binding table to populate.
 */
void	init_transform_edit_binds(t_key_binds *binds)
{
	bind_key(binds, KEY_Y, handle_transform_keys);
	bind_key(binds, KEY_U, handle_transform_keys);
	bind_key(binds, KEY_N, handle_transform_keys);
	bind_key(binds, KEY_M, handle_transform_keys);
	bind_key(binds, KEY_I, handle_transform_keys);
	bind_key(binds, KEY_J, handle_transform_keys);
	bind_key(binds, KEY_O, handle_transform_keys);
	bind_key(binds, KEY_K, handle_transform_keys);
	bind_key(binds, KEY_P, handle_transform_keys);
	bind_key(binds, KEY_L, handle_transform_keys);
}
