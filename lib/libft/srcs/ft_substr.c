/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 09:15:47 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:22:21 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "libft.h"

char	*ft_substr(const char *s, unsigned int start, size_t len)
{
	char	*new_str;
	size_t	idx;
	size_t	slen;

	if (!s)
		return (NULL);
	slen = ft_strlen(s);
	if (slen == 0 || len == 0 || slen <= start)
		return (ft_strdup(""));
	if (slen - start + 1 <= len)
		len = slen - start;
	new_str = (char *) malloc(sizeof(char) * (len + 1));
	if (!new_str)
		return (NULL);
	idx = 0;
	while (idx < len)
	{
		new_str[idx] = s[idx + start];
		idx++;
	}
	new_str[idx] = '\0';
	return (new_str);
}
