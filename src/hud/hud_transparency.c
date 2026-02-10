/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_transparency.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/03 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

/**
 * @brief Read a pixel color from the image buffer.
 *
 * @param params Pixel access parameters.
 * @return int Packed color value.
 */
int	get_pixel(t_pixel_params *params)
{
	char	*pixel;

	pixel = params->img_data + (params->y * params->size_line
			+ params->x * (params->bpp / 8));
	return (mlx_decode_pixel_bytes(pixel, params->bpp, params->endian));
}

/**
 * @brief Write a pixel color into the image buffer.
 *
 * @param params Pixel access parameters.
 * @param color Packed color value.
 */
void	set_pixel(t_pixel_params *params, int color)
{
	char	*pixel;

	pixel = params->img_data + (params->y * params->size_line
			+ params->x * (params->bpp / 8));
	mlx_encode_pixel_bytes(pixel, params->bpp, params->endian, color);
}
