/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 16:08:28 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:22:29 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	size_t	offset;

	offset = ft_strlen(s);
	while (offset)
		if (*(s + offset--) == (char)c)
			return ((char *)(s + ++offset));
	if (*s == (char)c)
		return ((char *)s);
	return (NULL);
}
