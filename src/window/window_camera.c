/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_camera.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:35:09 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window_internal.h"
#include <math.h>

static t_vec3	rotate_dir(t_vec3 dir, t_vec3 axis, double cos_a, double sin_a)
{
	t_vec3	result;

	result.x = dir.x * cos_a + (axis.y * dir.z - axis.z * dir.y) * sin_a;
	result.y = dir.y * cos_a + (axis.z * dir.x - axis.x * dir.z) * sin_a;
	result.z = dir.z * cos_a + (axis.x * dir.y - axis.y * dir.x) * sin_a;
	return (result);
}

/**
 * @brief Move the camera based on keyboard input.
 *
 * Translates the camera position along forward/back, right/left, and up/down
 * axes depending on the pressed key.
 *
 * @param render Render context containing the scene and camera.
 * @param keycode Key code identifying the movement direction.
 */
void	handle_camera_move(t_render *render, int keycode)
{
	t_vec3	right;
	t_vec3	move;
	double	step;

	step = 1.0;
	right = vec3_normalize(vec3_cross(render->scene->camera.direction,
				(t_vec3){0, 1, 0}));
	if (keycode == KEY_W)
		move = vec3_multiply(render->scene->camera.direction, step);
	else if (keycode == KEY_X)
		move = vec3_multiply(render->scene->camera.direction, -step);
	else if (keycode == KEY_A)
		move = vec3_multiply(right, -step);
	else if (keycode == KEY_D)
		move = vec3_multiply(right, step);
	else if (keycode == KEY_Q)
		move = (t_vec3){0, step, 0};
	else if (keycode == KEY_Z)
		move = (t_vec3){0, -step, 0};
	else
		return ;
	render->scene->camera.position = vec3_add(render->scene->camera.position,
			move);
	render->scene->camera.cache.valid = 0;
}

/**
 * @brief Pitch the camera up or down.
 *
 * Rotates the camera direction around its right vector using fixed step
 * increments and keeps the direction normalized.
 *
 * @param render Render context containing the scene and camera.
 * @param keycode Key code selecting pitch direction.
 */
void	handle_camera_pitch(t_render *render, int keycode)
{
	t_vec3	right;
	t_vec3	new_dir;
	double	angle;

	angle = 5.0 * M_PI / 180.0;
	if (keycode == KEY_C)
		angle = -angle;
	else if (keycode != KEY_E)
		return ;
	right = vec3_normalize(vec3_cross(render->scene->camera.direction,
				(t_vec3){0, 1, 0}));
	new_dir = rotate_dir(render->scene->camera.direction, right,
			cos(angle), sin(angle));
	render->scene->camera.direction = vec3_normalize(new_dir);
	render->scene->camera.cache.valid = 0;
}

/**
 * @brief Reset the camera to its initial position and direction.
 *
 * Restores the camera state captured at initialization.
 *
 * @param render Render context containing the scene and camera.
 */
void	handle_camera_reset(t_render *render)
{
	render->scene->camera.position = render->scene->camera.initial_position;
	render->scene->camera.direction = render->scene->camera.initial_direction;
	render->scene->camera.cache.valid = 0;
}
