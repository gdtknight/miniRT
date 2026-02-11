/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_bonus_options.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/10 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include <stdlib.h>

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

static t_parse_result	parse_checker_opt(const char *str, t_object *obj,
		const char **end)
{
	t_parse_result	result;

	result = parse_color_strict(str + 8, &obj->checker_color, end);
	if (result != PARSE_OK)
		return (result);
	obj->has_checker = 1;
	return (PARSE_OK);
}

static t_parse_result	parse_bump_opt(const char *str, t_object *obj,
		const char **end)
{
	const char	*start;
	int			len;

	start = str + 5;
	len = 0;
	while (start[len] && start[len] != ' '
		&& start[len] != '\t' && start[len] != '\n')
		len++;
	if (len == 0)
		return (PARSE_ERR_FORMAT);
	obj->bump_path = ft_substr(start, 0, len);
	if (!obj->bump_path)
		return (PARSE_ERR_FORMAT);
	*end = start + len;
	return (PARSE_OK);
}

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
		if (starts_with(*token, "checker:"))
			result = parse_checker_opt(*token, obj, token);
		else if (starts_with(*token, "bump:"))
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
