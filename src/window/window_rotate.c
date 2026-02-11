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

/**
 * @brief Rotate a vector around an axis using Rodrigues' formula.
 *
 * Applies the rotation of vector v around unit axis k by the given angle.
 *
 * @param v Vector to rotate.
 * @param k Rotation axis (should be normalized).
 * @param angle Rotation angle in radians.
 * @return t_vec3 Rotated vector.
 */
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

/**
 * @brief Map rotation key input to axis and angle.
 *
 * Determines rotation axis based on key input and writes the signed step
 * angle to the output parameter.
 *
 * @param keycode Key code indicating rotation direction.
 * @param angle Output pointer to receive angle in radians.
 * @return t_vec3 Rotation axis vector.
 */
static t_vec3	get_rotation_axis(int keycode, double *angle)
{
	double	step;

	step = M_PI / 36.0;
	if (keycode == KEY_I || keycode == KEY_J)
	{
		if (keycode == KEY_I)
			step = -step;
		*angle = step;
		return ((t_vec3){1.0, 0.0, 0.0});
	}
	if (keycode == KEY_O || keycode == KEY_K)
	{
		if (keycode == KEY_O)
			step = -step;
		*angle = step;
		return ((t_vec3){0.0, 1.0, 0.0});
	}
	if (keycode == KEY_P)
		step = -step;
	*angle = step;
	return ((t_vec3){0.0, 0.0, 1.0});
}

/**
 * @brief Apply rotation to a cylinder axis or plane normal.
 *
 * Rotates the relevant direction vector, normalizes it, and writes it back
 * to the object if the result is valid.
 *
 * @param obj Object to rotate (cylinder or plane).
 * @param axis Rotation axis.
 * @param angle Rotation angle in radians.
 */
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

/**
 * @brief Handle object rotation keys for the selected object.
 *
 * Applies axis rotations to planes and cylinders, marks BVH dirty, and
 * triggers debounce for re-rendering.
 *
 * @param render Render context containing selection and scene.
 * @param keycode Key code indicating rotation direction.
 */
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
