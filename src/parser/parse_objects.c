/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_objects.c                                    :+:      :+:    :+:   */
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

int	parse_sphere(char *line, t_scene *scene)
{
	char		*token;
	t_sphere	*sphere;

	if (scene->sphere_count >= 100)
		return (print_error("Too many spheres"));
	sphere = &scene->spheres[scene->sphere_count];
	token = line + 3;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &sphere->center))
		return (print_error("Invalid sphere center"));
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	sphere->diameter = atof(token);
	if (sphere->diameter <= 0)
		return (print_error("Sphere diameter must be positive"));
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	if (!parse_color(token, &sphere->color))
		return (0);
	scene->sphere_count++;
	return (1);
}

int	parse_plane(char *line, t_scene *scene)
{
	char		*token;
	t_plane		*plane;

	if (scene->plane_count >= 100)
		return (print_error("Too many planes"));
	plane = &scene->planes[scene->plane_count];
	token = line + 3;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &plane->point))
		return (print_error("Invalid plane point"));
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &plane->normal))
		return (print_error("Invalid plane normal"));
	plane->normal = vec3_normalize(plane->normal);
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	if (!parse_color(token, &plane->color))
		return (0);
	scene->plane_count++;
	return (1);
}

int	parse_cylinder(char *line, t_scene *scene)
{
	char		*token;
	t_cylinder	*cylinder;

	if (scene->cylinder_count >= 100)
		return (print_error("Too many cylinders"));
	cylinder = &scene->cylinders[scene->cylinder_count];
	token = line + 3;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &cylinder->center))
		return (print_error("Invalid cylinder center"));
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &cylinder->axis))
		return (print_error("Invalid cylinder axis"));
	cylinder->axis = vec3_normalize(cylinder->axis);
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	cylinder->diameter = atof(token);
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	cylinder->height = atof(token);
	if (cylinder->diameter <= 0 || cylinder->height <= 0)
		return (print_error("Cylinder dimensions must be positive"));
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	if (!parse_color(token, &cylinder->color))
		return (0);
	scene->cylinder_count++;
	return (1);
}
