/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 15:24:26 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:22:40 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strnchr(char *str, char c, int len)
{
	int	idx;

	if (!str)
		return (-1);
	idx = -1;
	while (++idx < len)
		if (str[idx] == c)
			return (idx);
	return (-1);
}
