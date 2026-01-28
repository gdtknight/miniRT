/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_rotate.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/28 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window_internal.h"
#include <math.h>

static t_vec3	rodrigues_rotate(t_vec3 v, t_vec3 k, double angle)
{
	t_vec3	cross;
	double	cos_a;
	double	sin_a;
	double	dot;
	t_vec3	result;

	cos_a = cos(angle);
	sin_a = sin(angle);
	dot = vec3_dot(k, v);
	cross = vec3_cross(k, v);
	result = vec3_add(vec3_multiply(v, cos_a),
			vec3_add(vec3_multiply(cross, sin_a),
				vec3_multiply(k, dot * (1.0 - cos_a))));
	return (result);
}

static t_vec3	get_rotation_axis(int keycode, double *angle)
{
	double	step;

	step = M_PI / 36.0;
	if (keycode == KEY_U || keycode == KEY_O)
	{
		if (keycode == KEY_O)
			step = -step;
		*angle = step;
		return ((t_vec3){1.0, 0.0, 0.0});
	}
	if (keycode == KEY_Y || keycode == KEY_P)
	{
		if (keycode == KEY_P)
			step = -step;
		*angle = step;
		return ((t_vec3){0.0, 1.0, 0.0});
	}
	if (keycode == KEY_RIGHT)
		step = -step;
	*angle = step;
	return ((t_vec3){0.0, 0.0, 1.0});
}

static void	apply_rotation(t_object *obj, t_vec3 axis, double angle)
{
	t_vec3	rotated;
	double	len;

	if (obj->type == OBJ_CYLINDER)
		rotated = rodrigues_rotate(obj->data.cylinder.axis, axis, angle);
	else
		rotated = rodrigues_rotate(obj->data.plane.normal, axis, angle);
	len = vec3_magnitude(rotated);
	if (len < EPSILON)
		return ;
	rotated = vec3_normalize(rotated);
	if (obj->type == OBJ_CYLINDER)
		obj->data.cylinder.axis = rotated;
	else
		obj->data.plane.normal = rotated;
}

void	handle_object_rotate(t_render *render, int keycode)
{
	t_object	*obj;
	int			idx;
	t_vec3		rot_axis;
	double		angle;

	idx = render->selection.index;
	if (idx < 0 || idx >= render->scene->objects.count)
		return ;
	obj = &render->scene->objects.items[idx];
	if (obj->type == OBJ_SPHERE)
		return ;
	if (obj->type != OBJ_CYLINDER && obj->type != OBJ_PLANE)
		return ;
	rot_axis = get_rotation_axis(keycode, &angle);
	apply_rotation(obj, rot_axis, angle);
	render_set_flag(render, RENDER_BVH_DIRTY);
	debounce_on_input(&render->debounce, render);
}
