/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/15 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int	print_error(const char *message)
{
	write(2, "Error\n", 6);
	if (message)
	{
		write(2, message, strlen(message));
		write(2, "\n", 1);
	}
	return (0);
}

void	error_exit(const char *message)
{
	print_error(message);
	exit(EXIT_FAILURE);
}
