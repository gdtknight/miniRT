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
#include <limits.h>
#include <math.h>

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
	double	frac;
	int		has_digits;

	if (!str || !value || !end)
		return (PARSE_ERR_NUMBER_FORMAT);
	sign = 1.0;
	has_digits = 0;
	if (*str == '-')
		sign = -1.0;
	if (*str == '-' || *str == '+')
		str++;
	result = parse_int_part(&str, &has_digits);
	if (!has_digits && *str == '.')
		return (PARSE_ERR_NUMBER_FORMAT);
	frac = parse_frac_part(&str, &has_digits);
	result += frac;
	if (!has_digits || frac < 0.0 || isinf(result))
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
static int	parse_int_digits(const char **str, int *result, int *has_digits)
{
	int	digit;

	while (parse_is_digit(**str))
	{
		digit = **str - '0';
		if (*result > (INT_MAX - digit) / 10)
			return (0);
		*result = *result * 10 + digit;
		(*str)++;
		*has_digits = 1;
	}
	return (1);
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
	if (!parse_int_digits(&str, &result, &has_digits))
		return (PARSE_ERR_NUMBER_FORMAT);
	if (!has_digits)
		return (PARSE_ERR_NUMBER_FORMAT);
	*value = result * sign;
	*end = str;
	return (PARSE_OK);
}
