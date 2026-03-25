/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 15:03:11 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:22:57 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	char	*d;
	int		siz;
	int		srclen;
	int		destlen;

	d = dst;
	siz = size;
	srclen = ft_strlen(src);
	destlen = ft_strlen(dst);
	while (siz-- && *dst)
		dst++;
	if (siz == -1)
		return (size + srclen);
	siz = size - (dst - d) - 1;
	while (siz-- && *src)
		*dst++ = *src++;
	*dst = '\0';
	return (destlen + srclen);
}
