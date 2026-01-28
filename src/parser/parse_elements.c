/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elements.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:28 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "vec3.h"
#include "error.h"

static char	*skip_to_next_token(char *token)
{
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	return (token);
}

int	parse_ambient(char *line, t_scene *scene)
{
	char	*token;
	double	ratio;

	if (scene_has_ambient(scene))
		return (print_error("Ambient lighting declared multiple times"));
	token = line + 2;
	while (*token == ' ')
		token++;
	ratio = ft_atof(token);
	if (!in_range(ratio, 0.0, 1.0))
		return (print_error("Ambient ratio must be in range [0.0, 1.0]"));
	scene->ambient.ratio = ratio;
	token = skip_to_next_token(token);
	if (!parse_color(token, &scene->ambient.color))
		return (0);
	scene_set_flag(scene, SCENE_HAS_AMBIENT);
	return (1);
}

int	parse_camera(char *line, t_scene *scene)
{
	char	*token;

	if (scene_has_camera(scene))
		return (print_error("Camera declared multiple times"));
	token = line + 2;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &scene->camera.position))
		return (print_error("Invalid camera position"));
	token = skip_to_next_token(token);
	if (!parse_vector(token, &scene->camera.direction))
		return (print_error("Invalid camera direction"));
	scene->camera.direction = vec3_normalize(scene->camera.direction);
	token = skip_to_next_token(token);
	scene->camera.fov = ft_atof(token);
	if (!in_range(scene->camera.fov, 0, 180))
		return (print_error("FOV must be in range [0, 180]"));
	scene->camera.initial_position = scene->camera.position;
	scene->camera.initial_direction = scene->camera.direction;
	scene_set_flag(scene, SCENE_HAS_CAMERA);
	return (1);
}

int	parse_light(char *line, t_scene *scene)
{
	char	*token;

	if (scene_has_light(scene))
		return (print_error("Light declared multiple times"));
	token = line + 2;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &scene->light.position))
		return (print_error("Invalid light position"));
	token = skip_to_next_token(token);
	scene->light.brightness = ft_atof(token);
	if (!in_range(scene->light.brightness, 0.0, 1.0))
		return (print_error("Light brightness must be in range [0.0, 1.0]"));
	token = skip_to_next_token(token);
	if (!parse_color(token, &scene->light.color))
		return (0);
	scene_set_flag(scene, SCENE_HAS_LIGHT);
	return (1);
}
