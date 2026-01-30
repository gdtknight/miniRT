/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   format_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/29 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "libft.h"
#include <stdlib.h>

/**
 * @brief Format an ID by concatenating a prefix and integer.
 *
 * Writes into the provided buffer using bounded libft functions.
 *
 * @param buf Destination buffer.
 * @param size Destination buffer size.
 * @param prefix Prefix string.
 * @param n Numeric suffix.
 * @return int 1 on success, 0 on allocation failure.
 */
int	format_id(char *buf, size_t size, const char *prefix, int n)
{
	char	*num;

	num = ft_itoa(n);
	if (!num)
		return (0);
	ft_strlcpy(buf, prefix, size);
	ft_strlcat(buf, num, size);
	free(num);
	return (1);
}

/**
 * @brief Append fractional part to an existing numeric string.
 *
 * @param buf Destination buffer containing integer part.
 * @param size Destination buffer size.
 * @param frac Fractional digits as integer.
 * @param precision Number of fractional digits (1 or 2).
 */
static void	write_frac_part(char *buf, size_t size, int frac, int precision)
{
	char	*frac_str;
	size_t	len;
	int		i;

	len = ft_strlen(buf);
	if (len + 1 >= size)
		return ;
	buf[len++] = '.';
	buf[len] = '\0';
	i = precision - 1;
	while (i > 0 && frac < 10)
	{
		if (len + 1 >= size)
			return ;
		buf[len++] = '0';
		buf[len] = '\0';
		i--;
	}
	frac_str = ft_itoa(frac);
	if (frac_str)
	{
		ft_strlcat(buf, frac_str, size);
		free(frac_str);
	}
}

/**
 * @brief Handle negative values by writing '-' and flipping sign.
 *
 * @param buf Destination buffer.
 * @param size Destination buffer size.
 * @param value Value to potentially negate (in/out).
 * @return int 1 on success, 0 if buffer is too small.
 */
static int	handle_negative(char *buf, size_t size, double *value)
{
	if (*value < 0)
	{
		if (size < 2)
			return (0);
		buf[0] = '-';
		buf[1] = '\0';
		*value = -(*value);
	}
	return (1);
}

/**
 * @brief Format a floating-point number into a buffer.
 *
 * Supports 1 or 2 decimal digits with rounding.
 *
 * @param buf Destination buffer.
 * @param size Destination buffer size.
 * @param value Value to format.
 * @param precision Number of decimal digits (1 or 2).
 * @return int 1 on success, 0 on failure.
 */
int	float_to_str(char *buf, size_t size, double value, int precision)
{
	int		int_part;
	int		frac_part;
	int		mult;
	char	*int_str;

	if (size == 0)
		return (0);
	buf[0] = '\0';
	if (!handle_negative(buf, size, &value))
		return (0);
	mult = 10;
	if (precision == 2)
		mult = 100;
	int_part = (int)value;
	frac_part = (int)((value - int_part) * mult + 0.5);
	int_str = ft_itoa(int_part);
	if (!int_str)
		return (0);
	ft_strlcat(buf, int_str, size);
	free(int_str);
	write_frac_part(buf, size, frac_part, precision);
	return (1);
}
