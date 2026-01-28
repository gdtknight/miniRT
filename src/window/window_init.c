/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window_internal.h"
#include "hud.h"
#include "keyguide.h"
#include "pixel_timing.h"
#include <stdlib.h>

static void	init_render_state(t_render *render, t_scene *scene)
{
	render->scene = scene;
	render->selection.type = OBJ_NONE;
	render->selection.index = 0;
	render->state_flags = RENDER_DIRTY;
	pixel_timing_init(&render->pixel_timing);
	debounce_init(&render->debounce);
}

static int	init_ui_components(t_render *render, t_scene *scene)
{
	if (hud_init(&render->hud, render->mlx.mlx, render->mlx.win) == -1)
	{
		free(render);
		return (-1);
	}
	if (keyguide_init(&render->keyguide, render->mlx.mlx,
			render->mlx.win) == -1)
	{
		hud_cleanup(&render->hud, render->mlx.mlx);
		free(render);
		return (-1);
	}
	render->hud.total_pages = hud_calculate_total_pages(scene);
	return (0);
}

static void	register_hooks(t_render *render)
{
	mlx_hook(render->mlx.win, 17, 0, close_window, render);
	mlx_hook(render->mlx.win, 2, 1L << 0, handle_key, render);
	mlx_hook(render->mlx.win, 3, 1L << 1, handle_key_release, render);
	mlx_hook(render->mlx.win, 12, 1L << 15, handle_expose, render);
	mlx_loop_hook(render->mlx.mlx, render_loop, render);
}

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
	if (init_ui_components(render, scene) == -1)
		return (NULL);
	register_hooks(render);
	return (render);
}

void	render_destroy(t_render *render)
{
	if (!render)
		return ;
	hud_cleanup(&render->hud, render->mlx.mlx);
	keyguide_cleanup(&render->keyguide, render->mlx.mlx);
	pixel_timing_cleanup(&render->pixel_timing);
	mlx_context_destroy(&render->mlx);
	free(render);
}
