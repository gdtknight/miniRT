/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_number_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

/**
 * @brief Check if character is a digit.
 *
 * @param c Character to check.
 * @return int 1 if digit, 0 otherwise.
 */
int	parse_is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

/**
 * @brief Parse the integer part of a number.
 *
 * @param str Pointer to string pointer (updated).
 * @param has_digits Set to 1 if any digits were parsed.
 * @return double Parsed integer value.
 */
double	parse_int_part(const char **str, int *has_digits)
{
	double	result;

	result = 0.0;
	while (parse_is_digit(**str))
	{
		result = result * 10.0 + (**str - '0');
		(*str)++;
		*has_digits = 1;
	}
	return (result);
}

/**
 * @brief Parse the fractional part of a number.
 *
 * @param str Pointer to string pointer (updated).
 * @param has_digits Set to 1 if any digits were parsed.
 * @return double Parsed fractional value.
 */
double	parse_frac_part(const char **str, int *has_digits)
{
	double	result;
	double	divisor;

	result = 0.0;
	divisor = 10.0;
	if (**str == '.')
	{
		(*str)++;
		while (parse_is_digit(**str))
		{
			result += (**str - '0') / divisor;
			divisor *= 10.0;
			(*str)++;
			*has_digits = 1;
		}
	}
	return (result);
}
