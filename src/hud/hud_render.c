/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_render.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.h"
#include "hud.h"

int	mlx_string_put(void *mlx_ptr, void *win_ptr, int x, int y,
		int color, char *string);

void	hud_mark_dirty(t_render *render)
{
	render->hud.dirty = 1;
}

void	hud_render_background_row(t_render *render, int y)
{
	int	x;
	int	scene_color;
	int	blended;

	x = HUD_MARGIN_X;
	while (x < HUD_MARGIN_X + HUD_WIDTH)
	{
		scene_color = get_pixel(render->img_data, x, y,
				render->size_line, render->bpp);
		blended = blend_colors(scene_color, HUD_COLOR_BG, HUD_BG_ALPHA);
		set_pixel(render->img_data, x, y, blended,
			render->size_line, render->bpp);
		x++;
	}
}

void	hud_render_background(t_render *render)
{
	int	y;

	y = HUD_MARGIN_Y;
	while (y < HUD_MARGIN_Y + HUD_HEIGHT)
	{
		hud_render_background_row(render, y);
		y++;
	}
}

void	hud_render_camera(t_render *render, int *y)
{
	char	buf[128];
	int		i;

	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, "Camera:");
	*y += HUD_LINE_HEIGHT;
	i = 0;
	while (i < 8)
		buf[i++] = ' ';
	buf[i++] = 'p';
	buf[i++] = 'o';
	buf[i++] = 's';
	buf[i] = '\0';
	hud_format_vec3(buf + i, render->scene->camera.position);
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
	i = 0;
	while (i < 8)
		buf[i++] = ' ';
	buf[i++] = 'd';
	buf[i++] = 'i';
	buf[i++] = 'r';
	buf[i] = '\0';
	hud_format_vec3(buf + i, render->scene->camera.direction);
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
}

static void	render_camera_fov(t_render *render, int *y)
{
	char	buf[64];
	int		i;

	i = 0;
	while (i < 8)
		buf[i++] = ' ';
	buf[i++] = 'f';
	buf[i++] = 'o';
	buf[i++] = 'v';
	buf[i++] = ':';
	buf[i] = '\0';
	hud_format_float(buf + i, render->scene->camera.fov);
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
}

void	hud_render_ambient(t_render *render, int *y)
{
	char	buf[128];
	int		i;

	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, "Ambient:");
	*y += HUD_LINE_HEIGHT;
	i = 0;
	while (i < 8)
		buf[i++] = ' ';
	buf[i++] = 'r';
	buf[i++] = 'a';
	buf[i++] = 't';
	buf[i++] = 'i';
	buf[i++] = 'o';
	buf[i++] = ':';
	buf[i] = '\0';
	hud_format_float(buf + i, render->scene->ambient.ratio);
	while (buf[i])
		i++;
	buf[i++] = ' ';
	buf[i] = '\0';
	hud_format_color(buf + i, render->scene->ambient.color);
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
}

void	hud_render_light(t_render *render, int *y)
{
	char	buf[128];
	int		i;

	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, "Light:");
	*y += HUD_LINE_HEIGHT;
	i = 0;
	while (i < 8)
		buf[i++] = ' ';
	buf[i++] = 'p';
	buf[i++] = 'o';
	buf[i++] = 's';
	buf[i] = '\0';
	hud_format_vec3(buf + i, render->scene->light.position);
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
}

static void	render_light_bright(t_render *render, int *y)
{
	char	buf[128];
	int		i;

	i = 0;
	while (i < 8)
		buf[i++] = ' ';
	buf[i++] = 'b';
	buf[i++] = 'r';
	buf[i++] = 'i';
	buf[i++] = 'g';
	buf[i++] = 'h';
	buf[i++] = 't';
	buf[i++] = ':';
	buf[i] = '\0';
	hud_format_float(buf + i, render->scene->light.brightness);
	while (buf[i])
		i++;
	buf[i++] = ' ';
	buf[i] = '\0';
	hud_format_color(buf + i, render->scene->light.color);
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
}

static void	render_object_header(t_render *render, int *y)
{
	char	buf[64];
	int		i;

	i = 0;
	buf[i++] = '-';
	buf[i++] = '-';
	buf[i++] = '-';
	buf[i++] = ' ';
	buf[i++] = 'O';
	buf[i++] = 'b';
	buf[i++] = 'j';
	buf[i++] = 'e';
	buf[i++] = 'c';
	buf[i++] = 't';
	buf[i++] = 's';
	buf[i++] = ' ';
	buf[i++] = '(';
	buf[i++] = 'P';
	buf[i++] = 'a';
	buf[i++] = 'g';
	buf[i++] = 'e';
	buf[i++] = ' ';
	buf[i++] = '0' + render->hud.current_page + 1;
	buf[i++] = '/';
	buf[i++] = '0' + render->hud.total_pages;
	buf[i++] = ')';
	buf[i++] = ' ';
	buf[i++] = '-';
	buf[i++] = '-';
	buf[i++] = '-';
	buf[i] = '\0';
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
}

static void	render_sphere_obj(t_render *render, int idx, int *y, int color)
{
	char	buf[128];
	int		i;

	i = 0;
	buf[i++] = 'S';
	buf[i++] = 'p';
	buf[i++] = 'h';
	buf[i++] = 'e';
	buf[i++] = 'r';
	buf[i++] = 'e';
	buf[i++] = ' ';
	buf[i++] = '#';
	buf[i++] = '0' + idx + 1;
	buf[i++] = ':';
	buf[i++] = ' ';
	buf[i++] = 'p';
	buf[i++] = 'o';
	buf[i++] = 's';
	buf[i] = '\0';
	hud_format_vec3(buf + i, render->scene->spheres[idx].center);
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, color, buf);
	*y += HUD_LINE_HEIGHT;
}

static void	render_plane_obj(t_render *render, int idx, int *y, int color)
{
	char	buf[128];
	int		i;

	i = 0;
	buf[i++] = 'P';
	buf[i++] = 'l';
	buf[i++] = 'a';
	buf[i++] = 'n';
	buf[i++] = 'e';
	buf[i++] = ' ';
	buf[i++] = '#';
	buf[i++] = '0' + idx + 1;
	buf[i++] = ':';
	buf[i++] = ' ';
	buf[i++] = 'p';
	buf[i++] = 'o';
	buf[i++] = 's';
	buf[i] = '\0';
	hud_format_vec3(buf + i, render->scene->planes[idx].point);
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, color, buf);
	*y += HUD_LINE_HEIGHT;
}

static void	render_cylinder_obj(t_render *render, int idx, int *y, int color)
{
	char	buf[128];
	int		i;

	i = 0;
	buf[i++] = 'C';
	buf[i++] = 'y';
	buf[i++] = 'l';
	buf[i++] = 'i';
	buf[i++] = 'n';
	buf[i++] = 'd';
	buf[i++] = 'e';
	buf[i++] = 'r';
	buf[i++] = ' ';
	buf[i++] = '#';
	buf[i++] = '0' + idx + 1;
	buf[i++] = ':';
	buf[i++] = ' ';
	buf[i++] = 'p';
	buf[i++] = 'o';
	buf[i++] = 's';
	buf[i] = '\0';
	hud_format_vec3(buf + i, render->scene->cylinders[idx].center);
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, color, buf);
	*y += HUD_LINE_HEIGHT;
}

static void	render_object_by_index(t_render *render, int g_idx, int *y)
{
	t_selection	sel;
	int			color;

	sel = hud_get_selection_from_global(g_idx, render->scene);
	color = HUD_COLOR_TEXT;
	if (sel.type == render->selection.type
		&& sel.index == render->selection.index)
		color = HUD_COLOR_HIGHLIGHT;
	if (sel.type == OBJ_SPHERE)
		render_sphere_obj(render, sel.index, y, color);
	else if (sel.type == OBJ_PLANE)
		render_plane_obj(render, sel.index, y, color);
	else if (sel.type == OBJ_CYLINDER)
		render_cylinder_obj(render, sel.index, y, color);
}

void	hud_render_objects(t_render *render, int *y)
{
	int	total_objects;
	int	start_idx;
	int	end_idx;
	int	i;

	total_objects = render->scene->sphere_count + render->scene->plane_count
		+ render->scene->cylinder_count;
	render->hud.total_pages = hud_calculate_total_pages(render->scene);
	render_object_header(render, y);
	if (total_objects == 0)
		return ;
	start_idx = render->hud.current_page * HUD_OBJECTS_PER_PAGE;
	end_idx = start_idx + HUD_OBJECTS_PER_PAGE;
	if (end_idx > total_objects)
		end_idx = total_objects;
	i = start_idx;
	while (i < end_idx)
	{
		render_object_by_index(render, i, y);
		i++;
	}
}

void	hud_render_content(t_render *render)
{
	int	y;

	y = HUD_MARGIN_Y + 20;
	hud_render_camera(render, &y);
	render_camera_fov(render, &y);
	y += HUD_LINE_HEIGHT / 2;
	hud_render_ambient(render, &y);
	y += HUD_LINE_HEIGHT / 2;
	hud_render_light(render, &y);
	render_light_bright(render, &y);
	y += HUD_LINE_HEIGHT;
	hud_render_objects(render, &y);
}

void	hud_render(t_render *render)
{
	if (!render->hud.visible)
		return ;
	hud_render_background(render);
	hud_render_content(render);
	render->hud.dirty = 0;
}
