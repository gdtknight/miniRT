/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/15 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "vec3.h"
#include "ray.h"
#include "window.h"

t_ray		create_camera_ray(t_camera *camera, double x, double y);
t_color		trace_ray(t_scene *scene, t_ray *ray);

static void	put_pixel_color(void *mlx, void *win, int xy[2], t_color color)
{
	extern int	mlx_pixel_put(void *, void *, int, int, int);

	mlx_pixel_put(mlx, win, xy[0], xy[1], (color.r << 16) | \
		(color.g << 8) | color.b);
}

static void	render_pixel(t_scene *scene, void *mlx_win[2], int x, int y)
{
	t_ray	ray;
	t_color	color;
	double	u;
	double	v;
	int		xy[2];

	u = (2.0 * x / 800.0) - 1.0;
	v = 1.0 - (2.0 * y / 600.0);
	ray = create_camera_ray(&scene->camera, u, v);
	color = trace_ray(scene, &ray);
	xy[0] = x;
	xy[1] = y;
	put_pixel_color(mlx_win[0], mlx_win[1], xy, color);
}

void	render_scene(t_scene *scene, void *mlx, void *win)
{
	int		x;
	int		y;
	void	*mlx_win[2];

	mlx_win[0] = mlx;
	mlx_win[1] = win;
	y = 0;
	while (y < 600)
	{
		x = 0;
		while (x < 800)
		{
			render_pixel(scene, mlx_win, x, y);
			x++;
		}
		y++;
	}
}
