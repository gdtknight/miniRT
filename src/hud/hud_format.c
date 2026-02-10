/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_format.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

/**
 * @brief Format a floating-point value with two decimal places.
 *
 * Writes the formatted value into the provided buffer.
 *
 * @param buffer Output buffer.
 * @param value Value to format.
 */
void	hud_format_float(char *buffer, double value)
{
	int		int_part;
	int		frac_part;
	int		i;

	if (value < 0)
	{
		buffer[0] = '-';
		value = -value;
		i = 1;
	}
	else
		i = 0;
	int_part = (int)value;
	frac_part = (int)((value - int_part) * 100);
	ft_itoa_buf(buffer + i, int_part);
	while (buffer[i])
		i++;
	buffer[i++] = '.';
	if (frac_part < 10)
		buffer[i++] = '0';
	ft_itoa_buf(buffer + i, frac_part);
}

/**
 * @brief Format a vec3 as "(x, y, z)" with two decimals each.
 *
 * @param buffer Output buffer.
 * @param vec Vector to format.
 */
void	hud_format_vec3(char *buffer, t_vec3 vec)
{
	int	i;

	buffer[0] = '(';
	i = 1;
	hud_format_float(buffer + i, vec.x);
	while (buffer[i])
		i++;
	ft_strcpy(buffer + i, ", ");
	i += 2;
	hud_format_float(buffer + i, vec.y);
	while (buffer[i])
		i++;
	ft_strcpy(buffer + i, ", ");
	i += 2;
	hud_format_float(buffer + i, vec.z);
	while (buffer[i])
		i++;
	buffer[i] = ')';
	buffer[i + 1] = '\0';
}

/**
 * @brief Format a color as "R:.. G:.. B:..".
 *
 * @param buffer Output buffer.
 * @param color Color to format.
 */
void	hud_format_color(char *buffer, t_color color)
{
	int	i;

	ft_strcpy(buffer, "R:");
	i = 2;
	ft_itoa_buf(buffer + i, color.r);
	while (buffer[i])
		i++;
	ft_strcpy(buffer + i, " G:");
	i += 3;
	ft_itoa_buf(buffer + i, color.g);
	while (buffer[i])
		i++;
	ft_strcpy(buffer + i, " B:");
	i += 3;
	ft_itoa_buf(buffer + i, color.b);
}
