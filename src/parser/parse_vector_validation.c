/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_vector_validation.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/09 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"

/**
 * @brief Validate vector components are in range [-1,1].
 *
 * Subject requires normalized vectors to have each component
 * in the range [-1,1] for direction/normal/axis vectors.
 *
 * @param vec Vector to validate.
 * @return t_parse_result PARSE_OK or PARSE_ERR_RANGE.
 */
t_parse_result	validate_vector_range(const t_vec3 *vec)
{
	if (!vec)
		return (PARSE_ERR_FORMAT);
	if (!in_range(vec->x, -1.0, 1.0))
		return (PARSE_ERR_RANGE);
	if (!in_range(vec->y, -1.0, 1.0))
		return (PARSE_ERR_RANGE);
	if (!in_range(vec->z, -1.0, 1.0))
		return (PARSE_ERR_RANGE);
	return (PARSE_OK);
}

/**
 * @brief Validate coordinate components are within bounds.
 *
 * Rejects positions with components beyond COORD_MAX to prevent
 * overflow in BVH construction and rendering.
 *
 * @param vec Position vector to validate.
 * @return t_parse_result PARSE_OK or PARSE_ERR_RANGE.
 */
t_parse_result	validate_coordinate_range(const t_vec3 *vec)
{
	if (!vec)
		return (PARSE_ERR_FORMAT);
	if (!in_range(vec->x, -COORD_MAX, COORD_MAX))
		return (PARSE_ERR_RANGE);
	if (!in_range(vec->y, -COORD_MAX, COORD_MAX))
		return (PARSE_ERR_RANGE);
	if (!in_range(vec->z, -COORD_MAX, COORD_MAX))
		return (PARSE_ERR_RANGE);
	return (PARSE_OK);
}
