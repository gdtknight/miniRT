/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:31 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:20:31 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "hud.h"
#include "keyguide.h"
#include <stdlib.h>
#include <math.h>

/*
** Handle window close event (X button clicked).
** Cleans up resources and exits program.
*/
int	close_window(void *param)
{
	t_render	*render;

	render = (t_render *)param;
	keyguide_cleanup(&render->keyguide, render->mlx);
	hud_cleanup(&render->hud, render->mlx);
	cleanup_all(render->scene, render);
	exit(0);
	return (0);
}

/* Forward declaration for re-rendering */
void	render_scene_to_buffer(t_scene *scene, t_render *render);

/* Platform-specific key code definitions */
#ifdef __APPLE__
/* macOS (Darwin) virtual key codes */
# define KEY_ESC 53
# define KEY_W 13
# define KEY_A 0
# define KEY_S 1
# define KEY_D 2
# define KEY_R 15
# define KEY_F 3
# define KEY_B 11
# define KEY_Q 12
# define KEY_I 34
# define KEY_H 4
# define KEY_TAB 48
# define KEY_SHIFT_L 56
# define KEY_SHIFT_R 60
# define KEY_UP 126
# define KEY_DOWN 125
# define KEY_BRACKET_LEFT 33
# define KEY_BRACKET_RIGHT 30
# define KEY_KP_0 82
# define KEY_KP_1 83
# define KEY_KP_2 84
# define KEY_KP_3 85
# define KEY_KP_4 86
# define KEY_KP_5 87
# define KEY_KP_6 88
# define KEY_KP_7 89
# define KEY_KP_8 91
# define KEY_KP_9 92
# define KEY_INSERT 114
# define KEY_HOME 115
# define KEY_PGUP 116
# define KEY_DELETE 117
# define KEY_END 119
# define KEY_PGDN 121
#elif defined(__linux__)
/* Linux/X11 KeySym values */
# define KEY_ESC 65307
# define KEY_W 119
# define KEY_A 97
# define KEY_S 115
# define KEY_D 100
# define KEY_R 114
# define KEY_F 102
# define KEY_B 98
# define KEY_Q 113
# define KEY_I 105
# define KEY_H 104
# define KEY_TAB 65289
# define KEY_SHIFT_L 65505
# define KEY_SHIFT_R 65506
# define KEY_UP 65362
# define KEY_DOWN 65364
# define KEY_BRACKET_LEFT 91
# define KEY_BRACKET_RIGHT 93
# define KEY_KP_0 65438
# define KEY_KP_1 65436
# define KEY_KP_2 65433
# define KEY_KP_3 65435
# define KEY_KP_4 65430
# define KEY_KP_5 65437
# define KEY_KP_6 65432
# define KEY_KP_7 65429
# define KEY_KP_8 65431
# define KEY_KP_9 65434
# define KEY_INSERT 65379
# define KEY_HOME 65360
# define KEY_PGUP 65365
# define KEY_DELETE 65535
# define KEY_END 65367
# define KEY_PGDN 65366
#else
# error "Unsupported platform - requires macOS or Linux"
#endif

static void	handle_camera_move(t_render *render, int keycode)
{
	t_vec3	right;
	t_vec3	move;
	double	step;

	step = 1.0;
	right = vec3_normalize(vec3_cross(render->scene->camera.direction,
			(t_vec3){0, 1, 0}));
	if (keycode == KEY_W)
		move = vec3_multiply(render->scene->camera.direction, step);
	else if (keycode == KEY_S)
		move = vec3_multiply(render->scene->camera.direction, -step);
	else if (keycode == KEY_A)
		move = vec3_multiply(right, -step);
	else if (keycode == KEY_D)
		move = vec3_multiply(right, step);
	else
		return ;
	render->scene->camera.position = vec3_add(render->scene->camera.position,
			move);
}

static void	handle_camera_pitch(t_render *render, int keycode)
{
	t_vec3	right;
	t_vec3	new_dir;
	double	angle;
	double	cos_a;
	double	sin_a;

	angle = 5.0 * M_PI / 180.0;
	if (keycode == KEY_F)
		angle = -angle;
	else if (keycode != KEY_R)
		return ;
	right = vec3_normalize(vec3_cross(render->scene->camera.direction,
			(t_vec3){0, 1, 0}));
	cos_a = cos(angle);
	sin_a = sin(angle);
	new_dir.x = render->scene->camera.direction.x * cos_a
		+ (right.y * render->scene->camera.direction.z
			- right.z * render->scene->camera.direction.y) * sin_a;
	new_dir.y = render->scene->camera.direction.y * cos_a
		+ (right.z * render->scene->camera.direction.x
			- right.x * render->scene->camera.direction.z) * sin_a;
	new_dir.z = render->scene->camera.direction.z * cos_a
		+ (right.x * render->scene->camera.direction.y
			- right.y * render->scene->camera.direction.x) * sin_a;
	render->scene->camera.direction = vec3_normalize(new_dir);
}

static void	handle_object_selection(t_render *render, int keycode)
{
	int	total;

	total = render->scene->sphere_count + render->scene->plane_count
		+ render->scene->cylinder_count;
	if (total == 0)
		return ;
	if (keycode == KEY_BRACKET_RIGHT)
	{
		if (render->selection.type == OBJ_NONE)
			render->selection.type = OBJ_SPHERE;
		else
		{
			render->selection.index++;
			if ((render->selection.type == OBJ_SPHERE
					&& render->selection.index >= render->scene->sphere_count)
				|| (render->selection.type == OBJ_PLANE
					&& render->selection.index >= render->scene->plane_count)
				|| (render->selection.type == OBJ_CYLINDER
					&& render->selection.index >= render->scene->cylinder_count))
			{
				render->selection.index = 0;
				if (render->selection.type == OBJ_SPHERE)
					render->selection.type = (render->scene->plane_count > 0)
						? OBJ_PLANE : ((render->scene->cylinder_count > 0)
							? OBJ_CYLINDER : OBJ_SPHERE);
				else if (render->selection.type == OBJ_PLANE)
					render->selection.type = (render->scene->cylinder_count > 0)
						? OBJ_CYLINDER : ((render->scene->sphere_count > 0)
							? OBJ_SPHERE : OBJ_PLANE);
				else
					render->selection.type = (render->scene->sphere_count > 0)
						? OBJ_SPHERE : ((render->scene->plane_count > 0)
							? OBJ_PLANE : OBJ_CYLINDER);
			}
		}
	}
	else if (keycode == KEY_BRACKET_LEFT)
	{
		if (render->selection.type == OBJ_NONE)
			render->selection.type = OBJ_SPHERE;
		else
		{
			render->selection.index--;
			if (render->selection.index < 0)
			{
				if (render->selection.type == OBJ_SPHERE)
				{
					render->selection.type = (render->scene->cylinder_count > 0)
						? OBJ_CYLINDER : ((render->scene->plane_count > 0)
							? OBJ_PLANE : OBJ_SPHERE);
					render->selection.index = (render->selection.type
							== OBJ_CYLINDER) ? render->scene->cylinder_count - 1
						: ((render->selection.type == OBJ_PLANE)
							? render->scene->plane_count - 1
							: render->scene->sphere_count - 1);
				}
				else if (render->selection.type == OBJ_PLANE)
				{
					render->selection.type = (render->scene->sphere_count > 0)
						? OBJ_SPHERE : ((render->scene->cylinder_count > 0)
							? OBJ_CYLINDER : OBJ_PLANE);
					render->selection.index = (render->selection.type
							== OBJ_SPHERE) ? render->scene->sphere_count - 1
						: render->scene->plane_count - 1;
				}
				else
				{
					render->selection.type = (render->scene->plane_count > 0)
						? OBJ_PLANE : ((render->scene->sphere_count > 0)
							? OBJ_SPHERE : OBJ_CYLINDER);
					render->selection.index = (render->selection.type
							== OBJ_PLANE) ? render->scene->plane_count - 1
						: render->scene->cylinder_count - 1;
				}
			}
		}
	}
	hud_mark_dirty(render);
}

static void	move_selected_object(t_render *render, t_vec3 move)
{
	if (render->selection.type == OBJ_SPHERE
		&& render->selection.index < render->scene->sphere_count)
		render->scene->spheres[render->selection.index].center
			= vec3_add(render->scene->spheres[render->selection.index].center,
				move);
	else if (render->selection.type == OBJ_PLANE
		&& render->selection.index < render->scene->plane_count)
		render->scene->planes[render->selection.index].point
			= vec3_add(render->scene->planes[render->selection.index].point,
				move);
	else if (render->selection.type == OBJ_CYLINDER
		&& render->selection.index < render->scene->cylinder_count)
		render->scene->cylinders[render->selection.index].center
			= vec3_add(render->scene->cylinders[render->selection.index].center,
				move);
}

static void	handle_object_move(t_render *render, int keycode)
{
	t_vec3	move;
	double	step;

	step = 1.0;
	move = (t_vec3){0, 0, 0};
	if (keycode == KEY_KP_4)
		move.x = -step;
	else if (keycode == KEY_KP_6)
		move.x = step;
	else if (keycode == KEY_KP_2)
		move.y = -step;
	else if (keycode == KEY_KP_8)
		move.y = step;
	else if (keycode == KEY_KP_1 || keycode == KEY_KP_3)
		move.z = (keycode == KEY_KP_1) ? -step : step;
	else
		return ;
	move_selected_object(render, move);
}

static void	handle_light_move(t_render *render, int keycode)
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

/*
** Handle keyboard input events with interactive controls.
** Uses dirty flag and low quality rendering for smooth interaction.
*/
int	handle_key(int keycode, void *param)
{
	t_render	*render;

	render = (t_render *)param;
	if (keycode == KEY_ESC)
		close_window(param);
	else if (keycode == KEY_H)
		hud_toggle(render);
	else if (keycode == KEY_TAB)
	{
		if (render->shift_pressed)
			hud_select_prev(render);
		else
			hud_select_next(render);
	}
	else if (keycode == KEY_UP)
		hud_page_up(render);
	else if (keycode == KEY_DOWN)
		hud_page_down(render);
	else if (keycode == KEY_W || keycode == KEY_A || keycode == KEY_S
		|| keycode == KEY_D)
	{
		handle_camera_move(render, keycode);
		render->low_quality = 1;
		render->dirty = 1;
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_R || keycode == KEY_F)
	{
		handle_camera_pitch(render, keycode);
		render->low_quality = 1;
		render->dirty = 1;
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_BRACKET_LEFT || keycode == KEY_BRACKET_RIGHT)
		handle_object_selection(render, keycode);
	else if (keycode >= KEY_KP_1 && keycode <= KEY_KP_8)
	{
		handle_object_move(render, keycode);
		render->low_quality = 1;
		render->dirty = 1;
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_INSERT || keycode == KEY_HOME || keycode == KEY_PGUP
		|| keycode == KEY_DELETE || keycode == KEY_END || keycode == KEY_PGDN)
	{
		handle_light_move(render, keycode);
		render->low_quality = 1;
		render->dirty = 1;
		hud_mark_dirty(render);
	}
	else if (keycode == KEY_B)
	{
		render->scene->render_state.bvh_enabled
			= !render->scene->render_state.bvh_enabled;
		render->dirty = 1;
	}
	else if (keycode == KEY_Q)
	{
		render->scene->render_state.adaptive_enabled
			= !render->scene->render_state.adaptive_enabled;
		render->dirty = 1;
	}
	else if (keycode == KEY_I)
	{
		render->scene->render_state.show_info
			= !render->scene->render_state.show_info;
		render->dirty = 1;
	}
	else if (keycode == KEY_SHIFT_L || keycode == KEY_SHIFT_R)
		render->shift_pressed = 1;
	return (0);
}

/*
** Handle key release events.
** Switches to high quality rendering when interaction stops.
*/
int	handle_key_release(int keycode, void *param)
{
	t_render	*render;

	render = (t_render *)param;
	if (keycode == KEY_W || keycode == KEY_A || keycode == KEY_S
		|| keycode == KEY_D || keycode == KEY_R || keycode == KEY_F
		|| (keycode >= KEY_KP_1 && keycode <= KEY_KP_8)
		|| keycode == KEY_INSERT || keycode == KEY_HOME || keycode == KEY_PGUP
		|| keycode == KEY_DELETE || keycode == KEY_END || keycode == KEY_PGDN)
	{
		render->low_quality = 0;
		render->dirty = 1;
	}
	else if (keycode == KEY_SHIFT_L || keycode == KEY_SHIFT_R)
		render->shift_pressed = 0;
	return (0);
}

/*
** Main rendering loop hook.
** Only re-renders when dirty flag is set.
*/
int	render_loop(void *param)
{
	t_render	*render;

	render = (t_render *)param;
	if (render->dirty)
	{
		render_scene_to_buffer(render->scene, render);
		mlx_put_image_to_window(render->mlx, render->win, render->img, 0, 0);
		render->dirty = 0;
	}
	if (render->hud.visible && render->hud.dirty)
	{
		hud_render(render);
		keyguide_render(render);
	}
	return (0);
}

/*
** Initialize MLX window and set up event handlers.
** Creates 800x600 window with image buffer for fast rendering.
** Registers close/keyboard handlers and rendering loop.
** Returns NULL on initialization failure.
*/
t_render	*init_window(t_scene *scene)
{
	t_render	*render;

	render = malloc(sizeof(t_render));
	if (!render)
		return (NULL);
	render->mlx = mlx_init();
	if (!render->mlx)
	{
		free(render);
		return (NULL);
	}
	render->win = mlx_new_window(render->mlx, WINDOW_WIDTH,
			WINDOW_HEIGHT, "miniRT");
	render->img = mlx_new_image(render->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (!render->win || !render->img)
	{
		free(render);
		return (NULL);
	}
	render->img_data = mlx_get_data_addr(render->img, &render->bpp,
			&render->size_line, &render->endian);
	render->scene = scene;
	render->selection.type = OBJ_NONE;
	render->selection.index = 0;
	render->dirty = 1;
	render->low_quality = 0;
	render->shift_pressed = 0;
	if (hud_init(&render->hud, render->mlx, render->win) == -1)
	{
		free(render);
		return (NULL);
	}
	if (keyguide_init(&render->keyguide, render->mlx, render->win) == -1)
	{
		hud_cleanup(&render->hud, render->mlx);
		free(render);
		return (NULL);
	}
	render->hud.total_pages = hud_calculate_total_pages(scene);
	mlx_hook(render->win, 17, 0, close_window, render);
	mlx_hook(render->win, 2, 1L << 0, handle_key, render);
	mlx_hook(render->win, 3, 1L << 1, handle_key_release, render);
	mlx_loop_hook(render->mlx, render_loop, render);
	return (render);
}
