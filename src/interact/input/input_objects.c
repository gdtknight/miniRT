/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_objects.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:36:02 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render/render.h"
#include "interact/input.h"

/**
 * @brief Move the currently selected object by a delta vector.
 *
 * Applies the translation to the correct object type and ignores invalid
 * selection indices.
 *
 * @param render Render context containing selection and scene.
 * @param move Translation vector to apply.
 * @return 1 if the object was moved, 0 otherwise.
 */
static int	move_selected_object(t_render *render, t_vec3 move)
{
	t_object	*obj;
	int			idx;

	idx = render->selection.index;
	if (idx < 0 || idx >= render->scene->objects.count)
		return (0);
	obj = &render->scene->objects.items[idx];
	if (obj->type == OBJ_SPHERE)
		obj->data.sphere.center = vec3_add(obj->data.sphere.center, move);
	else if (obj->type == OBJ_PLANE)
		obj->data.plane.point = vec3_add(obj->data.plane.point, move);
	else if (obj->type == OBJ_CYLINDER)
		obj->data.cylinder.center = vec3_add(obj->data.cylinder.center, move);
	else if (obj->type == OBJ_CONE)
		obj->data.cone.center = vec3_add(obj->data.cone.center, move);
	else
		return (0);
	return (1);
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
	if (!move_selected_object(render, move))
		return ;
	render_set_flag(render, RENDER_BVH_DIRTY);
	debounce_on_input(&render->debounce, render);
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

	move = (t_vec3){0, 0, 0};
	if (keycode == KEY_BRACKET_LEFT)
		move.x = -1.0;
	else if (keycode == KEY_BRACKET_RIGHT)
		move.x = 1.0;
	else if (keycode == KEY_SEMICOLON)
		move.y = -1.0;
	else if (keycode == KEY_QUOTE)
		move.y = 1.0;
	else if (keycode == KEY_COMMA)
		move.z = -1.0;
	else if (keycode == KEY_PERIOD)
		move.z = 1.0;
	else
		return ;
	if (!render->scene->light_count)
		return ;
	render->scene->lights[render->scene->selected_light].position
		= vec3_add(
			render->scene->lights[render->scene->selected_light].position,
			move);
	debounce_on_input(&render->debounce, render);
}
