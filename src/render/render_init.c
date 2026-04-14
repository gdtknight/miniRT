/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render/render.h"
#include "interact/event.h"
#include "interact/input.h"
#include "interact/hud.h"
#include "interact/keyguide.h"
#include <stdlib.h>

/**
 * @brief Initialize render state fields and subsystems.
 *
 * Binds the scene, resets selection, sets initial render flags, and
 * initializes timing/debounce subsystems.
 *
 * @param render Render context to initialize.
 * @param scene Scene associated with the render context.
 */
static void	init_render_state(t_render *render, t_scene *scene)
{
	render->scene = scene;
	render->selection.type = OBJ_NONE;
	render->selection.index = -1;
	render->state_flags = RENDER_DIRTY | RENDER_ENABLE_METRICS_PRINT;
	debounce_init(&render->debounce);
	render->key_binds.count = 0;
	init_hud_binds(&render->key_binds);
	init_camera_binds(&render->key_binds);
	init_transform_move_binds(&render->key_binds);
	init_transform_edit_binds(&render->key_binds);
}

/**
 * @brief Initialize HUD and key guide UI components.
 *
 * Initializes HUD and key guide state and precomputes pagination based
 * on the scene.
 *
 * @param render Render context containing UI state.
 * @param scene Scene used to compute pagination info.
 */
static void	init_ui_components(t_render *render, t_scene *scene)
{
	hud_init(&render->hud);
	keyguide_init(&render->keyguide);
	render->hud.total_pages = hud_calculate_total_pages(scene);
}

/**
 * @brief Register MLX event hooks for input and rendering.
 *
 * Sets up key press/release, expose, close, and loop callbacks.
 *
 * @param render Render context passed to callbacks.
 */
static void	register_hooks(t_render *render)
{
	mlx_hook(render->mlx.win, 17, 0, close_window, render);
	mlx_hook(render->mlx.win, 2, 1L << 0, handle_key, render);
	mlx_hook(render->mlx.win, 12, 1L << 15, handle_expose, render);
	mlx_loop_hook(render->mlx.mlx, render_loop, render);
}

/**
 * @brief Create and initialize a render context.
 *
 * Allocates the render context, sets up MLX window and buffers, initializes
 * render state and UI components, and registers event hooks.
 *
 * @param scene Scene to render.
 * @return t_render* Newly created render context or NULL on failure.
 */
t_render	*render_create(t_scene *scene)
{
	t_render	*render;

	render = malloc(sizeof(t_render));
	if (!render)
		return (NULL);
	if (!mlx_context_init(&render->mlx, WINDOW_WIDTH, WINDOW_HEIGHT, "miniRT"))
	{
		free(render);
		return (NULL);
	}
	init_render_state(render, scene);
	init_ui_components(render, scene);
	register_hooks(render);
	return (render);
}

/**
 * @brief Destroy a render context and release all resources.
 *
 * Frees UI components, timing resources, MLX context, and the render struct.
 *
 * @param render Render context to destroy.
 */
void	render_destroy(t_render *render)
{
	if (!render)
		return ;
	mlx_context_destroy(&render->mlx);
	free(render);
}
