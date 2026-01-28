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

/**
 * @brief hud create background 함수 - 생성 수행
 *
 * @param hud 파라미터
 * @param mlx 파라미터
 *
 * @return int 반환값
 */
int	hud_create_background(t_hud_state *hud, void *mlx)
{
	int	x;
	int	y;
	int	pixel;

	hud->bg_img = mlx_new_image(mlx, HUD_WIDTH, HUD_HEIGHT);
	if (!hud->bg_img)
		return (-1);
	hud->bg_data = mlx_get_data_addr(hud->bg_img, &hud->bpp,
			&hud->size_line, &hud->endian);
	y = 0;
	while (y < HUD_HEIGHT)
	{
		x = 0;
		while (x < HUD_WIDTH)
		{
			pixel = (int)(HUD_COLOR_BG * HUD_BG_ALPHA);
			((int *)hud->bg_data)[y * (hud->size_line / 4) + x] = pixel;
			x++;
		}
		y++;
	}
	return (0);
}

/**
 * @brief hud calculate total pages 함수 - 계산 수행
 *
 * @param scene 파라미터
 *
 * @return int 반환값
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
 * @brief hud init 함수 - 초기화 수행
 *
 * @param hud 파라미터
 * @param mlx 파라미터
 * @param win 파라미터
 *
 * @return int 반환값
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
 * @brief hud cleanup 함수 - 정리 수행
 *
 * @param hud 파라미터
 * @param mlx 파라미터
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
