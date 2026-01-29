/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_format_helpers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/29 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"
#include "libft.h"
#include <stdlib.h>

void	ft_strcpy(char *dst, const char *src)
{
	ft_strlcpy(dst, src, ft_strlen(src) + 1);
}

int	ft_numlen(int n)
{
	char	*str;
	int		len;

	str = ft_itoa(n);
	if (!str)
		return (1);
	len = ft_strlen(str);
	free(str);
	return (len);
}

void	ft_itoa_buf(char *buf, int n)
{
	char	*str;

	str = ft_itoa(n);
	if (!str)
	{
		buf[0] = '0';
		buf[1] = '\0';
		return ;
	}
	ft_strlcpy(buf, str, ft_strlen(str) + 1);
	free(str);
}
