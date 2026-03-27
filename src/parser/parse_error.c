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
