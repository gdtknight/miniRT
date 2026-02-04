/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_context.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MLX_CONTEXT_H
# define MLX_CONTEXT_H

/**
 * @brief MLX image data structure
 *
 * Contains all necessary data for MLX image manipulation.
 */
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

/**
 * @brief MLX context structure
 *
 * Encapsulates all MLX-related pointers and image data.
 */
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
void	mlx_encode_pixel_bytes(char *dst, int bpp, int endian, int color);
int		mlx_decode_pixel_bytes(const char *src, int bpp, int endian);
void	mlx_img_put_pixel(t_mlx_img *img, int x, int y, int color);
int		mlx_img_get_pixel(t_mlx_img *img, int x, int y);

#endif
