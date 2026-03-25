/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 15:01:44 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:22:53 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	unsigned int	srclen;

	srclen = ft_strlen(src);
	if (srclen + 1 < size)
	{
		while (*src)
			*dst++ = *src++;
		*dst = '\0';
		return (srclen);
	}
	if (size != 0)
	{
		while (size-- > 1)
			*dst++ = *src++;
		*dst = '\0';
	}
	return (srclen);
}
