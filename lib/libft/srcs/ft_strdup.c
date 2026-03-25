/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 13:38:23 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:23:16 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "libft.h"

char	*ft_strdup(const char *s)
{
	char	*d;
	char	*dst;
	int		srclen;

	if (!s)
		return (NULL);
	srclen = ft_strlen(s);
	d = (char *) malloc(sizeof(char) * (srclen + 1));
	if (!d)
		return (NULL);
	dst = d;
	while (*s)
		*d++ = *s++;
	*d = '\0';
	return (dst);
}
