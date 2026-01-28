/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_pixel.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mlx_context.h"

void	mlx_img_put_pixel(t_mlx_img *img, int x, int y, int color)
{
	char	*dst;

	if (!img || !img->data)
		return ;
	dst = img->data + (y * img->size_line + x * (img->bpp / 8));
	*(unsigned int *)dst = color;
}

int	mlx_img_get_pixel(t_mlx_img *img, int x, int y)
{
	char	*src;

	if (!img || !img->data)
		return (0);
	src = img->data + (y * img->size_line + x * (img->bpp / 8));
	return (*(unsigned int *)src);
}
