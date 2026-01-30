/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_token.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

/**
 * @brief Check if character is whitespace (space or tab).
 *
 * @param c Character to check.
 * @return int 1 if whitespace, 0 otherwise.
 */
static int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t');
}

/**
 * @brief Skip whitespace characters (space, tab).
 *
 * @param str Input string.
 * @return const char* Pointer to first non-whitespace character.
 */
const char	*skip_whitespace(const char *str)
{
	if (!str)
		return (NULL);
	while (is_whitespace(*str))
		str++;
	return (str);
}

/**
 * @brief Get next space-separated token.
 *
 * Skips leading whitespace, then finds the end of the token.
 *
 * @param str Current position.
 * @param end Set to character after token.
 * @return const char* Pointer to token start, or NULL if at end.
 */
const char	*next_token(const char *str, const char **end)
{
	const char	*start;

	if (!str || !end)
		return (NULL);
	str = skip_whitespace(str);
	if (*str == '\0' || *str == '\n' || *str == '#')
		return (NULL);
	start = str;
	while (*str && !is_whitespace(*str) && *str != '\n' && *str != '#')
		str++;
	*end = str;
	return (start);
}

/**
 * @brief Check if at end of meaningful content.
 *
 * Returns 1 if the current position is whitespace, null terminator,
 * newline, or comment character.
 *
 * @param str Current position.
 * @return int 1 if at line end, 0 otherwise.
 */
int	at_line_end(const char *str)
{
	if (!str)
		return (1);
	str = skip_whitespace(str);
	return (*str == '\0' || *str == '\n' || *str == '#');
}
