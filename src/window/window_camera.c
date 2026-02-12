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

static t_vec3	get_camera_right(t_vec3 dir)
{
	t_vec3	up;

	up = (t_vec3){0, 1, 0};
	if (fabs(vec3_dot(dir, up)) > 0.99)
		up = (t_vec3){0, 0, 1};
	return (vec3_normalize(vec3_cross(dir, up)));
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
	right = get_camera_right(render->scene->camera.direction);
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
	right = get_camera_right(render->scene->camera.direction);
	new_dir = rodrigues_rotate(render->scene->camera.direction, right, angle);
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

/**
 * @brief Rotate the camera left or right around the Y axis (yaw).
 *
 * @param render Render context containing the scene and camera.
 * @param keycode Key code selecting yaw direction.
 */
void	handle_camera_yaw(t_render *render, int keycode)
{
	t_vec3	new_dir;
	double	angle;

	angle = 5.0 * M_PI / 180.0;
	if (keycode == KEY_1)
		angle = -angle;
	else if (keycode != KEY_3)
		return ;
	new_dir = rodrigues_rotate(render->scene->camera.direction,
			(t_vec3){0, 1, 0}, angle);
	render->scene->camera.direction = vec3_normalize(new_dir);
	render->scene->camera.cache.valid = 0;
}
