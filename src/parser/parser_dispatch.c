/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_dispatch.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"

/**
 * @brief Check if line matches element prefix.
 *
 * @param line Line to check.
 * @param prefix Prefix to match.
 * @param len Length of prefix.
 * @return int 1 if matches, 0 otherwise.
 */
static int	matches_prefix(char *line, const char *prefix, int len)
{
	int	i;

	i = 0;
	while (i < len)
	{
		if (line[i] != prefix[i])
			return (0);
		i++;
	}
	return (line[len] == ' ' || line[len] == '\t');
}

/**
 * @brief Dispatch objects (sp, pl, cy).
 *
 * @param line Line content.
 * @param scene Scene to update.
 * @param ctx Error context.
 * @return t_parse_result Result code.
 */
static t_parse_result	dispatch_objects(char *line, t_scene *scene,
	t_error_context *ctx)
{
	if (matches_prefix(line, "sp", 2))
	{
		ctx->element_type = "Sphere";
		return (parse_sphere(line, scene));
	}
	if (matches_prefix(line, "pl", 2))
	{
		ctx->element_type = "Plane";
		return (parse_plane(line, scene));
	}
	if (matches_prefix(line, "cy", 2))
	{
		ctx->element_type = "Cylinder";
		return (parse_cylinder(line, scene));
	}
	return (PARSE_ERR_UNKNOWN_ELEMENT);
}

/**
 * @brief Dispatch elements (A, C, L).
 *
 * @param line Line content.
 * @param scene Scene to update.
 * @param ctx Error context.
 * @return t_parse_result Result code or -1 for objects.
 */
static int	dispatch_elements(char *line, t_scene *scene, t_error_context *ctx)
{
	if (matches_prefix(line, "A", 1))
	{
		ctx->element_type = "Ambient";
		return (parse_ambient(line, scene));
	}
	if (matches_prefix(line, "C", 1))
	{
		ctx->element_type = "Camera";
		return (parse_camera(line, scene));
	}
	if (matches_prefix(line, "L", 1))
	{
		ctx->element_type = "Light";
		return (parse_light(line, scene));
	}
	return (-1);
}

/**
 * @brief Parse element type prefix and dispatch to parser.
 *
 * @param line Line content.
 * @param scene Scene to update.
 * @param ctx Error context for reporting.
 * @return t_parse_result PARSE_OK or error code.
 */
t_parse_result	dispatch_element(char *line, t_scene *scene,
	t_error_context *ctx)
{
	int	result;

	line = (char *)skip_whitespace(line);
	if (*line == '\0' || *line == '\n' || *line == '#')
		return (PARSE_OK);
	result = dispatch_elements(line, scene, ctx);
	if (result >= 0)
		return ((t_parse_result)result);
	return (dispatch_objects(line, scene, ctx));
}
