/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_objects.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:36:02 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window_internal.h"

/**
 * @brief Move the currently selected object by a delta vector.
 *
 * Applies the translation to the correct object type and ignores invalid
 * selection indices.
 *
 * @param render Render context containing selection and scene.
 * @param move Translation vector to apply.
 */
static void	move_selected_object(t_render *render, t_vec3 move)
{
	t_object	*obj;
	int			idx;

	idx = render->selection.index;
	if (idx < 0 || idx >= render->scene->objects.count)
		return ;
	obj = &render->scene->objects.items[idx];
	if (obj->type == OBJ_SPHERE)
		obj->data.sphere.center = vec3_add(obj->data.sphere.center, move);
	else if (obj->type == OBJ_PLANE)
		obj->data.plane.point = vec3_add(obj->data.plane.point, move);
	else if (obj->type == OBJ_CYLINDER)
		obj->data.cylinder.center = vec3_add(obj->data.cylinder.center, move);
}

/**
 * @brief Handle object movement keys.
 *
 * Converts key input into axis-aligned movement and marks the BVH as dirty
 * to trigger rebuild.
 *
 * @param render Render context containing selection and flags.
 * @param keycode Key code identifying movement direction.
 */
void	handle_object_move(t_render *render, int keycode)
{
	t_vec3	move;
	double	step;

	step = 1.0;
	move = (t_vec3){0, 0, 0};
	if (keycode == KEY_R)
		move.x = -step;
	else if (keycode == KEY_T)
		move.x = step;
	else if (keycode == KEY_F)
		move.y = -step;
	else if (keycode == KEY_G)
		move.y = step;
	else if (keycode == KEY_V)
		move.z = -step;
	else if (keycode == KEY_B)
		move.z = step;
	else
		return ;
	move_selected_object(render, move);
	render_set_flag(render, RENDER_BVH_DIRTY);
}

/**
 * @brief Handle light movement keys.
 *
 * Translates the scene light position along the X/Y/Z axes based on key input.
 *
 * @param render Render context containing scene lighting.
 * @param keycode Key code identifying movement direction.
 */
void	handle_light_move(t_render *render, int keycode)
{
	t_vec3	move;
	double	step;

	step = 1.0;
	move = (t_vec3){0, 0, 0};
	if (keycode == KEY_INSERT)
		move.x = step;
	else if (keycode == KEY_DELETE)
		move.x = -step;
	else if (keycode == KEY_HOME)
		move.y = step;
	else if (keycode == KEY_END)
		move.y = -step;
	else if (keycode == KEY_PGUP)
		move.z = step;
	else if (keycode == KEY_PGDN)
		move.z = -step;
	else
		return ;
	render->scene->light.position = vec3_add(render->scene->light.position,
			move);
}
