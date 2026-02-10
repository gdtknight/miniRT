/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_color.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

/**
 * @brief Pack RGB components into a single 0xRRGGBB integer.
 *
 * @param r Red component (0-255).
 * @param g Green component (0-255).
 * @param b Blue component (0-255).
 * @return int Packed color value.
 */
int	make_color(int r, int g, int b)
{
	return ((r << 16) | (g << 8) | b);
}

/**
 * @brief Blend a single color component using alpha.
 *
 * @param bg Background component value.
 * @param fg Foreground component value.
 * @param alpha Blend factor in [0, 1].
 * @return int Blended component value.
 */
static int	blend_component(int bg, int fg, double alpha)
{
	return ((int)(bg * (1.0 - alpha) + fg * alpha));
}

/**
 * @brief Blend two packed RGB colors with alpha.
 *
 * @param bg_color Background packed color.
 * @param fg_color Foreground packed color.
 * @param alpha Blend factor in [0, 1].
 * @return int Blended packed color.
 */
int	blend_colors(int bg_color, int fg_color, double alpha)
{
	int	r;
	int	g;
	int	b;

	r = blend_component(get_red(bg_color), get_red(fg_color), alpha);
	g = blend_component(get_green(bg_color), get_green(fg_color), alpha);
	b = blend_component(get_blue(bg_color), get_blue(fg_color), alpha);
	return (make_color(r, g, b));
}
