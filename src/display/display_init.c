/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display_init.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mlx_context.h"
#include "mlx.h"
#include "libft.h"

/**
 * @brief Initialize an MLX image buffer and metadata.
 *
 * Allocates a new image via MLX, retrieves the raw pixel buffer address,
 * and stores image properties in the provided struct.
 *
 * @param img Image structure to initialize.
 * @param mlx MLX connection pointer.
 * @param width Image width in pixels.
 * @param height Image height in pixels.
 * @return int 1 on success, 0 on failure.
 */
int	mlx_img_init(t_mlx_img *img, void *mlx, int width, int height)
{
	if (!img || !mlx)
		return (0);
	ft_bzero(img, sizeof(t_mlx_img));
	img->img = mlx_new_image(mlx, width, height);
	if (!img->img)
		return (0);
	img->data = mlx_get_data_addr(img->img, &img->bpp,
			&img->size_line, &img->endian);
	img->width = width;
	img->height = height;
	return (1);
}

/**
 * @brief Initialize MLX connection, window, and back buffer.
 *
 * Creates an MLX connection, window, and image buffer. All fields are
 * zeroed prior to initialization to ensure safe cleanup on failure.
 *
 * @param ctx Context structure to initialize.
 * @param width Window width in pixels.
 * @param height Window height in pixels.
 * @param title Window title string.
 * @return int 1 on success, 0 on failure.
 */
int	mlx_context_init(t_mlx_context *ctx, int width, int height, char *title)
{
	if (!ctx)
		return (0);
	ft_bzero(ctx, sizeof(t_mlx_context));
	ctx->mlx = mlx_init();
	if (!ctx->mlx)
		return (0);
	ctx->win = mlx_new_window(ctx->mlx, width, height, title);
	if (!ctx->win)
		return (mlx_context_destroy(ctx), 0);
	if (!mlx_img_init(&ctx->img, ctx->mlx, width, height))
		return (mlx_context_destroy(ctx), 0);
	return (1);
}
