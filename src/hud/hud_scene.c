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
 * @brief hud render camera 함수 - 렌더링 수행
 *
 * @param render 파라미터
 * @param y 파라미터
 */
void	hud_render_camera(t_render *render, int *y)
{
	mlx_string_put(render->mlx.mlx, render->mlx.win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, "Camera:");
	*y += HUD_LINE_HEIGHT;
	format_and_print_vec3(render, y, "pos", render->scene->camera.position);
	format_and_print_vec3(render, y, "dir", render->scene->camera.direction);
}

/**
 * @brief render camera fov 함수 - 렌더링 수행
 *
 * @param render 파라미터
 * @param y 파라미터
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
	mlx_string_put(render->mlx.mlx, render->mlx.win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
}

/**
 * @brief hud render ambient 함수 - 렌더링 수행
 *
 * @param render 파라미터
 * @param y 파라미터
 */
void	hud_render_ambient(t_render *render, int *y)
{
	char	buf[128];
	int		i;

	mlx_string_put(render->mlx.mlx, render->mlx.win, HUD_MARGIN_X + 10,
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
	mlx_string_put(render->mlx.mlx, render->mlx.win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
}

/**
 * @brief hud render light 함수 - 렌더링 수행
 *
 * @param render 파라미터
 * @param y 파라미터
 */
void	hud_render_light(t_render *render, int *y)
{
	char	buf[128];
	int		i;

	mlx_string_put(render->mlx.mlx, render->mlx.win, HUD_MARGIN_X + 10,
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
	mlx_string_put(render->mlx.mlx, render->mlx.win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
}

/**
 * @brief render light bright 함수 - 렌더링 수행
 *
 * @param render 파라미터
 * @param y 파라미터
 */
void	render_light_bright(t_render *render, int *y)
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
	mlx_string_put(render->mlx.mlx, render->mlx.win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
}
