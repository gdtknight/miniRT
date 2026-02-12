/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_events.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:35:18 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window_internal.h"
#include "hud.h"
#include "keyguide.h"

/**
 * @brief Handle key press events and trigger actions.
 *
 * Routes keyboard input to HUD controls, object selection, camera movement,
 * and transform operations while updating render flags as needed.
 *
 * @param keycode Key code of the pressed key.
 * @param param Pointer to the render context.
 * @return int Always returns 0 for MLX event handling.
 */
int	handle_key(int keycode, void *param)
{
	t_render	*render;

	render = (t_render *)param;
	if (keycode == KEY_ESC)
		close_window(param);
	handle_hud_keys(render, keycode);
	handle_camera_keys(render, keycode);
	handle_transform_keys(render, keycode);
	return (0);
}

/**
 * @brief Handle window expose events by presenting the framebuffer.
 *
 * Pushes the current image buffer to the window and draws HUD overlays
 * if they are enabled.
 *
 * @param render Render context containing image buffer and HUD state.
 * @return int Always returns 0 for MLX event handling.
 */
int	handle_expose(void *param)
{
	t_render	*render;

	render = (t_render *)param;
	mlx_put_image_to_window(render->mlx.mlx, render->mlx.win,
		render->mlx.img.img, 0, 0);
	if (render->hud.visible)
	{
		hud_render(render);
		keyguide_render(render);
	}
	return (0);
}

/**
 * @brief Handle key release events.
 *
 * Updates render flags when modifier keys are released.
 *
 * @param keycode Key code of the released key.
 * @param param Pointer to the render context.
 * @return int Always returns 0 for MLX event handling.
 */
int	handle_key_release(int keycode, void *param)
{
	(void)keycode;
	(void)param;
	return (0);
}
