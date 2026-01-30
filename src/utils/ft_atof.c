/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:20 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:20:21 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Check if character is whitespace (space, tab, newline, etc).
*/
/**
 * @brief Check whether a character is whitespace.
 *
 * @param c Character to test.
 * @return int 1 if whitespace, 0 otherwise.
 */
static int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\r' || c == '\v' || c == '\f');
}

/*
** Check if character is a digit (0-9).
*/
/**
 * @brief Check whether a character is a decimal digit.
 *
 * @param c Character to test.
 * @return int 1 if digit, 0 otherwise.
 */
static int	ft_isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

/*
** Parse integer part of floating point number.
** Updates string pointer and returns integer value.
*/
/**
 * @brief Parse the integer part of a floating-point string.
 *
 * Advances the input pointer past digits and returns the integer value.
 *
 * @param str Input string pointer (in/out).
 * @return double Parsed integer part.
 */
static double	parse_integer_part(const char **str)
{
	double	result;

	result = 0.0;
	while (ft_isdigit(**str))
	{
		result = result * 10.0 + (**str - '0');
		(*str)++;
	}
	return (result);
}

/*
** Parse fractional part of floating point number.
** Handles digits after decimal point.
*/
/**
 * @brief Parse the fractional part of a floating-point string.
 *
 * Advances the input pointer past fractional digits and returns the fraction.
 *
 * @param str Input string pointer (in/out).
 * @return double Parsed fractional part.
 */
static double	parse_fractional_part(const char **str)
{
	double	result;
	double	divisor;

	result = 0.0;
	divisor = 10.0;
	if (**str == '.')
	{
		(*str)++;
		while (ft_isdigit(**str))
		{
			result += (**str - '0') / divisor;
			divisor *= 10.0;
			(*str)++;
		}
	}
	return (result);
}

/*
** Convert string to double precision floating point number.
** Format: [whitespace][+/-][digits][.digits]
** Stops at first invalid character.
*/
/**
 * @brief Convert a string to double.
 *
 * Parses optional whitespace, sign, integer part, and fractional part.
 *
 * @param str Input string.
 * @return double Parsed value.
 */
double	ft_atof(const char *str)
{
	double	result;
	double	sign;

	sign = 1.0;
	while (ft_isspace(*str))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1.0;
		str++;
	}
	result = parse_integer_part(&str);
	result += parse_fractional_part(&str);
	return (result * sign);
}
