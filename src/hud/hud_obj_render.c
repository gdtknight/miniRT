/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_obj_render.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"
#include "hud_internal.h"

/**
 * @brief Get the position of an object for HUD display.
 *
 * @param obj Object to query.
 * @return t_vec3 Position vector or zero vector if unsupported.
 */
static t_vec3	get_object_position(t_object *obj)
{
	if (obj->type == OBJ_SPHERE)
		return (obj->data.sphere.center);
	else if (obj->type == OBJ_PLANE)
		return (obj->data.plane.point);
	else if (obj->type == OBJ_CYLINDER)
		return (obj->data.cylinder.center);
	else if (obj->type == OBJ_CONE)
		return (obj->data.cone.center);
	return ((t_vec3){0, 0, 0});
}

/**
 * @brief Append object ID and label into the buffer.
 *
 * @param buf Destination buffer.
 * @param i Current index (in/out).
 * @param obj Object containing ID.
 */
static void	fill_obj_id(char *buf, int *i, t_object *obj)
{
	int	j;

	j = 0;
	while (obj->id[j] && j < 15)
		buf[(*i)++] = obj->id[j++];
	buf[(*i)++] = ':';
	buf[(*i)++] = ' ';
	buf[(*i)++] = 'p';
	buf[(*i)++] = 'o';
	buf[(*i)++] = 's';
	buf[*i] = '\0';
}

/**
 * @brief Render a single object entry in the HUD list.
 *
 * Formats the object ID and position and renders it at the current y.
 *
 * @param render Render context containing scene and MLX handles.
 * @param idx Object index in the scene list.
 * @param y Current y position (in/out).
 * @param color Text color for the entry.
 */
void	hud_render_object_entry(t_render *render, int idx, int *y, int color)
{
	char		buf[128];
	int			i;
	t_object	*obj;

	obj = &render->scene->objects.items[idx];
	i = 0;
	fill_obj_id(buf, &i, obj);
	hud_format_vec3(buf + i, get_object_position(obj));
	hud_print_line(render, y, color, buf);
}
