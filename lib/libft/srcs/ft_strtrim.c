/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 12:52:54 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:22:25 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	is_set(const char c, const char *set)
{
	while (*set)
		if (*set++ == c)
			return (1);
	return (0);
}

char	*ft_strtrim(const char *s1, const char *set)
{
	size_t	sidx;
	size_t	eidx;
	size_t	slen;

	if (!s1)
		return (NULL);
	slen = ft_strlen(s1);
	if (slen == 0)
		return (ft_strdup(""));
	sidx = 0;
	eidx = slen - 1;
	while (is_set(s1[sidx], set))
		sidx++;
	while (sidx < eidx && is_set(s1[eidx], set))
		eidx--;
	return (ft_substr(s1, sidx, ++eidx - sidx));
}
