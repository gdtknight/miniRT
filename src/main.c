/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:38 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "window.h"
#include "spatial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int	parse_args(int argc, char **argv, char **filename, int *bvh_vis)
{
	int	i;

	*filename = NULL;
	*bvh_vis = 0;
	i = 0;
	while (++i < argc)
	{
		if (strcmp(argv[i], "--bvh-vis") == 0)
			*bvh_vis = 1;
		else if (argv[i][0] == '-')
			return (printf("Unknown option: %s\n", argv[i]), 0);
		else if (*filename == NULL)
			*filename = argv[i];
		else
			return (printf("Multiple scene files provided\n"), 0);
	}
	return (*filename != NULL);
}

static int	init_and_parse(char *filename, t_scene **scene)
{
	*scene = scene_create();
	if (!*scene)
	{
		error_print(ERR_MALLOC);
		return (0);
	}
	if (!parse_scene(filename, *scene))
	{
		scene_destroy(*scene);
		return (0);
	}
	scene_build_bvh(*scene);
	return (1);
}

static int	init_render_ctx(t_scene *scene, t_render **render, int bvh_vis)
{
	if (bvh_vis && scene->bvh)
		scene->bvh->visualize = 1;
	*render = render_create(scene);
	if (!*render)
	{
		error_print(ERR_WINDOW_INIT);
		scene_destroy(scene);
		return (0);
	}
	return (1);
}

int	main(int argc, char **argv)
{
	t_scene		*scene;
	t_render	*render;
	char		*filename;
	int			bvh_vis;

	if (argc < 2)
	{
		printf("Usage: %s <scene_file.rt> [--bvh-vis]\n", argv[0]);
		return (1);
	}
	if (!parse_args(argc, argv, &filename, &bvh_vis))
	{
		printf("Usage: %s <scene_file.rt> [--bvh-vis]\n", argv[0]);
		return (1);
	}
	if (!init_and_parse(filename, &scene))
		return (1);
	if (!init_render_ctx(scene, &render, bvh_vis))
		return (1);
	mlx_loop(render->mlx.mlx);
	return (0);
}
