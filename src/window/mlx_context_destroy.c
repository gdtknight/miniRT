/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_context_destroy.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/03/24 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mlx_context.h"
#include "mlx.h"
#ifdef __linux__
# include <stdlib.h>
#endif

#ifdef __linux__

static void	mlx_free_display(void *mlx)
{
	if (!mlx)
		return ;
	mlx_destroy_display(mlx);
	free(mlx);
}

#else

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
