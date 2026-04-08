/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scene/parser.h"
#include "common/error.h"
#include <unistd.h>

/**
 * @brief Recursively write unsigned integer digits to stderr.
 *
 * @param n Unsigned integer value to write.
 */
static void	write_uint(unsigned int n)
{
	char	c;

	if (n >= 10)
		write_uint(n / 10);
	c = '0' + (n % 10);
	write(2, &c, 1);
}

/**
 * @brief Write a signed integer to stderr.
 *
 * @param n Integer value to write.
 */
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
 * @brief Look up human-readable message for a parse error code.
 *
 * @param code Parse result code.
 * @return Pointer to static error message string.
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

/**
 * @brief Initialize error context with default values.
 *
 * @param ctx Error context to initialize.
 */
void	error_context_init(t_error_context *ctx)
{
	ctx->line_num = 0;
	ctx->element_type = NULL;
	ctx->error_code = PARSE_OK;
}

/**
 * @brief Print error message with context to stderr.
 *
 * @param ctx Error context containing line, element, and error code.
 */
void	error_context_print(t_error_context *ctx)
{
	if (!ctx || ctx->error_code == PARSE_OK)
		return ;
	error_write_str("Error\nLine ");
	error_write_int(ctx->line_num);
	if (ctx->element_type != NULL)
	{
		error_write_str(" (");
		error_write_str(ctx->element_type);
		error_write_str(")");
	}
	error_write_str(": ");
	error_write_str(get_error_message(ctx->error_code));
	error_write_str("\n");
}
