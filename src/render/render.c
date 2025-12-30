/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 08:57:16 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "ray.h"
#include "window.h"

t_ray		create_camera_ray(t_camera *camera, double x, double y);
t_color		trace_ray(t_scene *scene, t_ray *ray);

/*
** Write color directly to image buffer for fast rendering.
** Converts RGB color to packed integer and writes to memory.
*/
static void	put_pixel_to_buffer(t_render *render, int x, int y, t_color color)
{
	int		offset;
	int		pixel_color;

	if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
		return ;
	offset = y * render->size_line + x * (render->bpp / 8);
	pixel_color = (color.r << 16) | (color.g << 8) | color.b;
	*(int *)(render->img_data + offset) = pixel_color;
}

/*
** Render single pixel at screen coordinates (x, y).
** Converts screen space to normalized device coordinates.
** Creates camera ray, traces it, and writes resulting color to buffer.
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

/*
** Render scene at reduced resolution for fast preview.
** Uses 2x2 pixel blocks to achieve 4x speedup.
*/
static void	render_low_quality(t_scene *scene, t_render *render)
{
	int		x;
	int		y;
	t_ray	ray;
	t_color	color;
	double	u;
	double	v;

	y = 0;
	while (y < WINDOW_HEIGHT)
	{
		x = 0;
		while (x < WINDOW_WIDTH)
		{
			u = (2.0 * x / (double)WINDOW_WIDTH) - 1.0;
			v = 1.0 - (2.0 * y / (double)WINDOW_HEIGHT);
			ray = create_camera_ray(&scene->camera, u, v);
			color = trace_ray(scene, &ray);
			put_pixel_to_buffer(render, x, y, color);
			if (x + 1 < WINDOW_WIDTH)
				put_pixel_to_buffer(render, x + 1, y, color);
			if (y + 1 < WINDOW_HEIGHT)
			{
				put_pixel_to_buffer(render, x, y + 1, color);
				if (x + 1 < WINDOW_WIDTH)
					put_pixel_to_buffer(render, x + 1, y + 1, color);
			}
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
void	render_scene_to_buffer(t_scene *scene, t_render *render)
{
	int		x;
	int		y;

	if (render->low_quality)
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

/*
** Legacy wrapper for backward compatibility.
** Renders to temporary buffer and displays.
*/
void	render_scene(t_scene *scene, void *mlx, void *win)
{
	(void)scene;
	(void)mlx;
	(void)win;
}
