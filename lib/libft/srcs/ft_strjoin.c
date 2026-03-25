/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:55:13 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:23:04 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "libft.h"

char	*ft_strjoin(const char *s1, const char *s2)
{
	char	*joined;
	size_t	len;

	len = ft_strlen(s1) + ft_strlen(s2) + 1;
	joined = (char *) malloc(sizeof(char) * len);
	if (!joined)
		return (NULL);
	ft_strlcpy(joined, s1, len);
	ft_strlcat(joined, s2, len);
	return (joined);
}
