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
#include "bvh_vis.h"
#include "texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Parse CLI arguments and extract options/scene filename.
 *
 * Scans argv for the optional "--bvh-vis" flag and a single scene file path.
 * Returns failure if an unknown option is provided or if multiple scene files
 * are passed. On success, updates filename and bvh_vis output parameters.
 *
 * @param argc Argument count from main.
 * @param argv Argument vector from main.
 * @param filename Output pointer for the scene file path.
 * @param bvh_vis Output flag for BVH visualization mode.
 * @return int 1 on success (scene file present), 0 on failure.
 */
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

/**
 * @brief Create a scene, parse it from file, and build the BVH.
 *
 * Allocates a new scene, parses the given .rt file into it, and builds
 * acceleration structures. On failure, cleans up and reports errors.
 *
 * @param filename Path to the scene file.
 * @param scene Output pointer receiving the created scene.
 * @return int 1 on success, 0 on failure.
 */
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
	if ((*scene)->bvh && !(*scene)->bvh->root)
		write(2, "Warning: BVH build failed, using brute force\n", 46);
	return (1);
}

/**
 * @brief Initialize rendering context and optional BVH visualization.
 *
 * Enables BVH visualization on the scene if requested and available,
 * then creates the render context tied to the given scene.
 *
 * @param scene Parsed scene used for rendering.
 * @param render Output pointer receiving the render context.
 * @param bvh_vis Non-zero to enable BVH visualization.
 * @return int 1 on success, 0 on failure.
 */
static int	init_render_ctx(t_scene *scene, t_render **render, int bvh_vis)
{
	if (bvh_vis && scene->bvh)
	{
		scene->bvh->visualize = 1;
		bvh_visualize(scene->bvh, NULL, scene);
	}
	*render = render_create(scene);
	if (!*render)
	{
		error_print(ERR_WINDOW_INIT);
		scene_destroy(scene);
		return (0);
	}
	if (!load_all_bump_maps(scene, (*render)->mlx.mlx))
	{
		cleanup_all_bump_maps(scene, (*render)->mlx.mlx);
		render_destroy(*render);
		scene_destroy(scene);
		return (0);
	}
	return (1);
}

/**
 * @brief Program entry point for miniRT.
 *
 * Validates CLI arguments, loads the scene, initializes the render context,
 * and starts the MLX event loop. Returns a non-zero exit code on failure.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int Exit status (0 on success, 1 on failure).
 */
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
