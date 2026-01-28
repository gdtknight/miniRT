/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_key_handlers.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 14:14:49 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window_internal.h"
#include "hud.h"

/*
** Handle camera-related keys and mark dirty for low quality render.
*/
/**
 * @brief handle camera keys 함수
 *
 * @param render 파라미터
 * @param keycode 파라미터
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
}

/*
** Handle object and light movement keys.
*/
/**
 * @brief handle transform keys 함수
 *
 * @param render 파라미터
 * @param keycode 파라미터
 */
static void	handle_resize_rotate_keys(t_render *render, int keycode)
{
	if (keycode == KEY_J || keycode == KEY_K
		|| keycode == KEY_N || keycode == KEY_M)
	{
		handle_object_resize(render, keycode);
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_U || keycode == KEY_O
		|| keycode == KEY_Y || keycode == KEY_P
		|| keycode == KEY_LEFT || keycode == KEY_RIGHT)
	{
		handle_object_rotate(render, keycode);
		hud_mark_dirty(render);
	}
}

void	handle_transform_keys(t_render *render, int keycode)
{
	if (keycode == KEY_R || keycode == KEY_T || keycode == KEY_F
		|| keycode == KEY_G || keycode == KEY_V || keycode == KEY_B)
	{
		handle_object_move(render, keycode);
		debounce_on_input(&render->debounce, render);
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_INSERT || keycode == KEY_HOME
		|| keycode == KEY_PGUP || keycode == KEY_DELETE
		|| keycode == KEY_END || keycode == KEY_PGDN)
	{
		handle_light_move(render, keycode);
		debounce_on_input(&render->debounce, render);
		hud_mark_dirty(render);
	}
	else
		handle_resize_rotate_keys(render, keycode);
}

/*
** Handle HUD-related keys.
*/
/**
 * @brief handle hud keys 함수
 *
 * @param render 파라미터
 * @param keycode 파라미터
 */
void	handle_hud_keys(t_render *render, int keycode)
{
	if (keycode == KEY_H)
		hud_toggle(render);
	else if (keycode == KEY_TAB)
	{
		if (render_has_flag(render, RENDER_SHIFT_HELD))
			hud_select_prev(render);
		else
			hud_select_next(render);
	}
	else if (keycode == KEY_UP)
		hud_change_page(render, -1);
	else if (keycode == KEY_DOWN)
		hud_change_page(render, 1);
}
