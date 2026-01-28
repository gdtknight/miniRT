/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_objects.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 14:14:36 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window_internal.h"

/*
** Move the currently selected object by the given delta vector.
*/
/**
 * @brief move selected object 함수
 *
 * @param render 파라미터
 * @param move 파라미터
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

/*
** Handle object movement with RTFGVB keys.
** R/T: X axis, F/G: Y axis, V/B: Z axis
*/
/**
 * @brief handle object move 함수
 *
 * @param render 파라미터
 * @param keycode 파라미터
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

/*
** Handle light movement with Insert/Delete/Home/End/PgUp/PgDn keys.
** Insert/Delete: X axis, Home/End: Y axis, PgUp/PgDn: Z axis
*/
/**
 * @brief handle light move 함수
 *
 * @param render 파라미터
 * @param keycode 파라미터
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
