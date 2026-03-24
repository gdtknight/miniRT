/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cylinder.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "vec3.h"
#include "utils.h"
#include <stdlib.h>

/**
 * @brief Parse cylinder diameter and height from tokens.
 *
 * Validates positive dimensions and updates radius, radius_sq, and half_height.
 *
 * @param token Pointer to the dimension tokens.
 * @param obj Cylinder object to update.
 * @return t_parse_result PARSE_OK on success, error code on failure.
 */
static t_parse_result	parse_cylinder_dims(const char **token, t_object *obj)
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
	obj->data.cylinder.radius = diameter / 2.0;
	obj->data.cylinder.radius_sq = obj->data.cylinder.radius
		* obj->data.cylinder.radius;
	obj->data.cylinder.half_height = height / 2.0;
	return (PARSE_OK);
}

/**
 * @brief Parse cylinder center and axis vectors.
 *
 * @param token Token pointer.
 * @param obj Object to populate.
 * @return t_parse_result PARSE_OK on success, error code on failure.
 */
static t_parse_result	parse_cyl_vectors(const char **token, t_object *obj)
{
	t_parse_result	result;

	result = parse_vector_strict(*token, &obj->data.cylinder.center, token);
	if (result != PARSE_OK)
		return (result);
	result = validate_coordinate_range(&obj->data.cylinder.center);
	if (result != PARSE_OK)
		return (result);
	*token = skip_whitespace(*token);
	result = parse_vector_strict(*token, &obj->data.cylinder.axis, token);
	if (result != PARSE_OK)
		return (result);
	result = validate_vector_range(&obj->data.cylinder.axis);
	if (result != PARSE_OK)
		return (result);
	result = validate_direction_vector(&obj->data.cylinder.axis);
	if (result != PARSE_OK)
		return (result);
	obj->data.cylinder.axis = vec3_normalize(obj->data.cylinder.axis);
	return (PARSE_OK);
}

/**
 * @brief Parse a cylinder definition line into a scene object.
 *
 * Extracts center, axis, dimensions, and color, assigns an ID, and appends
 * the cylinder to the scene object list.
 *
 * @param line Raw line from the scene file.
 * @param scene Scene to update.
 * @return t_parse_result PARSE_OK on success, error code on failure.
 */
t_parse_result	parse_cylinder(char *line, t_scene *scene)
{
	const char		*token;
	t_object		obj;
	t_parse_result	result;

	ft_bzero(&obj, sizeof(t_object));
	obj.type = OBJ_CYLINDER;
	token = skip_whitespace(line + 3);
	result = parse_cyl_vectors(&token, &obj);
	if (result != PARSE_OK)
		return (result);
	token = skip_whitespace(token);
	result = parse_cylinder_dims(&token, &obj);
	if (result != PARSE_OK)
		return (result);
	result = parse_color_strict(skip_whitespace(token), &obj.color, &token);
	if (result != PARSE_OK)
		return (result);
	result = parse_bonus_options(&token, &obj);
	if (result != PARSE_OK)
		return (result);
	format_id(obj.id, 16, "cy-", get_type_count(scene, OBJ_CYLINDER) + 1);
	if (object_list_add(&scene->objects, &obj))
		return (PARSE_OK);
	free(obj.bump_path);
	return (PARSE_ERR_FORMAT);
}
