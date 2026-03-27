/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_resize.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/28 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scene.h"
#include "render.h"
#include "input.h"

static int	resize_sphere(t_object *obj, int keycode)
{
	double	step;

	step = 1.0;
	if (keycode == KEY_Y)
		step = -1.0;
	else if (keycode != KEY_U)
		return (0);
	if (obj->data.sphere.radius + step < 0.1)
		return (0);
	obj->data.sphere.radius += step;
	obj->data.sphere.radius_sq = obj->data.sphere.radius
		* obj->data.sphere.radius;
	return (1);
}

static int	resize_cylinder(t_object *obj, int keycode)
{
	double	step;

	step = 1.0;
	if (keycode == KEY_Y || keycode == KEY_U)
	{
		if (keycode == KEY_Y)
			step = -1.0;
		if (obj->data.cylinder.radius + step < 0.1)
			return (0);
		obj->data.cylinder.radius += step;
		obj->data.cylinder.radius_sq = obj->data.cylinder.radius
			* obj->data.cylinder.radius;
		return (1);
	}
	if (keycode == KEY_M)
		step = -1.0;
	if (obj->data.cylinder.half_height + step < 0.1)
		return (0);
	obj->data.cylinder.half_height += step;
	return (1);
}

static int	resize_cone(t_object *obj, int keycode)
{
	double	step;

	step = 1.0;
	if (keycode == KEY_Y || keycode == KEY_U)
	{
		if (keycode == KEY_Y)
			step = -1.0;
		if (obj->data.cone.radius + step < 0.1)
			return (0);
		obj->data.cone.radius += step;
		obj->data.cone.radius_sq = obj->data.cone.radius
			* obj->data.cone.radius;
		return (1);
	}
	if (keycode == KEY_M)
		step = -1.0;
	if (obj->data.cone.half_height + step < 0.1)
		return (0);
	obj->data.cone.half_height += step;
	return (1);
}

void	handle_object_resize(t_render *render, int keycode)
{
	t_object	*obj;
	int			idx;
	int			changed;

	idx = render->selection.index;
	if (idx < 0 || idx >= render->scene->objects.count)
		return ;
	obj = &render->scene->objects.items[idx];
	changed = 0;
	if (obj->type == OBJ_SPHERE)
		changed = resize_sphere(obj, keycode);
	else if (obj->type == OBJ_CYLINDER)
		changed = resize_cylinder(obj, keycode);
	else if (obj->type == OBJ_CONE)
		changed = resize_cone(obj, keycode);
	if (!changed)
		return ;
	render_set_flag(render, RENDER_BVH_DIRTY);
	debounce_on_input(&render->debounce, render);
}
