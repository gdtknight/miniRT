/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/15 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static t_scene	*init_scene(void)
{
	t_scene	*scene;

	scene = malloc(sizeof(t_scene));
	if (!scene)
		return (NULL);
	memset(scene, 0, sizeof(t_scene));
	scene->sphere_count = 0;
	scene->plane_count = 0;
	scene->cylinder_count = 0;
	scene->has_ambient = 0;
	scene->has_camera = 0;
	scene->has_light = 0;
	return (scene);
}

int	main(int argc, char **argv)
{
	t_scene		*scene;
	t_render	*render;

	if (argc != 2)
	{
		printf("Usage: %s <scene_file.rt>\n", argv[0]);
		return (1);
	}
	scene = init_scene();
	if (!scene)
		return (print_error("Failed to initialize scene"));
	if (!parse_scene(argv[1], scene))
	{
		cleanup_scene(scene);
		return (1);
	}
	render = init_window(scene);
	if (!render)
	{
		cleanup_scene(scene);
		return (print_error("Failed to initialize window"));
	}
	render_scene(scene, render->mlx, render->win);
	mlx_loop(render->mlx);
	return (0);
}
