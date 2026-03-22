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

#include "parser.h"
#include "error.h"

/**
 * @brief Initialize error context with default values.
 *
 * @param ctx Error context to initialize.
 */
void	error_context_init(t_error_context *ctx)
{
	if (!ctx)
		return ;
	ctx->line_num = 0;
	ctx->element_type = NULL;
	ctx->error_code = PARSE_OK;
}

/**
 * @brief Set the line number in error context.
 *
 * @param ctx Error context to update.
 * @param line Line number (1-based).
 */
void	error_context_set_line(t_error_context *ctx, int line)
{
	if (!ctx)
		return ;
	ctx->line_num = line;
}

/**
 * @brief Set the element type in error context.
 *
 * @param ctx Error context to update.
 * @param type Element type string or NULL.
 */
void	error_context_set_element(t_error_context *ctx, const char *type)
{
	if (!ctx)
		return ;
	ctx->element_type = type;
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
