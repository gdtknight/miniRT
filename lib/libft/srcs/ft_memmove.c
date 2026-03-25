/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 21:20:48 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:23:55 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char	*unsigned_dest;
	unsigned char	*unsigned_src;

	if (!dest && !src)
		return (NULL);
	unsigned_dest = (unsigned char *)dest;
	unsigned_src = (unsigned char *)src;
	if (unsigned_src < unsigned_dest)
		while (n--)
			*(unsigned_dest + n) = *(unsigned_src + n);
	else
		while (n--)
			*unsigned_dest++ = *unsigned_src++;
	return (dest);
}
