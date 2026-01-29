/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_objects.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:34 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "vec3.h"
#include "utils.h"

static char	*skip_to_next_token(char *token)
{
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	return (token);
}

static int	get_type_count(t_scene *scene, t_object_type type)
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

int	parse_sphere(char *line, t_scene *scene)
{
	char		*token;
	t_object	obj;
	double		diameter;

	obj.type = OBJ_SPHERE;
	token = line + 3;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &obj.data.sphere.center))
		return (print_error("Invalid sphere center"));
	token = skip_to_next_token(token);
	diameter = ft_atof(token);
	if (diameter <= 0)
		return (print_error("Sphere diameter must be positive"));
	obj.data.sphere.radius = diameter / 2.0;
	obj.data.sphere.radius_sq = obj.data.sphere.radius * obj.data.sphere.radius;
	format_id(obj.id, 8, "sp-", get_type_count(scene, OBJ_SPHERE) + 1);
	token = skip_to_next_token(token);
	if (!parse_color(token, &obj.color))
		return (0);
	if (!object_list_add(&scene->objects, &obj))
		return (print_error("Failed to add sphere"));
	return (1);
}

int	parse_plane(char *line, t_scene *scene)
{
	char		*token;
	t_object	obj;

	obj.type = OBJ_PLANE;
	token = line + 3;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &obj.data.plane.point))
		return (print_error("Invalid plane point"));
	token = skip_to_next_token(token);
	if (!parse_vector(token, &obj.data.plane.normal))
		return (print_error("Invalid plane normal"));
	obj.data.plane.normal = vec3_normalize(obj.data.plane.normal);
	format_id(obj.id, 8, "pl-", get_type_count(scene, OBJ_PLANE) + 1);
	token = skip_to_next_token(token);
	if (!parse_color(token, &obj.color))
		return (0);
	if (!object_list_add(&scene->objects, &obj))
		return (print_error("Failed to add plane"));
	return (1);
}
