/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_objects.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:34 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "vec3.h"
#include "utils.h"
#include <stdlib.h>

/**
 * @brief Count objects of a specific type in the scene.
 *
 * @param scene Scene containing object list.
 * @param type Object type to count.
 * @return int Number of objects of the given type.
 */
int	get_type_count(t_scene *scene, t_object_type type)
{
	int	i;
	int	count;

	count = 0;
	i = 0;
	while (i < scene->objects.count)
	{
		if (scene->objects.items[i].type == type)
			count++;
		i++;
	}
	return (count);
}

/**
 * @brief Parse sphere dimensions and color.
 *
 * @param token Current token pointer.
 * @param obj Object to populate.
 * @return t_parse_result PARSE_OK on success, error code on failure.
 */
static t_parse_result	parse_sphere_data(const char **token, t_object *obj)
{
	double			diameter;
	t_parse_result	result;

	result = parse_double(*token, &diameter, token);
	if (result != PARSE_OK)
		return (result);
	if (diameter <= 0)
		return (PARSE_ERR_RANGE);
	obj->data.sphere.radius = diameter / 2.0;
	obj->data.sphere.radius_sq = obj->data.sphere.radius
		* obj->data.sphere.radius;
	*token = skip_whitespace(*token);
	result = parse_color_strict(*token, &obj->color, token);
	return (result);
}

/**
 * @brief Parse a sphere definition line into a scene object.
 *
 * Extracts center, diameter, and color, assigns an ID, and appends the
 * sphere to the scene object list.
 *
 * @param line Raw line from the scene file.
 * @param scene Scene to update.
 * @return t_parse_result PARSE_OK on success, error code on failure.
 */
t_parse_result	parse_sphere(char *line, t_scene *scene)
{
	const char		*token;
	t_object		obj;
	t_parse_result	result;

	ft_bzero(&obj, sizeof(t_object));
	obj.type = OBJ_SPHERE;
	token = skip_whitespace(line + 3);
	result = parse_vector_strict(token, &obj.data.sphere.center, &token);
	if (result != PARSE_OK)
		return (result);
	result = validate_coordinate_range(&obj.data.sphere.center);
	if (result != PARSE_OK)
		return (result);
	token = skip_whitespace(token);
	result = parse_sphere_data(&token, &obj);
	if (result != PARSE_OK)
		return (result);
	result = parse_bonus_options(&token, &obj);
	if (result != PARSE_OK)
		return (result);
	format_id(obj.id, 16, "sp-", get_type_count(scene, OBJ_SPHERE) + 1);
	if (object_list_add(&scene->objects, &obj))
		return (PARSE_OK);
	free(obj.bump_path);
	return (PARSE_ERR_OVERFLOW);
}

/**
 * @brief Parse plane normal and color.
 *
 * @param token Current token pointer.
 * @param obj Object to populate.
 * @return t_parse_result PARSE_OK on success, error code on failure.
 */
static t_parse_result	parse_plane_data(const char **token, t_object *obj)
{
	t_parse_result	result;

	result = parse_vector_strict(*token, &obj->data.plane.normal, token);
	if (result != PARSE_OK)
		return (result);
	result = validate_vector_range(&obj->data.plane.normal);
	if (result != PARSE_OK)
		return (result);
	result = validate_direction_vector(&obj->data.plane.normal);
	if (result != PARSE_OK)
		return (result);
	obj->data.plane.normal = vec3_normalize(obj->data.plane.normal);
	*token = skip_whitespace(*token);
	result = parse_color_strict(*token, &obj->color, token);
	return (result);
}

/**
 * @brief Parse a plane definition line into a scene object.
 *
 * Extracts point, normal, and color, assigns an ID, and appends the plane
 * to the scene object list.
 *
 * @param line Raw line from the scene file.
 * @param scene Scene to update.
 * @return t_parse_result PARSE_OK on success, error code on failure.
 */
t_parse_result	parse_plane(char *line, t_scene *scene)
{
	const char		*token;
	t_object		obj;
	t_parse_result	result;

	ft_bzero(&obj, sizeof(t_object));
	obj.type = OBJ_PLANE;
	token = skip_whitespace(line + 3);
	result = parse_vector_strict(token, &obj.data.plane.point, &token);
	if (result != PARSE_OK)
		return (result);
	result = validate_coordinate_range(&obj.data.plane.point);
	if (result != PARSE_OK)
		return (result);
	token = skip_whitespace(token);
	result = parse_plane_data(&token, &obj);
	if (result != PARSE_OK)
		return (result);
	result = parse_bonus_options(&token, &obj);
	if (result != PARSE_OK)
		return (result);
	format_id(obj.id, 16, "pl-", get_type_count(scene, OBJ_PLANE) + 1);
	if (object_list_add(&scene->objects, &obj))
		return (PARSE_OK);
	free(obj.bump_path);
	return (PARSE_ERR_OVERFLOW);
}
