/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 21:19:28 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:23:59 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char	*unsigned_dest;
	unsigned char	*unsigned_src;

	if (!dest && !src)
		return (NULL);
	unsigned_dest = (unsigned char *)dest;
	unsigned_src = (unsigned char *)src;
	while (n--)
		*unsigned_dest++ = *unsigned_src++;
	return (dest);
}
