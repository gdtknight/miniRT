/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 21:28:59 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:23:07 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_striteri(char *s, void (*f)(unsigned int, char*))
{
	size_t	idx;
	size_t	len;

	if (!s)
		return ;
	idx = 0;
	len = ft_strlen(s);
	while (idx < len)
	{
		f(idx, &s[idx]);
		idx++;
	}
}
