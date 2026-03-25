/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 16:25:46 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:22:33 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	little_len;
	size_t	idx;

	little_len = ft_strlen(little);
	if (little_len == 0)
		return ((char *)big);
	if (ft_strlen(big) == 0 || len < little_len)
		return (NULL);
	idx = 0;
	while (idx < len)
	{
		if (len - idx < little_len)
			return (NULL);
		if (ft_strncmp(&big[idx], little, little_len) == 0)
			return ((char *)(&big[idx]));
		idx++;
	}
	return (NULL);
}
