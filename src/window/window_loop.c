/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_loop.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:35:49 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window_internal.h"
#include "hud.h"
#include "keyguide.h"
#include "metrics.h"
#include "spatial.h"

/**
 * @brief Rebuild the BVH if the dirty flag is set.
 *
 * Reconstructs the BVH and clears the corresponding render flag.
 *
 * @param render Render context containing the scene and flags.
 */
static void	rebuild_bvh_if_dirty(t_render *render)
{
	if (render_has_flag(render, RENDER_BVH_DIRTY))
	{
		scene_build_bvh(render->scene);
		render_clear_flag(render, RENDER_BVH_DIRTY);
	}
}

/**
 * @brief Execute a full render pass and present the framebuffer.
 *
 * Runs the renderer, updates metrics, handles debounce cancellation, and
 * swaps the image buffer to the window on success.
 *
 * @param render Render context containing scene and buffers.
 * @return int 1 if the render completed, 0 if canceled.
 */
static int	execute_render_pass(t_render *render)
{
	render_set_flag(render, RENDER_RENDERING);
	metrics_start_frame(&render->scene->metrics);
	render_scene_to_buffer(render->scene, render);
	render_clear_flag(render, RENDER_RENDERING);
	if (render->debounce.cancel_requested)
	{
		debounce_cancel(&render->debounce);
		render_set_flag(render, RENDER_DIRTY);
		return (0);
	}
	metrics_end_frame(&render->scene->metrics);
	metrics_print_summary(&render->scene->metrics,
		render->scene->objects.count);
	mlx_put_image_to_window(render->mlx.mlx, render->mlx.win,
		render->mlx.img.img, 0, 0);
	render_clear_flag(render, RENDER_DIRTY);
	return (1);
}

/**
 * @brief MLX loop hook for continuous rendering updates.
 *
 * Updates debounce state, rebuilds BVH if needed, renders when dirty, and
 * draws HUD overlays when requested.
 *
 * @param param Pointer to the render context.
 * @return int Always returns 0 for MLX loop hook.
 */
int	render_loop(void *param)
{
	t_render	*render;
	int			rendered;

	render = (t_render *)param;
	rendered = 0;
	debounce_update(&render->debounce, render);
	rebuild_bvh_if_dirty(render);
	if (render_has_flag(render, RENDER_DIRTY))
		rendered = execute_render_pass(render);
	if (render->hud.visible && (render->hud.dirty || rendered))
	{
		hud_render(render);
		keyguide_render(render);
	}
	return (0);
}
