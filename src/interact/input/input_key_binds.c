/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_key_binds.c                                  :+:      :+:    :+:   */
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
 *
 * @param binds Key bindings table to append to.
 * @param keycode Key code to bind.
 * @param handler Handler function for the key.
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
 * @brief Register HUD toggle and navigation key bindings.
 *
 * @param binds Key binding table to populate.
 */
void	init_hud_binds(t_key_binds *binds)
{
	bind_key(binds, KEY_H, handle_hud_keys);
	bind_key(binds, KEY_TAB, handle_hud_keys);
	bind_key(binds, KEY_UP, handle_hud_keys);
	bind_key(binds, KEY_DOWN, handle_hud_keys);
	bind_key(binds, KEY_EQUAL, handle_hud_keys);
}

/**
 * @brief Register camera movement and rotation key bindings.
 *
 * @param binds Key binding table to populate.
 */
void	init_camera_binds(t_key_binds *binds)
{
	bind_key(binds, KEY_W, handle_camera_keys);
	bind_key(binds, KEY_X, handle_camera_keys);
	bind_key(binds, KEY_A, handle_camera_keys);
	bind_key(binds, KEY_D, handle_camera_keys);
	bind_key(binds, KEY_Q, handle_camera_keys);
	bind_key(binds, KEY_Z, handle_camera_keys);
	bind_key(binds, KEY_E, handle_camera_keys);
	bind_key(binds, KEY_C, handle_camera_keys);
	bind_key(binds, KEY_S, handle_camera_keys);
	bind_key(binds, KEY_1, handle_camera_keys);
	bind_key(binds, KEY_3, handle_camera_keys);
}

/**
 * @brief Register object and light movement key bindings.
 *
 * @param binds Key binding table to populate.
 */
void	init_transform_move_binds(t_key_binds *binds)
{
	bind_key(binds, KEY_R, handle_transform_keys);
	bind_key(binds, KEY_T, handle_transform_keys);
	bind_key(binds, KEY_F, handle_transform_keys);
	bind_key(binds, KEY_G, handle_transform_keys);
	bind_key(binds, KEY_V, handle_transform_keys);
	bind_key(binds, KEY_B, handle_transform_keys);
	bind_key(binds, KEY_BRACKET_LEFT, handle_transform_keys);
	bind_key(binds, KEY_BRACKET_RIGHT, handle_transform_keys);
	bind_key(binds, KEY_SEMICOLON, handle_transform_keys);
	bind_key(binds, KEY_QUOTE, handle_transform_keys);
	bind_key(binds, KEY_COMMA, handle_transform_keys);
	bind_key(binds, KEY_PERIOD, handle_transform_keys);
}
