/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 15:23:57 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:21:31 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	ft_carry(char buf[], int *idx)
{
	int	cur;

	cur = MAX_LEN - 2;
	while (buf[cur] && (buf[cur] - '0') + 1 == 10)
		buf[cur--] = '0';
	if (cur == *idx)
		buf[(*idx)--] = '1';
	else
		buf[cur] += 1;
	buf[(*idx)--] = '-';
}

char	*ft_itoa(int n)
{
	int		idx;
	int		flag;
	char	buf[MAX_LEN];

	idx = -1;
	while (++idx < MAX_LEN)
		buf[idx] = '0';
	idx = MAX_LEN - 1;
	buf[idx--] = '\0';
	flag = 0;
	if (n == 0)
		buf[idx--] = '0';
	if (n < 0)
	{
		flag = 1;
		n = (n + 1) * (-1);
	}
	while (n > 0)
	{
		buf[idx--] = (n % 10) + '0';
		n /= 10;
	}
	if (flag)
		ft_carry(buf, &idx);
	return (ft_strdup(&buf[++idx]));
}
