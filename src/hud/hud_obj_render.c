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

#include "window.h"
#include "hud.h"

static t_vec3	get_object_position(t_object *obj)
{
	if (obj->type == OBJ_SPHERE)
		return (obj->data.sphere.center);
	else if (obj->type == OBJ_PLANE)
		return (obj->data.plane.point);
	else if (obj->type == OBJ_CYLINDER)
		return (obj->data.cylinder.center);
	return ((t_vec3){0, 0, 0});
}

static void	fill_obj_id(char *buf, int *i, t_object *obj)
{
	int	j;

	j = 0;
	while (obj->id[j])
		buf[(*i)++] = obj->id[j++];
	buf[(*i)++] = ':';
	buf[(*i)++] = ' ';
	buf[(*i)++] = 'p';
	buf[(*i)++] = 'o';
	buf[(*i)++] = 's';
	buf[*i] = '\0';
}

void	render_object_entry(t_render *render, int idx, int *y, int color)
{
	char		buf[128];
	int			i;
	t_object	*obj;

	obj = &render->scene->objects.items[idx];
	i = 0;
	fill_obj_id(buf, &i, obj);
	hud_format_vec3(buf + i, get_object_position(obj));
	mlx_string_put(render->mlx.mlx, render->mlx.win, HUD_MARGIN_X + 10,
		*y, color, buf);
	*y += HUD_LINE_HEIGHT;
}
