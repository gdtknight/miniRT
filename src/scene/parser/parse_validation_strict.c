/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_validation_strict.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scene/parser.h"
#include "common/ray.h"
#include "common/error.h"

int	in_range(double value, double min, double max)
{
	return (value >= min && value <= max);
}

/**
 * @brief Parse vector with strict validation.
 *
 * Format: "x,y,z" (no spaces around commas, exactly 3 components).
 *
 * @param str Input string.
 * @param vec Output vector.
 * @param end Pointer to character after parsed vector.
 * @return t_parse_result PARSE_OK on success, error code on failure.
 */
t_parse_result	parse_vector_strict(const char *str, t_vec3 *vec,
	const char **end)
{
	t_parse_result	result;

	if (!str || !vec || !end)
		return (PARSE_ERR_FORMAT);
	result = parse_double(str, &vec->x, &str);
	if (result != PARSE_OK)
		return (result);
	if (*str != ',')
		return (PARSE_ERR_COMPONENT_COUNT);
	str++;
	result = parse_double(str, &vec->y, &str);
	if (result != PARSE_OK)
		return (result);
	if (*str != ',')
		return (PARSE_ERR_COMPONENT_COUNT);
	str++;
	result = parse_double(str, &vec->z, &str);
	if (result != PARSE_OK)
		return (result);
	*end = str;
	return (PARSE_OK);
}

/**
 * @brief Parse color component values.
 *
 * @param str Input string.
 * @param rgb Output RGB array.
 * @param end_ptr End pointer.
 * @return t_parse_result PARSE_OK or error code.
 */
static t_parse_result	parse_color_values(const char *str, int *rgb,
	const char **end_ptr)
{
	t_parse_result	result;
	int				i;

	i = 0;
	while (i < 3)
	{
		result = parse_int(str, &rgb[i], &str);
		if (result != PARSE_OK)
			return (result);
		if (!in_range(rgb[i], 0, 255))
			return (PARSE_ERR_RANGE);
		if (i < 2 && *str != ',')
			return (PARSE_ERR_COMPONENT_COUNT);
		if (i < 2)
			str++;
		i++;
	}
	*end_ptr = str;
	return (PARSE_OK);
}

/**
 * @brief Parse color with strict validation.
 *
 * Format: "r,g,b" (no spaces, exactly 3 components, each in [0,255]).
 *
 * @param str Input string.
 * @param color Output color.
 * @param end Pointer to character after parsed color.
 * @return t_parse_result PARSE_OK on success, error code on failure.
 */
t_parse_result	parse_color_strict(const char *str, t_color *color,
	const char **end)
{
	t_parse_result	result;
	int				rgb[3];

	if (!str || !color || !end)
		return (PARSE_ERR_FORMAT);
	result = parse_color_values(str, rgb, end);
	if (result != PARSE_OK)
		return (result);
	color->r = rgb[0];
	color->g = rgb[1];
	color->b = rgb[2];
	return (PARSE_OK);
}

/**
 * @brief Validate direction/normal vector is not zero.
 *
 * @param vec Vector to validate.
 * @return t_parse_result PARSE_OK or PARSE_ERR_ZERO_VECTOR.
 */
t_parse_result	validate_direction_vector(t_vec3 *vec)
{
	double	len_sq;

	if (!vec)
		return (PARSE_ERR_FORMAT);
	len_sq = vec->x * vec->x + vec->y * vec->y + vec->z * vec->z;
	if (len_sq < EPSILON * EPSILON)
		return (PARSE_ERR_ZERO_VECTOR);
	return (PARSE_OK);
}
