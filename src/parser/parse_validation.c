/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_validation.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/15 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "vec3.h"
#include <stdlib.h>
#include <math.h>

int	in_range(double value, double min, double max)
{
	return (value >= min && value <= max);
}

int	parse_vector(char *str, t_vec3 *vec)
{
	char	*token;

	token = str;
	vec->x = atof(token);
	while (*token && *token != ',')
		token++;
	if (*token != ',')
		return (0);
	token++;
	vec->y = atof(token);
	while (*token && *token != ',')
		token++;
	if (*token != ',')
		return (0);
	token++;
	vec->z = atof(token);
	return (1);
}

int	parse_color(char *str, t_color *color)
{
	char	*token;
	int		r;
	int		g;
	int		b;

	token = str;
	r = atoi(token);
	while (*token && *token != ',')
		token++;
	if (*token != ',')
		return (0);
	token++;
	g = atoi(token);
	while (*token && *token != ',')
		token++;
	if (*token != ',')
		return (0);
	token++;
	b = atoi(token);
	if (!in_range(r, 0, 255) || !in_range(g, 0, 255) || !in_range(b, 0, 255))
		return (print_error("RGB values must be in range [0, 255]"));
	color->r = r;
	color->g = g;
	color->b = b;
	return (1);
}
