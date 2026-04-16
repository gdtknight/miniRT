/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event_dispatch.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:35:18 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render/render.h"
#include "interact/input.h"
#include "interact/hud.h"
#include "interact/keyguide.h"
#include "lighting/texture.h"
#include <stdlib.h>

/**
 * @brief Handle window close event and shut down cleanly.
 *
 * Frees render and scene resources, then exits the process.
 *
 * @param param Pointer to the render context.
 * @return int Always returns 0 for MLX event handling.
 */
int	close_window(void *param)
{
	t_render	*render;
	t_scene		*scene;

	render = (t_render *)param;
	scene = render->scene;
	cleanup_all_bump_maps(scene, render->mlx.mlx);
	render_destroy(render);
	scene_destroy(scene);
	exit(0);
}

/**
 * @brief Handle key press events via dispatch table lookup.
 *
 * Looks up the keycode in the key bind table and calls the matched handler.
 * Unbound keys are ignored with no function calls.
 *
 * @param keycode Key code of the pressed key.
 * @param param Pointer to the render context.
 * @return int Always returns 0 for MLX event handling.
 */
int	handle_key(int keycode, void *param)
{
	t_render	*render;
	int			i;

	render = (t_render *)param;
	if (keycode == KEY_ESC)
		close_window(param);
	i = 0;
	while (i < render->key_binds.count)
	{
		if (render->key_binds.entries[i].keycode == keycode)
			return (render->key_binds.entries[i].handler(render, keycode), 0);
		i++;
	}
	return (0);
}

/**
 * @brief Handle window expose events by presenting the framebuffer.
 *
 * Pushes the current image buffer to the window and draws HUD overlays
 * if they are enabled.
 *
 * @param param Pointer to the render context (cast internally).
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
