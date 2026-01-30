/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_validation.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:39 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "vec3.h"
#include <math.h>

/**
 * @brief Check whether a value lies within an inclusive range.
 *
 * @param value Value to check.
 * @param min Minimum allowed value.
 * @param max Maximum allowed value.
 * @return int 1 if within range, 0 otherwise.
 */
int	in_range(double value, double min, double max)
{
	return (value >= min && value <= max);
}

/**
 * @brief Parse a 3D vector from "x,y,z" format (legacy).
 *
 * Uses ft_atof for each component and expects commas as separators.
 *
 * @param str Input string containing vector components.
 * @param vec Output vector to fill.
 * @return int 1 on success, 0 on failure.
 */
int	parse_vector(char *str, t_vec3 *vec)
{
	char	*token;

	token = str;
	vec->x = ft_atof(token);
	while (*token && *token != ',')
		token++;
	if (*token != ',')
		return (0);
	token++;
	vec->y = ft_atof(token);
	while (*token && *token != ',')
		token++;
	if (*token != ',')
		return (0);
	token++;
	vec->z = ft_atof(token);
	return (1);
}

/**
 * @brief Parse an RGB color from "R,G,B" format (legacy).
 *
 * Validates each component in the 0–255 range.
 *
 * @param str Input string containing color components.
 * @param color Output color to fill.
 * @return int 1 on success, 0 on failure.
 */
int	parse_color(char *str, t_color *color)
{
	char	*token;
	int		r;
	int		g;
	int		b;

	token = str;
	r = ft_atoi(token);
	while (*token && *token != ',')
		token++;
	if (*token != ',')
		return (0);
	token++;
	g = ft_atoi(token);
	while (*token && *token != ',')
		token++;
	if (*token != ',')
		return (0);
	token++;
	b = ft_atoi(token);
	if (!in_range(r, 0, 255) || !in_range(g, 0, 255) || !in_range(b, 0, 255))
		return (print_error("RGB values must be in range [0, 255]"));
	color->r = r;
	color->g = g;
	color->b = b;
	return (1);
}
