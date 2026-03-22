/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_scene.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.h"
#include "hud.h"
#include "hud_text.h"

/**
 * @brief Render camera section in the HUD.
 *
 * Displays camera position and direction.
 *
 * @param render Render context containing scene.
 * @param y Current y position (in/out).
 */
void	hud_render_camera(t_render *render, int *y)
{
	hud_print_line(render, y, HUD_COLOR_TEXT, "Camera:");
	format_and_print_vec3(render, y, "pos", render->scene->camera.position);
	format_and_print_vec3(render, y, "dir", render->scene->camera.direction);
}

/**
 * @brief Render camera field-of-view in the HUD.
 *
 * @param render Render context containing scene.
 * @param y Current y position (in/out).
 */
void	render_camera_fov(t_render *render, int *y)
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
	hud_print_line(render, y, HUD_COLOR_TEXT, buf);
}

/**
 * @brief Render ambient light section in the HUD.
 *
 * @param render Render context containing scene.
 * @param y Current y position (in/out).
 */
void	hud_render_ambient(t_render *render, int *y)
{
	char	buf[128];
	int		i;

	hud_print_line(render, y, HUD_COLOR_TEXT, "Ambient:");
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
	hud_print_line(render, y, HUD_COLOR_TEXT, buf);
}

/**
 * @brief Render light position section in the HUD.
 *
 * @param render Render context containing scene.
 * @param y Current y position (in/out).
 */
void	hud_render_light(t_render *render, int *y)
{
	char	buf[128];
	int		i;

	if (render->scene->light_count == 0)
		return ;
	if (render->scene->selected_light >= render->scene->light_count)
		return ;
	i = 0;
	buf[i++] = 'L';
	ft_itoa_buf(buf + i, render->scene->selected_light + 1);
	while (buf[i])
		i++;
	buf[i++] = '/';
	ft_itoa_buf(buf + i, render->scene->light_count);
	while (buf[i])
		i++;
	buf[i++] = ':';
	buf[i] = '\0';
	hud_print_line(render, y, HUD_COLOR_TEXT, buf);
	format_and_print_vec3(render, y, "pos",
		render->scene->lights[render->scene->selected_light].position);
}

/**
 * @brief Render light brightness and color section in the HUD.
 *
 * @param render Render context containing scene.
 * @param y Current y position (in/out).
 */
void	render_light_bright(t_render *render, int *y)
{
	char	buf[128];
	int		i;

	if (render->scene->light_count == 0
		|| render->scene->selected_light >= render->scene->light_count)
		return ;
	ft_memset(buf, ' ', 8);
	ft_memcpy(buf + 8, "bright:", 7);
	i = 15;
	buf[i] = '\0';
	hud_format_float(buf + i,
		render->scene->lights[render->scene->selected_light].brightness);
	while (buf[i])
		i++;
	buf[i++] = ' ';
	buf[i] = '\0';
	hud_format_color(buf + i,
		render->scene->lights[render->scene->selected_light].color);
	hud_print_line(render, y, HUD_COLOR_TEXT, buf);
}
