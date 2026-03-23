/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_dispatch.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:35:35 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window_internal.h"
#include "hud.h"
#include "hud_text.h"

/**
 * @brief Handle camera-related keys and trigger re-rendering.
 *
 * Dispatches movement, pitch, and reset operations and marks the render
 * state dirty via the debounce system.
 *
 * @param render Render context containing scene and debounce state.
 * @param keycode Key code to interpret.
 */
void	handle_camera_keys(t_render *render, int keycode)
{
	if (keycode == KEY_W || keycode == KEY_X || keycode == KEY_A
		|| keycode == KEY_D || keycode == KEY_Q || keycode == KEY_Z)
	{
		handle_camera_move(render, keycode);
		debounce_on_input(&render->debounce, render);
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_E || keycode == KEY_C)
	{
		handle_camera_pitch(render, keycode);
		debounce_on_input(&render->debounce, render);
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_S)
	{
		handle_camera_reset(render);
		debounce_on_input(&render->debounce, render);
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_1 || keycode == KEY_3)
	{
		handle_camera_yaw(render, keycode);
		debounce_on_input(&render->debounce, render);
		hud_mark_dirty(render);
	}
}

/**
 * @brief Handle resize/rotate key groups for objects.
 *
 * Applies size or rotation updates to the selected object and marks the HUD
 * as dirty.
 *
 * @param render Render context containing selection state.
 * @param keycode Key code to interpret.
 */
static void	handle_resize_rotate_keys(t_render *render, int keycode)
{
	if (keycode == KEY_Y || keycode == KEY_U
		|| keycode == KEY_N || keycode == KEY_M)
	{
		handle_object_resize(render, keycode);
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_I || keycode == KEY_J
		|| keycode == KEY_O || keycode == KEY_K
		|| keycode == KEY_P || keycode == KEY_L)
	{
		handle_object_rotate(render, keycode);
		hud_mark_dirty(render);
	}
}

/**
 * @brief Handle object and light transformation keys.
 *
 * Moves objects, moves lights, or delegates to resize/rotate handlers
 * while triggering debounce and HUD refresh.
 *
 * @param render Render context containing scene and selection state.
 * @param keycode Key code to interpret.
 */
void	handle_transform_keys(t_render *render, int keycode)
{
	if (keycode == KEY_R || keycode == KEY_T || keycode == KEY_F
		|| keycode == KEY_G || keycode == KEY_V || keycode == KEY_B)
	{
		handle_object_move(render, keycode);
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_BRACKET_LEFT || keycode == KEY_BRACKET_RIGHT
		|| keycode == KEY_SEMICOLON || keycode == KEY_QUOTE
		|| keycode == KEY_COMMA || keycode == KEY_PERIOD)
	{
		handle_light_move(render, keycode);
		hud_mark_dirty(render);
	}
	else
		handle_resize_rotate_keys(render, keycode);
}

/**
 * @brief Handle HUD interaction keys.
 *
 * Toggles HUD visibility, changes selection, and switches pages based on
 * navigation keys.
 *
 * @param render Render context containing HUD state.
 * @param keycode Key code to interpret.
 */
void	handle_hud_keys(t_render *render, int keycode)
{
	if (keycode == KEY_H)
		hud_toggle(render);
	else if (keycode == KEY_TAB)
		hud_select_next(render);
	else if (keycode == KEY_UP)
		hud_change_page(render, -1);
	else if (keycode == KEY_DOWN)
		hud_change_page(render, 1);
	else if (keycode == KEY_EQUAL && render->scene->light_count > 0)
	{
		render->scene->selected_light = (render->scene->selected_light + 1)
			% render->scene->light_count;
		hud_mark_dirty(render);
	}
}
