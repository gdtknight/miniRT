/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:34:16 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render.h"
#include "metrics.h"
#include "ray.h"

/**
 * @brief Write a single pixel color into the MLX image buffer.
 *
 * Computes the byte offset for (x, y) within the image data, converts the
 * RGB components to a packed integer, and stores it directly. Coordinates
 * outside the render resolution are ignored.
 *
 * @param render Active render context containing the image buffer.
 * @param x Pixel x coordinate in screen space.
 * @param y Pixel y coordinate in screen space.
 * @param color RGB color to write.
 */
static void	put_pixel_to_buffer(t_render *render, int x, int y, t_color color)
{
	int		pixel_color;

	pixel_color = (color.r << 16) | (color.g << 8) | color.b;
	mlx_img_put_pixel(&render->mlx.img, x, y, pixel_color);
}

/**
 * @brief Render a single pixel by tracing a camera ray.
 *
 * Converts pixel coordinates to normalized device coordinates, generates
 * a camera ray, traces it against the scene, records timing, and writes
 * the resulting color to the image buffer.
 *
 * @param scene Scene to be rendered.
 * @param render Render context for timing and output.
 * @param x Pixel x coordinate in screen space.
 * @param y Pixel y coordinate in screen space.
 */
static void	render_pixel(t_scene *scene, t_render *render, int x, int y)
{
	t_ray	ray;
	t_color	color;
	double	u;
	double	v;

	u = (2.0 * x / (double)WINDOW_WIDTH) - 1.0;
	v = 1.0 - (2.0 * y / (double)WINDOW_HEIGHT);
	ray = create_camera_ray(&scene->camera, u, v);
	color = trace_ray(scene, &ray);
	put_pixel_to_buffer(render, x, y, color);
}

/**
 * @brief Draw a 2x2 block for low-quality preview rendering.
 *
 * Writes the given color to (x, y) and, if in bounds, to the three adjacent
 * pixels that form a 2x2 block. This trades quality for speed.
 *
 * @param render Render context with image buffer.
 * @param x Top-left x coordinate of the block.
 * @param y Top-left y coordinate of the block.
 * @param color RGB color to fill.
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

/**
 * @brief Render the scene at reduced resolution for fast preview.
 *
 * Steps through the screen in 2-pixel increments, traces one ray per 2x2
 * block, and fills the block with the same color. Aborts early if a debounce
 * cancel is requested.
 *
 * @param scene Scene to be rendered.
 * @param render Render context containing state and output buffer.
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

/**
 * @brief Render the full scene into the image buffer.
 *
 * Chooses low-quality or full-quality rendering based on render flags,
 * iterates over all pixels, and records per-pixel timing statistics.
 * Aborts early if a debounce cancel is requested.
 *
 * @param scene Scene to be rendered.
 * @param render Render context containing output buffer and state.
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
		x = 0;
		while (x < WINDOW_WIDTH)
		{
			render_pixel(scene, render, x, y);
			x++;
		}
		y++;
	}
}
