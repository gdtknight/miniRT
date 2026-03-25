/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 00:57:56 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:20:59 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stdlib.h>

#include "libft.h"

void	*ft_calloc(size_t count, size_t size)
{
	void	*addr;

	if (count == 0 || size == 0)
		return ((void *) malloc(0));
	if (ULLONG_MAX / count < size)
		return (NULL);
	addr = (void *) malloc(size * count);
	if (addr)
		ft_bzero (addr, count * size);
	return (addr);
}
