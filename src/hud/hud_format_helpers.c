/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_format_helpers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/29 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"
#include "libft.h"
#include <stdlib.h>

/**
 * @brief Copy a string into a destination buffer.
 *
 * Uses ft_strlcpy to ensure the destination is null-terminated.
 *
 * @param dst Destination buffer.
 * @param src Source string.
 */
void	ft_strcpy(char *dst, const char *src)
{
	ft_strlcpy(dst, src, ft_strlen(src) + 1);
}

/**
 * @brief Convert an integer to string and write into a buffer.
 *
 * Writes "0" on allocation failure.
 *
 * @param buf Destination buffer.
 * @param n Integer to format.
 */
void	ft_itoa_buf(char *buf, int n)
{
	char	*str;

	str = ft_itoa(n);
	if (!str)
	{
		buf[0] = '0';
		buf[1] = '\0';
		return ;
	}
	ft_strlcpy(buf, str, ft_strlen(str) + 1);
	free(str);
}
