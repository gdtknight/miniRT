/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_number.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

/**
 * @brief Parse double value with format validation.
 *
 * @param str Input string.
 * @param value Output value (only set on success).
 * @param end Pointer to character after parsed number.
 * @return t_parse_result PARSE_OK or PARSE_ERR_NUMBER_FORMAT.
 */
t_parse_result	parse_double(const char *str, double *value, const char **end)
{
	double	sign;
	double	result;
	int		has_digits;

	if (!str || !value || !end)
		return (PARSE_ERR_NUMBER_FORMAT);
	sign = 1.0;
	has_digits = 0;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1.0;
		str++;
	}
	result = parse_int_part(&str, &has_digits);
	result += parse_frac_part(&str, &has_digits);
	if (!has_digits)
		return (PARSE_ERR_NUMBER_FORMAT);
	if (*str == '.' || *str == 'e' || *str == 'E')
		return (PARSE_ERR_NUMBER_FORMAT);
	*value = result * sign;
	*end = str;
	return (PARSE_OK);
}

/**
 * @brief Parse integer digits from string.
 *
 * @param str String pointer.
 * @param result Output result.
 * @param has_digits Set to 1 if digits found.
 */
static void	parse_int_digits(const char **str, int *result, int *has_digits)
{
	while (parse_is_digit(**str))
	{
		*result = *result * 10 + (**str - '0');
		(*str)++;
		*has_digits = 1;
	}
}

/**
 * @brief Parse integer value with format validation.
 *
 * @param str Input string.
 * @param value Output value (only set on success).
 * @param end Pointer to character after parsed number.
 * @return t_parse_result PARSE_OK or PARSE_ERR_NUMBER_FORMAT.
 */
t_parse_result	parse_int(const char *str, int *value, const char **end)
{
	int		sign;
	int		result;
	int		has_digits;

	if (!str || !value || !end)
		return (PARSE_ERR_NUMBER_FORMAT);
	sign = 1;
	has_digits = 0;
	result = 0;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	parse_int_digits(&str, &result, &has_digits);
	if (!has_digits)
		return (PARSE_ERR_NUMBER_FORMAT);
	*value = result * sign;
	*end = str;
	return (PARSE_OK);
}
