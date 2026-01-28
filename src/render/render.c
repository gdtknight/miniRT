/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:32:45 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "ray.h"
#include "window.h"
#include "pixel_timing.h"
#include "metrics.h"

/*
** Convert screen pixel coordinates to Normalized Device Coordinates.
** NDC range: u in [-1, 1] (left to right), v in [-1, 1] (bottom to top).
*/
/*
** Write color directly to image buffer for fast rendering.
** Converts RGB color to packed integer and writes to memory.
*/
/**
 * @brief put pixel to buffer 함수
 *
 * @param render 파라미터
 * @param x 파라미터
 * @param y 파라미터
 * @param color 파라미터
 */
static void	put_pixel_to_buffer(t_render *render, int x, int y, t_color color)
{
	int		offset;
	int		pixel_color;

	if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
		return ;
	offset = y * render->mlx.img.size_line + x * (render->mlx.img.bpp / 8);
	pixel_color = (color.r << 16) | (color.g << 8) | color.b;
	*(int *)(render->mlx.img.data + offset) = pixel_color;
}

/*
** Render single pixel at screen coordinates (x, y).
** Converts screen space to normalized device coordinates.
** Creates camera ray, traces it, and writes resulting color to buffer.
*/
/**
 * @brief render pixel 함수 - 렌더링 수행
 *
 * @param scene 파라미터
 * @param render 파라미터
 * @param x 파라미터
 * @param y 파라미터
 */
static void	render_pixel(t_scene *scene, t_render *render, int x, int y)
{
	t_ray	ray;
	t_color	color;
	double	u;
	double	v;
	long	timing[2];

	u = (2.0 * x / (double)WINDOW_WIDTH) - 1.0;
	v = 1.0 - (2.0 * y / (double)WINDOW_HEIGHT);
	ray = create_camera_ray(&scene->camera, u, v);
	timing[0] = get_time_ns();
	color = trace_ray(scene, &ray);
	timing[1] = get_time_ns();
	pixel_timing_add_sample(&render->pixel_timing, timing[1] - timing[0]);
	put_pixel_to_buffer(render, x, y, color);
}

/*
** Draw a 2x2 pixel block with the same color for low quality rendering.
*/
/**
 * @brief draw pixel block 함수 - 그리기 수행
 *
 * @param render 파라미터
 * @param x 파라미터
 * @param y 파라미터
 * @param color 파라미터
 */
static void	draw_pixel_block(t_render *render, int x, int y, t_color color)
{
	put_pixel_to_buffer(render, x, y, color);
	if (x + 1 < WINDOW_WIDTH)
		put_pixel_to_buffer(render, x + 1, y, color);
	if (y + 1 < WINDOW_HEIGHT)
	{
		put_pixel_to_buffer(render, x, y + 1, color);
		if (x + 1 < WINDOW_WIDTH)
			put_pixel_to_buffer(render, x + 1, y + 1, color);
	}
}

/*
** Render scene at reduced resolution for fast preview.
** Uses 2x2 pixel blocks to achieve 4x speedup.
*/
/**
 * @brief render low quality 함수 - 렌더링 수행
 *
 * @param scene 파라미터
 * @param render 파라미터
 */
static void	render_low_quality(t_scene *scene, t_render *render)
{
	int		x;
	int		y;
	t_ray	ray;
	t_color	color;
	double	uv[2];

	y = 0;
	while (y < WINDOW_HEIGHT)
	{
		if (render->debounce.cancel_requested)
			return ;
		x = 0;
		while (x < WINDOW_WIDTH)
		{
			uv[0] = (2.0 * x / (double)WINDOW_WIDTH) - 1.0;
			uv[1] = 1.0 - (2.0 * y / (double)WINDOW_HEIGHT);
			ray = create_camera_ray(&scene->camera, uv[0], uv[1]);
			color = trace_ray(scene, &ray);
			draw_pixel_block(render, x, y, color);
			x += 2;
		}
		y += 2;
	}
}

/*
** Render entire scene to image buffer.
** Uses low quality mode if requested for faster preview.
** Otherwise renders at full 800x600 resolution.
*/
/**
 * @brief render scene to buffer 함수 - 렌더링 수행
 *
 * @param scene 파라미터
 * @param render 파라미터
 */
void	render_scene_to_buffer(t_scene *scene, t_render *render)
{
	int		x;
	int		y;

	if (render_has_flag(render, RENDER_LOW_QUALITY))
	{
		render_low_quality(scene, render);
		return ;
	}
	y = 0;
	while (y < WINDOW_HEIGHT)
	{
		if (render->debounce.cancel_requested)
			return ;
		x = 0;
		while (x < WINDOW_WIDTH)
		{
			render_pixel(scene, render, x, y);
			x++;
		}
		y++;
	}
	pixel_timing_calculate_stats(&render->pixel_timing);
	pixel_timing_print_stats(&render->pixel_timing);
}
