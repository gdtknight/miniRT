/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_camera.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/12 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window_internal.h"
#include <math.h>

static void	camera_rebuild_dir(t_camera *cam)
{
	double	cos_p;

	cam->pitch = fmod(cam->pitch, 2.0 * M_PI);
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
	t_vec3	move;
	double	step;

	step = 1.0;
	right.x = -cos(render->scene->camera.yaw);
	right.y = 0;
	right.z = sin(render->scene->camera.yaw);
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
	cam->pitch = asin(cam->initial_direction.y);
	cam->yaw = atan2(cam->initial_direction.x, cam->initial_direction.z);
	cam->cache.valid = 0;
}

void	handle_camera_yaw(t_render *render, int keycode)
{
	double	step;

	step = 5.0 * M_PI / 180.0;
	if (keycode == KEY_1)
		step = -step;
	else if (keycode != KEY_3)
		return ;
	render->scene->camera.yaw += step;
	camera_rebuild_dir(&render->scene->camera);
}
