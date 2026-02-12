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
	int	len;

	len = 0;
	while (str[len])
		len++;
	write(2, str, len);
}

/**
 * @brief Write an integer to stderr.
 *
 * @param n Integer to write.
 */
static void	write_uint(unsigned int n)
{
	char	c;

	if (n >= 10)
		write_uint(n / 10);
	c = '0' + (n % 10);
	write(2, &c, 1);
}

void	error_write_int(int n)
{
	unsigned int	u;

	if (n < 0)
	{
		write(2, "-", 1);
		u = -(unsigned int)n;
	}
	else
		u = (unsigned int)n;
	write_uint(u);
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
		"Missing required element",
		"I/O error while reading file",
		"Maximum element count exceeded"
	};

	if (code >= 0 && code < PARSE_ERR_COUNT)
		return (msgs[code]);
	return ("Unknown error");
}
