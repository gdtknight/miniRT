/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_validation.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:39 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:39 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "vec3.h"

/*
** Check if value is within specified range [min, max] inclusive.
** Used for parameter validation during parsing.
*/
int	in_range(double value, double min, double max)
{
	return (value >= min && value <= max);
}

/*
** Parse 3D vector from comma-separated string.
** Format: "x,y,z" where x, y, z are floating point numbers.
** Returns 1 on success, 0 on parsing error.
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

/*
** Parse RGB color from comma-separated string.
** Format: "R,G,B" where R, G, B are integers in range [0, 255].
** Returns 1 on success, 0 on parsing or validation error.
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
