/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_color_extract.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

/**
 * @brief Extract the red component from a packed color.
 *
 * @param color Packed color value.
 * @return int Red component (0-255).
 */
int	get_red(int color)
{
	return ((color >> 16) & 0xFF);
}

/**
 * @brief Extract the green component from a packed color.
 *
 * @param color Packed color value.
 * @return int Green component (0-255).
 */
int	get_green(int color)
{
	return ((color >> 8) & 0xFF);
}

/**
 * @brief Extract the blue component from a packed color.
 *
 * @param color Packed color value.
 * @return int Blue component (0-255).
 */
int	get_blue(int color)
{
	return (color & 0xFF);
}
