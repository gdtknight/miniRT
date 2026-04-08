/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display_pixel.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render/window.h"

/**
 * @brief Encode a color integer into BGRA byte order.
 */
static void	encode_pixel_bytes(char *dst, int color)
{
	dst[0] = (char)(color & 0xFF);
	dst[1] = (char)((color >> 8) & 0xFF);
	dst[2] = (char)((color >> 16) & 0xFF);
	dst[3] = 0;
}

/**
 * @brief Write a pixel value into the MLX image buffer.
 *
 * Computes the byte offset for the (x, y) coordinate and writes the packed
 * color value in BGRA little-endian format.
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
	encode_pixel_bytes(dst, color);
}
