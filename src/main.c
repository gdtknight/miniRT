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
#include "render.h"
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
 * @brief Create a scene and parse it from a file.
 *
 * Allocates a new scene and parses the given .rt file into it.
 * On failure, cleans up and reports errors.
 *
 * @param filename Path to the scene file.
 * @param scene Output pointer receiving the created scene.
 * @return int 1 on success, 0 on failure.
 */
static int	init_scene(char *filename, t_scene **scene)
{
	*scene = scene_create();
	if (!*scene)
		return (error_print(ERR_MALLOC), 0);
	if (!parse_scene(filename, *scene))
	{
		scene_destroy(*scene);
		return (0);
	}
	return (1);
}

/**
 * @brief Create the render context for a scene.
 *
 * Initializes the MLX window and image tied to the given scene.
 * On failure, destroys the scene and reports errors.
 *
 * @param scene Parsed scene used for rendering.
 * @param render Output pointer receiving the render context.
 * @return int 1 on success, 0 on failure.
 */
static int	init_render(t_scene *scene, t_render **render)
{
	*render = render_create(scene);
	if (!*render)
	{
		error_print(ERR_WINDOW_INIT);
		scene_destroy(scene);
		return (0);
	}
	return (1);
}

/**
 * @brief Load bump map textures for all scene objects.
 *
 * Loads XPM bump maps via MLX. On failure, cleans up all resources.
 *
 * @param scene Scene containing objects with bump map paths.
 * @param render Render context providing the MLX handle.
 * @return int 1 on success, 0 on failure.
 */
static int	load_textures(t_scene *scene, t_render *render)
{
	if (!load_all_bump_maps(scene, render->mlx.mlx))
	{
		cleanup_all_bump_maps(scene, render->mlx.mlx);
		render_destroy(render);
		scene_destroy(scene);
		return (0);
	}
	return (1);
}

/**
 * @brief Program entry point for miniRT.
 *
 * Parses CLI arguments, loads the scene, builds the BVH, initializes
 * the render context, loads textures, and starts the MLX event loop.
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

	if (!parse_args(argc, argv, &filename, &bvh_vis))
		return (printf("Usage: %s <scene.rt> [--bvh-vis]\n", argv[0]), 1);
	if (!init_scene(filename, &scene))
		return (1);
	scene_build_bvh(scene);
	if (bvh_vis && scene->bvh)
		scene->bvh->visualize = 1;
	bvh_visualize(scene->bvh, NULL, scene);
	if (!init_render(scene, &render))
		return (1);
	if (!load_textures(scene, render))
		return (1);
	mlx_loop(render->mlx.mlx);
	cleanup_all_bump_maps(scene, render->mlx.mlx);
	render_destroy(render);
	scene_destroy(scene);
	return (0);
}
