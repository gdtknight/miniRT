/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_resize.c                                    :+:      :+:    :+:   */
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

/**
 * @brief Resize a sphere by adjusting its radius.
 *
 * Applies a fixed step based on key input, clamps to a minimum radius,
 * and updates the cached radius squared value.
 *
 * @param obj Sphere object to resize.
 * @param keycode Key code indicating resize direction.
 */
static void	resize_sphere(t_object *obj, int keycode)
{
	double	step;

	step = 1.0;
	if (keycode == KEY_K)
		step = -1.0;
	else if (keycode != KEY_J)
		return ;
	if (obj->data.sphere.radius + step < 0.1)
		return ;
	obj->data.sphere.radius += step;
	obj->data.sphere.radius_sq = obj->data.sphere.radius
		* obj->data.sphere.radius;
}

/**
 * @brief Resize a cylinder by adjusting radius or height.
 *
 * Uses key input to change radius (J/K) or half-height (N/M), clamps to a
 * minimum size, and updates cached radius squared when needed.
 *
 * @param obj Cylinder object to resize.
 * @param keycode Key code indicating resize direction.
 */
static void	resize_cylinder(t_object *obj, int keycode)
{
	double	step;

	step = 1.0;
	if (keycode == KEY_J || keycode == KEY_K)
	{
		if (keycode == KEY_K)
			step = -1.0;
		if (obj->data.cylinder.radius + step < 0.1)
			return ;
		obj->data.cylinder.radius += step;
		obj->data.cylinder.radius_sq = obj->data.cylinder.radius
			* obj->data.cylinder.radius;
	}
	else if (keycode == KEY_N || keycode == KEY_M)
	{
		if (keycode == KEY_M)
			step = -1.0;
		if (obj->data.cylinder.half_height + step < 0.1)
			return ;
		obj->data.cylinder.half_height += step;
	}
}

/**
 * @brief Handle object resize keys for the selected object.
 *
 * Applies resize operations to spheres and cylinders, marks BVH dirty, and
 * triggers debounce for re-rendering.
 *
 * @param render Render context containing selection and scene.
 * @param keycode Key code indicating resize action.
 */
void	handle_object_resize(t_render *render, int keycode)
{
	t_object	*obj;
	int			idx;

	idx = render->selection.index;
	if (idx < 0 || idx >= render->scene->objects.count)
		return ;
	obj = &render->scene->objects.items[idx];
	if (obj->type == OBJ_SPHERE)
		resize_sphere(obj, keycode);
	else if (obj->type == OBJ_CYLINDER)
		resize_cylinder(obj, keycode);
	else
		return ;
	render_set_flag(render, RENDER_BVH_DIRTY);
	debounce_on_input(&render->debounce, render);
}
