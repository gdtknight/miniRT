/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elements.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:28 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "vec3.h"
#include "error.h"

/**
 * @brief Parse ambient light definition.
 *
 * Validates ratio and color, applies to the scene, and marks the ambient flag.
 *
 * @param line Raw line from the scene file.
 * @param scene Scene to update.
 * @return t_parse_result PARSE_OK on success, error code on failure.
 */
t_parse_result	parse_ambient(char *line, t_scene *scene)
{
	const char		*token;
	double			ratio;
	t_parse_result	result;

	if (scene_has_ambient(scene))
		return (PARSE_ERR_DUPLICATE);
	token = skip_whitespace(line + 2);
	result = parse_double(token, &ratio, &token);
	if (result != PARSE_OK)
		return (result);
	if (!in_range(ratio, 0.0, 1.0))
		return (PARSE_ERR_RANGE);
	scene->ambient.ratio = ratio;
	token = skip_whitespace(token);
	result = parse_color_strict(token, &scene->ambient.color, &token);
	if (result != PARSE_OK)
		return (result);
	if (!at_line_end(token))
		return (PARSE_ERR_TRAILING_TOKEN);
	scene_set_flag(scene, SCENE_HAS_AMBIENT);
	return (PARSE_OK);
}

/**
 * @brief Parse camera position and direction.
 *
 * @param token Current token pointer.
 * @param scene Scene to update.
 * @return t_parse_result PARSE_OK on success, error code on failure.
 */
static t_parse_result	parse_camera_vecs(const char **token, t_scene *scene)
{
	t_parse_result	result;

	result = parse_vector_strict(*token, &scene->camera.position, token);
	if (result != PARSE_OK)
		return (result);
	*token = skip_whitespace(*token);
	result = parse_vector_strict(*token, &scene->camera.direction, token);
	if (result != PARSE_OK)
		return (result);
	result = validate_vector_range(&scene->camera.direction);
	if (result != PARSE_OK)
		return (result);
	result = validate_direction_vector(&scene->camera.direction);
	if (result != PARSE_OK)
		return (result);
	scene->camera.direction = vec3_normalize(scene->camera.direction);
	return (PARSE_OK);
}

/**
 * @brief Parse camera definition.
 *
 * Extracts position, direction, and FOV, normalizes direction, and stores
 * initial camera state for reset operations.
 *
 * @param line Raw line from the scene file.
 * @param scene Scene to update.
 * @return t_parse_result PARSE_OK on success, error code on failure.
 */
t_parse_result	parse_camera(char *line, t_scene *scene)
{
	const char		*token;
	t_parse_result	result;
	int				fov;

	if (scene_has_camera(scene))
		return (PARSE_ERR_DUPLICATE);
	token = skip_whitespace(line + 2);
	result = parse_camera_vecs(&token, scene);
	if (result != PARSE_OK)
		return (result);
	token = skip_whitespace(token);
	result = parse_int(token, &fov, &token);
	if (result != PARSE_OK)
		return (result);
	if (!in_range(fov, 1, 179))
		return (PARSE_ERR_RANGE);
	scene->camera.fov = fov;
	if (!at_line_end(token))
		return (PARSE_ERR_TRAILING_TOKEN);
	scene->camera.initial_position = scene->camera.position;
	scene->camera.initial_direction = scene->camera.direction;
	scene->camera.cache.valid = 0;
	scene_set_flag(scene, SCENE_HAS_CAMERA);
	return (PARSE_OK);
}

/**
 * @brief Parse light source definition.
 *
 * Extracts position, brightness, and color, validates ranges, and marks the
 * light flag in the scene.
 *
 * @param line Raw line from the scene file.
 * @param scene Scene to update.
 * @return t_parse_result PARSE_OK on success, error code on failure.
 */
t_parse_result	parse_light(char *line, t_scene *scene)
{
	const char		*token;
	t_parse_result	result;
	t_light			*light;

	if (scene->light_count >= MAX_LIGHTS)
		return (PARSE_ERR_OVERFLOW);
	light = &scene->lights[scene->light_count];
	token = skip_whitespace(line + 2);
	result = parse_vector_strict(token, &light->position, &token);
	if (result != PARSE_OK)
		return (result);
	token = skip_whitespace(token);
	result = parse_double(token, &light->brightness, &token);
	if (result != PARSE_OK)
		return (result);
	if (!in_range(light->brightness, 0.0, 1.0))
		return (PARSE_ERR_RANGE);
	token = skip_whitespace(token);
	result = parse_color_strict(token, &light->color, &token);
	if (result != PARSE_OK)
		return (result);
	if (!at_line_end(token))
		return (PARSE_ERR_TRAILING_TOKEN);
	scene->light_count++;
	return (PARSE_OK);
}
