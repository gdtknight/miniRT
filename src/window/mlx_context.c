/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_context.c                                      :+:      :+:    :+:   */
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
	return (1);
}

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
		return (0);
	if (!mlx_img_init(&ctx->img, ctx->mlx, width, height))
		return (0);
	return (1);
}

void	mlx_context_destroy(t_mlx_context *ctx)
{
	if (!ctx)
		return ;
	mlx_img_destroy(&ctx->img, ctx->mlx);
	if (ctx->win && ctx->mlx)
		mlx_destroy_window(ctx->mlx, ctx->win);
	ctx->mlx = NULL;
	ctx->win = NULL;
}
