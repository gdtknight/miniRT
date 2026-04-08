/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_bonus_options.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/04/08 22:39:58 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scene/parser.h"
#include <stdlib.h>
#include "libft.h"

/**
 * @brief Check if a string starts with the given prefix.
 */
static int	starts_with(const char *str, const char *prefix)
{
	while (*prefix)
	{
		if (*str != *prefix)
			return (0);
		str++;
		prefix++;
	}
	return (1);
}

/**
 * @brief Parse checker bonus option and apply to object.
 */
static t_parse_result	parse_checker_opt(const char *str, t_object *obj,
		const char **end)
{
	t_parse_result	result;

	result = parse_color_strict(str + sizeof("checker:") - 1,
			&obj->checker_color, end);
	if (result != PARSE_OK)
		return (result);
	obj->has_checker = 1;
	return (PARSE_OK);
}

/**
 * @brief Parse bump map path option and apply to object.
 */
static t_parse_result	parse_bump_opt(const char *str, t_object *obj,
		const char **end)
{
	const char	*start;
	int			len;

	start = str + sizeof("bump:") - 1;
	len = 0;
	while (start[len] && start[len] != ' '
		&& start[len] != '\t' && start[len] != '\n')
		len++;
	if (len == 0)
		return (PARSE_ERR_FORMAT);
	free(obj->bump_path);
	obj->bump_path = ft_substr(start, 0, len);
	if (!obj->bump_path)
		return (PARSE_ERR_OVERFLOW);
	*end = start + len;
	return (PARSE_OK);
}

/**
 * @brief Parse optional bonus tokens (checker, bump) after color.
 *
 * Consumes remaining tokens on the line. Resets bonus fields
 * before parsing and cleans up on failure.
 *
 * @param token Current parse position (updated on return).
 * @param obj Object to receive bonus options.
 * @return t_parse_result PARSE_OK or error code.
 */
t_parse_result	parse_bonus_options(const char **token, t_object *obj)
{
	t_parse_result	result;

	obj->has_checker = 0;
	obj->bump_path = NULL;
	obj->bump_map = NULL;
	*token = skip_whitespace(*token);
	while (!at_line_end(*token))
	{
		result = PARSE_ERR_TRAILING_TOKEN;
		if (starts_with(*token, "checker:") && !obj->has_checker)
			result = parse_checker_opt(*token, obj, token);
		else if (starts_with(*token, "bump:") && !obj->bump_path)
			result = parse_bump_opt(*token, obj, token);
		if (result != PARSE_OK)
		{
			free(obj->bump_path);
			obj->bump_path = NULL;
			return (result);
		}
		*token = skip_whitespace(*token);
	}
	return (PARSE_OK);
}
