/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_error_msg.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include <unistd.h>

/**
 * @brief Write a string to stderr.
 *
 * @param str String to write.
 */
void	error_write_str(const char *str)
{
	while (*str)
	{
		write(2, str, 1);
		str++;
	}
}

/**
 * @brief Write an integer to stderr.
 *
 * @param n Integer to write.
 */
void	error_write_int(int n)
{
	char	c;

	if (n >= 10)
		error_write_int(n / 10);
	c = '0' + (n % 10);
	write(2, &c, 1);
}

/**
 * @brief Get error message for a parse result code.
 *
 * @param code Parse result code.
 * @return const char* Error message string.
 */
const char	*get_error_message(t_parse_result code)
{
	static const char	*msgs[] = {
		"",
		"Invalid format",
		"Value out of range",
		"Vector requires exactly 3 components",
		"Duplicate declaration",
		"Line too long (max 4096 characters)",
		"Direction vector cannot be zero",
		"Unexpected token at end of line",
		"Unknown element identifier",
		"Invalid number format",
		"Missing required element"
	};

	if (code < PARSE_ERR_COUNT)
		return (msgs[code]);
	return ("Unknown error");
}
