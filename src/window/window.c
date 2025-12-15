/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/15 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include <stdlib.h>

int	close_window(void *param)
{
	t_render	*render;

	render = (t_render *)param;
	cleanup_all(render->scene, render);
	exit(0);
	return (0);
}

int	handle_key(int keycode, void *param)
{
	if (keycode == 65307)
		close_window(param);
	return (0);
}

t_render	*init_window(t_scene *scene)
{
	t_render	*render;

	render = malloc(sizeof(t_render));
	if (!render)
		return (NULL);
	render->mlx = mlx_init();
	if (!render->mlx)
	{
		free(render);
		return (NULL);
	}
	render->win = mlx_new_window(render->mlx, 800, 600, "miniRT");
	if (!render->win)
	{
		free(render);
		return (NULL);
	}
	render->scene = scene;
	mlx_hook(render->win, 17, 0, close_window, render);
	mlx_key_hook(render->win, handle_key, render);
	return (render);
}
