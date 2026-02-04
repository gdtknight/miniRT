/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"
#include <stdlib.h>

static void	fill_background_pixels(t_pixel_params *params, int color)
{
	int	x;
	int	y;

	y = 0;
	while (y < HUD_HEIGHT)
	{
		params->y = y;
		x = 0;
		while (x < HUD_WIDTH)
		{
			params->x = x;
			set_pixel(params, color);
			x++;
		}
		y++;
	}
}

/**
 * @brief Create the HUD background image buffer.
 *
 * Allocates the background image and fills it with a semi-transparent color.
 *
 * @param hud HUD state to populate.
 * @param mlx MLX connection pointer.
 * @return int 0 on success, -1 on failure.
 */
int	hud_create_background(t_hud_state *hud, void *mlx)
{
	int				pixel;
	t_pixel_params	params;

	hud->bg_img = mlx_new_image(mlx, HUD_WIDTH, HUD_HEIGHT);
	if (!hud->bg_img)
		return (-1);
	hud->bg_data = mlx_get_data_addr(hud->bg_img, &hud->bpp,
			&hud->size_line, &hud->endian);
	params.img_data = hud->bg_data;
	params.size_line = hud->size_line;
	params.bpp = hud->bpp;
	params.endian = hud->endian;
	pixel = (int)(HUD_COLOR_BG * HUD_BG_ALPHA);
	fill_background_pixels(&params, pixel);
	return (0);
}

/**
 * @brief Calculate the number of object list pages for the HUD.
 *
 * @param scene Scene containing object list.
 * @return int Total number of pages (minimum 1).
 */
int	hud_calculate_total_pages(t_scene *scene)
{
	int	total_objects;
	int	total_pages;

	total_objects = scene->objects.count;
	if (total_objects == 0)
		return (1);
	total_pages = (total_objects + HUD_OBJECTS_PER_PAGE - 1)
		/ HUD_OBJECTS_PER_PAGE;
	return (total_pages);
}

/**
 * @brief Initialize HUD state and background resources.
 *
 * Sets initial state values and allocates the background image.
 *
 * @param hud HUD state to initialize.
 * @param mlx MLX connection pointer.
 * @param win Window pointer (unused but kept for API consistency).
 * @return int 0 on success, -1 on failure.
 */
int	hud_init(t_hud_state *hud, void *mlx, void *win)
{
	(void)win;
	hud->visible = 1;
	hud->current_page = 0;
	hud->objects_per_page = HUD_OBJECTS_PER_PAGE;
	hud->dirty = 1;
	hud->bg_img = NULL;
	hud->bg_data = NULL;
	if (hud_create_background(hud, mlx) == -1)
		return (-1);
	return (0);
}

/**
 * @brief Destroy HUD background resources.
 *
 * Frees the background image and clears cached pointers.
 *
 * @param hud HUD state to clean up.
 * @param mlx MLX connection pointer.
 */
void	hud_cleanup(t_hud_state *hud, void *mlx)
{
	if (hud->bg_img)
	{
		mlx_destroy_image(mlx, hud->bg_img);
		hud->bg_img = NULL;
		hud->bg_data = NULL;
	}
}
