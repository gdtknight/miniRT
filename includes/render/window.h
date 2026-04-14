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

/**
 * @brief Initialize an MLX connection, window, and back buffer.
 *
 * @param ctx Context structure to initialize.
 * @param width Window width in pixels.
 * @param height Window height in pixels.
 * @param title Window title string.
 * @return 1 on success, 0 on failure.
 */
int		mlx_context_init(t_mlx_context *ctx, int width, int height,
			char *title);

/**
 * @brief Destroy MLX resources held by the context.
 *
 * Releases the image buffer, window, and (on Linux) the display
 * connection.
 *
 * @param ctx Context structure to destroy.
 */
void	mlx_context_destroy(t_mlx_context *ctx);

/**
 * @brief Initialize an MLX image buffer and its metadata.
 *
 * @param img Image structure to initialize.
 * @param mlx MLX connection pointer.
 * @param width Image width in pixels.
 * @param height Image height in pixels.
 * @return 1 on success, 0 on failure.
 */
int		mlx_img_init(t_mlx_img *img, void *mlx, int width, int height);

/**
 * @brief Destroy an MLX image and reset its fields.
 *
 * @param img Image structure to destroy.
 * @param mlx MLX connection pointer.
 */
void	mlx_img_destroy(t_mlx_img *img, void *mlx);

/* Pixel operations */

/**
 * @brief Write a pixel value into the MLX image buffer.
 *
 * Writes the packed color in BGRA little-endian format. Coordinates
 * outside the image bounds are silently ignored.
 *
 * @param img Image buffer to modify.
 * @param x Pixel x coordinate.
 * @param y Pixel y coordinate.
 * @param color Packed color value (0xRRGGBB).
 */
void	mlx_img_put_pixel(t_mlx_img *img, int x, int y, int color);

#endif
