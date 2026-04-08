/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/04/08 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WINDOW_H
# define WINDOW_H

# include "mlx.h"

/* Window resolution constants */
# define WINDOW_WIDTH 1440
# define WINDOW_HEIGHT 900
# define ASPECT_RATIO_NUM 1440.0
# define ASPECT_RATIO_DEN 900.0

typedef struct s_mlx_img
{
	void	*img;
	char	*data;
	int		bpp;
	int		size_line;
	int		endian;
	int		width;
	int		height;
}	t_mlx_img;

typedef struct s_mlx_context
{
	void		*mlx;
	void		*win;
	t_mlx_img	img;
}	t_mlx_context;

/* MLX context operations */
int		mlx_context_init(t_mlx_context *ctx, int width, int height,
			char *title);
void	mlx_context_destroy(t_mlx_context *ctx);
int		mlx_img_init(t_mlx_img *img, void *mlx, int width, int height);
void	mlx_img_destroy(t_mlx_img *img, void *mlx);

/* Pixel operations */
void	mlx_img_put_pixel(t_mlx_img *img, int x, int y, int color);

#endif
