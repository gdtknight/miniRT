/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:16 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/28 17:26:04 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common/error.h"
#include <stdlib.h>
#include <unistd.h>

void	error_write_str(const char *str)
{
	int	len;

	len = 0;
	while (str[len])
		len++;
	write(2, str, len);
}

const char	*error_get_message(t_error_code code)
{
	static const char	*error_messages[ERR_COUNT] = {
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

	if (code >= 0 && code < ERR_COUNT)
		return (error_messages[code]);
	return ("Unknown error");
}

int	error_print(t_error_code code)
{
	const char	*msg;

	error_write_str("Error\n");
	msg = error_get_message(code);
	if (msg)
	{
		error_write_str(msg);
		error_write_str("\n");
	}
	return (1);
}

void	error_exit(t_error_code code)
{
	error_print(code);
	exit(EXIT_FAILURE);
}

void	error_print_ctx(const char *prefix, const char *detail)
{
	error_write_str("Error\n");
	error_write_str(prefix);
	error_write_str(detail);
	error_write_str("\n");
}
