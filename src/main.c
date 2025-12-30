/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:38 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:21:10 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "window.h"
#include "spatial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
** Initialize scene structure with default values.
** Allocates memory and sets all counters to zero.
** Returns NULL on allocation failure.
*/
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
	scene->shadow_config = init_shadow_config();
	render_state_init(&scene->render_state);
	scene->render_state.bvh_enabled = 1;
	return (scene);
}

/*
** Main program entry point.
** Validates arguments, initializes scene, parses input file,
** creates window, and starts rendering loop.
*/
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
	scene_build_bvh(scene);
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
