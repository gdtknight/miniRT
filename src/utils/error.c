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

#include "error.h"
#include "libft.h"
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief Return a human-readable error message for a code.
 *
 * @param code Error code to translate.
 * @return const char* Error message string.
 */
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

/**
 * @brief Print an error message for a code to stderr.
 *
 * @param code Error code to print.
 * @return int Always returns 1 for convenience.
 */
int	error_print(t_error_code code)
{
	const char	*msg;

	write(2, "Error\n", 6);
	msg = error_get_message(code);
	if (msg)
	{
		write(2, msg, ft_strlen(msg));
		write(2, "\n", 1);
	}
	return (1);
}

/**
 * @brief Print an error message and exit the program.
 *
 * @param code Error code to print.
 */
void	error_exit(t_error_code code)
{
	error_print(code);
	exit(EXIT_FAILURE);
}

/**
 * @brief Legacy error print function with custom message.
 *
 * Prints "Error" header and the provided message to stderr.
 *
 * @param message Error message string.
 * @return int Always returns 0 for convenience.
 */
int	print_error(const char *message)
{
	write(2, "Error\n", 6);
	if (message)
	{
		write(2, message, ft_strlen(message));
		write(2, "\n", 1);
	}
	return (0);
}
