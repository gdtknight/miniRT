/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elements.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:28 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:29 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "vec3.h"

/*
** Parse ambient lighting element from scene file.
** Format: A <ratio> <R,G,B>
** Validates ratio is in range [0.0, 1.0].
*/
int	parse_ambient(char *line, t_scene *scene)
{
	char	*token;
	double	ratio;

	if (scene->has_ambient)
		return (print_error("Ambient lighting declared multiple times"));
	token = line + 2;
	while (*token == ' ')
		token++;
	ratio = ft_atof(token);
	if (!in_range(ratio, 0.0, 1.0))
		return (print_error("Ambient ratio must be in range [0.0, 1.0]"));
	scene->ambient.ratio = ratio;
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	if (!parse_color(token, &scene->ambient.color))
		return (0);
	scene->has_ambient = 1;
	return (1);
}

/*
** Parse camera element from scene file.
** Format: C <x,y,z> <nx,ny,nz> <fov>
** Normalizes direction vector and validates FOV range [0, 180].
*/
int	parse_camera(char *line, t_scene *scene)
{
	char	*token;

	if (scene->has_camera)
		return (print_error("Camera declared multiple times"));
	token = line + 2;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &scene->camera.position))
		return (print_error("Invalid camera position"));
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &scene->camera.direction))
		return (print_error("Invalid camera direction"));
	scene->camera.direction = vec3_normalize(scene->camera.direction);
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	scene->camera.fov = ft_atof(token);
	if (!in_range(scene->camera.fov, 0, 180))
		return (print_error("FOV must be in range [0, 180]"));
	scene->has_camera = 1;
	return (1);
}

/*
** Parse light source element from scene file.
** Format: L <x,y,z> <brightness> <R,G,B>
** Validates brightness is in range [0.0, 1.0].
*/
int	parse_light(char *line, t_scene *scene)
{
	char	*token;

	if (scene->has_light)
		return (print_error("Light declared multiple times"));
	token = line + 2;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &scene->light.position))
		return (print_error("Invalid light position"));
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	scene->light.brightness = ft_atof(token);
	if (!in_range(scene->light.brightness, 0.0, 1.0))
		return (print_error("Light brightness must be in range [0.0, 1.0]"));
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	if (!parse_color(token, &scene->light.color))
		return (0);
	scene->has_light = 1;
	return (1);
}
