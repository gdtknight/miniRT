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

/**
 * @brief Write a pixel value into the MLX image buffer.
 *
 * Computes the byte offset for the (x, y) coordinate and writes the packed
 * color value directly into the image data buffer.
 *
 * @param img Image buffer to modify.
 * @param x Pixel x coordinate.
 * @param y Pixel y coordinate.
 * @param color Packed color value (0xRRGGBB).
 */
void	mlx_img_put_pixel(t_mlx_img *img, int x, int y, int color)
{
	char	*dst;

	if (!img || !img->data)
		return ;
	if (x < 0 || x >= img->width || y < 0 || y >= img->height)
		return ;
	dst = img->data + (y * img->size_line + x * (img->bpp / 8));
	*(unsigned int *)dst = color;
}

/**
 * @brief Read a pixel value from the MLX image buffer.
 *
 * Computes the byte offset for the (x, y) coordinate and returns the packed
 * color value stored at that location.
 *
 * @param img Image buffer to read from.
 * @param x Pixel x coordinate.
 * @param y Pixel y coordinate.
 * @return int Packed color value (0xRRGGBB) or 0 on invalid input.
 */
int	mlx_img_get_pixel(t_mlx_img *img, int x, int y)
{
	char	*src;

	if (!img || !img->data)
		return (0);
	if (x < 0 || x >= img->width || y < 0 || y >= img->height)
		return (0);
	src = img->data + (y * img->size_line + x * (img->bpp / 8));
	return (*(unsigned int *)src);
}
