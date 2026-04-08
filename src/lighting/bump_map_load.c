/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bump_map_load.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/10 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lighting/texture.h"
#include "scene/scene.h"
#include "common/error.h"
#include "mlx.h"
#include <stdlib.h>

t_bump_map	*bump_map_load(void *mlx, char *filepath)
{
	t_bump_map	*bmap;

	bmap = malloc(sizeof(t_bump_map));
	if (!bmap)
		return (NULL);
	bmap->img = mlx_xpm_file_to_image(mlx, filepath,
			&bmap->width, &bmap->height);
	if (!bmap->img)
	{
		free(bmap);
		return (NULL);
	}
	bmap->data = mlx_get_data_addr(bmap->img, &bmap->bpp,
			&bmap->size_line, &bmap->endian);
	if (!bmap->data)
	{
		mlx_destroy_image(mlx, bmap->img);
		free(bmap);
		return (NULL);
	}
	return (bmap);
}

void	bump_map_destroy(void *mlx, t_bump_map *bmap)
{
	if (!bmap)
		return ;
	if (bmap->img)
		mlx_destroy_image(mlx, bmap->img);
	free(bmap);
}

int	load_all_bump_maps(void *scene_ptr, void *mlx)
{
	t_scene		*scene;
	t_object	*obj;
	int			i;

	scene = (t_scene *)scene_ptr;
	i = 0;
	while (i < scene->objects.count)
	{
		obj = &scene->objects.items[i];
		if (obj->bump_path)
		{
			obj->bump_map = bump_map_load(mlx, obj->bump_path);
			if (!obj->bump_map)
			{
				error_print_ctx("bump map: ", obj->bump_path);
				return (0);
			}
		}
		i++;
	}
	return (1);
}

void	cleanup_all_bump_maps(void *scene_ptr, void *mlx)
{
	t_scene		*scene;
	t_object	*obj;
	int			i;

	scene = (t_scene *)scene_ptr;
	i = 0;
	while (i < scene->objects.count)
	{
		obj = &scene->objects.items[i];
		if (obj->bump_map)
			bump_map_destroy(mlx, obj->bump_map);
		if (obj->bump_path)
			free(obj->bump_path);
		obj->bump_map = NULL;
		obj->bump_path = NULL;
		i++;
	}
}
