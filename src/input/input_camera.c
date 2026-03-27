/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_camera.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/12 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render.h"
#include "input.h"
#include <math.h>

static void	camera_rebuild_dir(t_camera *cam)
{
	double	cos_p;
	double	limit;

	limit = M_PI / 2.0 - 0.01;
	if (cam->pitch > limit)
		cam->pitch = limit;
	if (cam->pitch < -limit)
		cam->pitch = -limit;
	cam->yaw = fmod(cam->yaw, 2.0 * M_PI);
	cos_p = cos(cam->pitch);
	cam->direction.x = cos_p * sin(cam->yaw);
	cam->direction.y = sin(cam->pitch);
	cam->direction.z = cos_p * cos(cam->yaw);
	cam->direction = vec3_normalize(cam->direction);
	cam->cache.valid = 0;
}

void	handle_camera_move(t_render *render, int keycode)
{
	t_vec3	right;
	t_vec3	up;
	t_vec3	move;
	double	step;

	step = 1.0;
	right = (t_vec3){-cos(render->scene->camera.yaw), 0,
		sin(render->scene->camera.yaw)};
	up = vec3_normalize(vec3_cross(right, render->scene->camera.direction));
	if (keycode == KEY_W)
		move = vec3_multiply(render->scene->camera.direction, step);
	else if (keycode == KEY_X)
		move = vec3_multiply(render->scene->camera.direction, -step);
	else if (keycode == KEY_A)
		move = vec3_multiply(right, -step);
	else if (keycode == KEY_D)
		move = vec3_multiply(right, step);
	else if (keycode == KEY_Q)
		move = vec3_multiply(up, step);
	else if (keycode == KEY_Z)
		move = vec3_multiply(up, -step);
	else
		return ;
	render->scene->camera.position = vec3_add(render->scene->camera.position,
			move);
}

void	handle_camera_pitch(t_render *render, int keycode)
{
	double	step;

	step = 5.0 * M_PI / 180.0;
	if (keycode == KEY_C)
		step = -step;
	else if (keycode != KEY_E)
		return ;
	render->scene->camera.pitch += step;
	camera_rebuild_dir(&render->scene->camera);
}

void	handle_camera_reset(t_render *render)
{
	t_camera	*cam;

	cam = &render->scene->camera;
	cam->position = cam->initial_position;
	cam->direction = cam->initial_direction;
	cam->pitch = asin(fmax(-1.0, fmin(1.0, cam->initial_direction.y)));
	cam->yaw = atan2(cam->initial_direction.x, cam->initial_direction.z);
	cam->cache.valid = 0;
}

void	handle_camera_yaw(t_render *render, int keycode)
{
	double	step;

	step = 5.0 * M_PI / 180.0;
	if (keycode == KEY_3)
		step = -step;
	else if (keycode != KEY_1)
		return ;
	render->scene->camera.yaw += step;
	camera_rebuild_dir(&render->scene->camera);
}
