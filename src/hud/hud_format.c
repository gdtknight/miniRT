/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_format.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

static void	ft_strcpy(char *dst, const char *src)
{
	int	i;

	i = 0;
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
}

static int	ft_numlen(int n)
{
	int	len;

	len = 0;
	if (n <= 0)
		len = 1;
	while (n)
	{
		n /= 10;
		len++;
	}
	return (len);
}

static void	ft_itoa_buf(char *buf, int n)
{
	int		len;
	long	num;

	num = n;
	if (num < 0)
	{
		buf[0] = '-';
		num = -num;
	}
	len = ft_numlen(n);
	buf[len] = '\0';
	while (len > 0 && num >= 0)
	{
		buf[--len] = (num % 10) + '0';
		num /= 10;
		if (n < 0 && len == 1)
			break ;
	}
}

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
