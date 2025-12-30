/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_transparency.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/30 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

int	get_red(int color)
{
	return ((color >> 16) & 0xFF);
}

int	get_green(int color)
{
	return ((color >> 8) & 0xFF);
}

int	get_blue(int color)
{
	return (color & 0xFF);
}

int	make_color(int r, int g, int b)
{
	return ((r << 16) | (g << 8) | b);
}

int	blend_colors(int bg_color, int fg_color, double alpha)
{
	int	r;
	int	g;
	int	b;

	r = (int)((1.0 - alpha) * get_red(bg_color)
			+ alpha * get_red(fg_color));
	g = (int)((1.0 - alpha) * get_green(bg_color)
			+ alpha * get_green(fg_color));
	b = (int)((1.0 - alpha) * get_blue(bg_color)
			+ alpha * get_blue(fg_color));
	return (make_color(r, g, b));
}

int	get_pixel(char *img_data, int x, int y, int size_line, int bpp)
{
	char	*pixel;

	pixel = img_data + (y * size_line + x * (bpp / 8));
	return (*(int *)pixel);
}

void	set_pixel(char *img_data, int x, int y, int color,
	int size_line, int bpp)
{
	char	*pixel;

	pixel = img_data + (y * size_line + x * (bpp / 8));
	*(int *)pixel = color;
}
