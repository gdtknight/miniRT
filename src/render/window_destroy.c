/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_destroy.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/03/24 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render/window.h"
#include <stdlib.h>

/**
 * @brief Destroy an MLX image and reset image fields.
 *
 * Frees the MLX image resource if present and clears cached pointers.
 *
 * @param img Image structure to destroy.
 * @param mlx MLX connection pointer.
 */
void	mlx_img_destroy(t_mlx_img *img, void *mlx)
{
	if (!img || !mlx)
		return ;
	if (img->img)
	{
		mlx_destroy_image(mlx, img->img);
		img->img = NULL;
	}
	img->data = NULL;
}

#ifdef __linux__

/**
 * @brief Free the MLX display connection on Linux.
 *
 * @param mlx MLX connection pointer to release.
 */
static void	mlx_free_display(void *mlx)
{
	if (!mlx)
		return ;
	mlx_destroy_display(mlx);
	free(mlx);
}

#else

/**
 * @brief No-op display cleanup on non-Linux platforms.
 *
 * @param mlx MLX connection pointer (unused).
 */
static void	mlx_free_display(void *mlx)
{
	(void)mlx;
}

#endif

/**
 * @brief Destroy MLX resources in the context.
 *
 * Releases the image buffer and window if they exist and clears pointers.
 *
 * @param ctx Context structure to destroy.
 */
void	mlx_context_destroy(t_mlx_context *ctx)
{
	if (!ctx)
		return ;
	mlx_img_destroy(&ctx->img, ctx->mlx);
	if (ctx->win && ctx->mlx)
		mlx_destroy_window(ctx->mlx, ctx->win);
	mlx_free_display(ctx->mlx);
	ctx->mlx = NULL;
	ctx->win = NULL;
}
