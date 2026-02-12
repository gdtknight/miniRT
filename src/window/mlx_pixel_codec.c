/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_pixel_codec.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 14:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/04 14:30:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mlx_context.h"

static void	decode_rgb(const char *src, int bpp, int endian, unsigned char *rgb)
{
	if (bpp < 24)
	{
		rgb[0] = 0;
		rgb[1] = 0;
		rgb[2] = 0;
		return ;
	}
	if (bpp == 24 && endian == 1)
	{
		rgb[0] = (unsigned char)src[0];
		rgb[1] = (unsigned char)src[1];
		rgb[2] = (unsigned char)src[2];
	}
	else if (endian == 0)
	{
		rgb[0] = (unsigned char)src[2];
		rgb[1] = (unsigned char)src[1];
		rgb[2] = (unsigned char)src[0];
	}
	else
	{
		rgb[0] = (unsigned char)src[1];
		rgb[1] = (unsigned char)src[2];
		rgb[2] = (unsigned char)src[3];
	}
}

static void	encode_rgb(char *dst, int bpp, int endian, unsigned char *rgb)
{
	if (bpp == 24 && endian == 1)
	{
		dst[0] = rgb[0];
		dst[1] = rgb[1];
		dst[2] = rgb[2];
	}
	else if (endian == 0)
	{
		dst[0] = rgb[2];
		dst[1] = rgb[1];
		dst[2] = rgb[0];
		if (bpp != 24)
			dst[3] = 0;
	}
	else
	{
		dst[0] = 0;
		dst[1] = rgb[0];
		dst[2] = rgb[1];
		dst[3] = rgb[2];
	}
}

void	mlx_encode_pixel_bytes(char *dst, int bpp, int endian, int color)
{
	unsigned char	rgb[3];

	rgb[0] = (unsigned char)((color >> 16) & 0xFF);
	rgb[1] = (unsigned char)((color >> 8) & 0xFF);
	rgb[2] = (unsigned char)(color & 0xFF);
	encode_rgb(dst, bpp, endian, rgb);
}

int	mlx_decode_pixel_bytes(const char *src, int bpp, int endian)
{
	unsigned char	rgb[3];

	decode_rgb(src, bpp, endian, rgb);
	return ((rgb[0] << 16) | (rgb[1] << 8) | rgb[2]);
}
