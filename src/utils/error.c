/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:16 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static const char	*g_error_messages[ERR_COUNT] = {
	"No error",
	"Memory allocation failed",
	"Failed to open file",
	"Failed to read file",
	"Invalid file extension (expected .rt)",
	"Invalid format in scene file",
	"Invalid ambient light configuration",
	"Invalid camera configuration",
	"Invalid light configuration",
	"Invalid sphere configuration",
	"Invalid plane configuration",
	"Invalid cylinder configuration",
	"Duplicate element in scene file",
	"Missing required element in scene file",
	"Failed to initialize window",
	"Failed to initialize image",
	"Failed to build BVH",
	"Invalid parameter"
};

const char	*error_get_message(t_error_code code)
{
	if (code >= 0 && code < ERR_COUNT)
		return (g_error_messages[code]);
	return ("Unknown error");
}

int	error_print(t_error_code code)
{
	const char	*msg;

	write(2, "Error\n", 6);
	msg = error_get_message(code);
	if (msg)
	{
		write(2, msg, strlen(msg));
		write(2, "\n", 1);
	}
	return (1);
}

void	error_exit(t_error_code code)
{
	error_print(code);
	exit(EXIT_FAILURE);
}

/* Legacy function for backward compatibility */
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
