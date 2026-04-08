/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cone.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/04/08 22:40:08 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scene/parser.h"
#include "common/utils.h"
#include <stdlib.h>
#include "libft.h"

/**
 * @brief Parse cone diameter and height, compute derived fields.
 */
static t_parse_result	parse_cone_dims(const char **token, t_object *obj)
{
	double			diameter;
	double			height;
	t_parse_result	result;

	result = parse_double(*token, &diameter, token);
	if (result != PARSE_OK)
		return (result);
	*token = skip_whitespace(*token);
	result = parse_double(*token, &height, token);
	if (result != PARSE_OK)
		return (result);
	if (diameter <= 0 || height <= 0)
		return (PARSE_ERR_RANGE);
	obj->data.cone.radius = diameter / 2.0;
	obj->data.cone.radius_sq = obj->data.cone.radius
		* obj->data.cone.radius;
	obj->data.cone.half_height = height / 2.0;
	return (PARSE_OK);
}

/**
 * @brief Parse and validate cone center position and axis vector.
 */
static t_parse_result	parse_cone_vectors(const char **token, t_object *obj)
{
	t_parse_result	result;

	result = parse_vector_strict(*token, &obj->data.cone.center, token);
	if (result != PARSE_OK)
		return (result);
	result = validate_coordinate_range(&obj->data.cone.center);
	if (result != PARSE_OK)
		return (result);
	*token = skip_whitespace(*token);
	result = parse_vector_strict(*token, &obj->data.cone.axis, token);
	if (result != PARSE_OK)
		return (result);
	result = validate_vector_range(&obj->data.cone.axis);
	if (result != PARSE_OK)
		return (result);
	result = validate_direction_vector(&obj->data.cone.axis);
	if (result != PARSE_OK)
		return (result);
	obj->data.cone.axis = vec3_normalize(obj->data.cone.axis);
	return (PARSE_OK);
}

/**
 * @brief Parse a cone definition line and add it to the scene.
 *
 * Extracts center, axis, diameter, height, color, and bonus
 * options, then appends the cone object to the scene list.
 *
 * @param line Raw line from the scene file.
 * @param scene Scene to update.
 * @return t_parse_result PARSE_OK or error code.
 */
t_parse_result	parse_cone(char *line, t_scene *scene)
{
	const char		*token;
	t_object		obj;
	t_parse_result	result;

	ft_bzero(&obj, sizeof(t_object));
	obj.type = OBJ_CONE;
	token = skip_whitespace(line + 3);
	result = parse_cone_vectors(&token, &obj);
	if (result != PARSE_OK)
		return (result);
	token = skip_whitespace(token);
	result = parse_cone_dims(&token, &obj);
	if (result != PARSE_OK)
		return (result);
	result = parse_color_strict(skip_whitespace(token), &obj.color, &token);
	if (result != PARSE_OK)
		return (result);
	result = parse_bonus_options(&token, &obj);
	if (result != PARSE_OK)
		return (result);
	format_id(obj.id, 16, "co-", get_type_count(scene, OBJ_CONE) + 1);
	if (object_list_add(&scene->objects, &obj))
		return (PARSE_OK);
	free(obj.bump_path);
	return (PARSE_ERR_OVERFLOW);
}
