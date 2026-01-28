/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cylinder.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "vec3.h"
#include <stdio.h>

static char	*skip_to_next_token(char *token)
{
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	return (token);
}

static int	get_cylinder_count(t_scene *scene)
{
	int	i;
	int	count;

	count = 0;
	i = 0;
	while (i < scene->objects.count)
	{
		if (scene->objects.items[i].type == OBJ_CYLINDER)
			count++;
		i++;
	}
	return (count);
}

static int	parse_cylinder_dims(char *token, t_object *obj)
{
	double	diameter;
	double	height;

	token = skip_to_next_token(token);
	diameter = ft_atof(token);
	token = skip_to_next_token(token);
	height = ft_atof(token);
	if (diameter <= 0 || height <= 0)
		return (print_error("Cylinder dimensions must be positive"));
	obj->data.cylinder.radius = diameter / 2.0;
	obj->data.cylinder.radius_sq = obj->data.cylinder.radius
		* obj->data.cylinder.radius;
	obj->data.cylinder.half_height = height / 2.0;
	return (1);
}

int	parse_cylinder(char *line, t_scene *scene)
{
	char		*token;
	t_object	obj;

	obj.type = OBJ_CYLINDER;
	token = line + 3;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &obj.data.cylinder.center))
		return (print_error("Invalid cylinder center"));
	token = skip_to_next_token(token);
	if (!parse_vector(token, &obj.data.cylinder.axis))
		return (print_error("Invalid cylinder axis"));
	obj.data.cylinder.axis = vec3_normalize(obj.data.cylinder.axis);
	snprintf(obj.id, 8, "cy-%d", get_cylinder_count(scene) + 1);
	if (!parse_cylinder_dims(token, &obj))
		return (0);
	token = skip_to_next_token(skip_to_next_token(token));
	if (!parse_color(token, &obj.color))
		return (0);
	if (!object_list_add(&scene->objects, &obj))
		return (print_error("Failed to add cylinder"));
	return (1);
}
