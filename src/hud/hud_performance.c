/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_performance.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/31 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"
#include "metrics.h"

/**
 * @brief Render the performance section header.
 *
 * @param render Render context with MLX handles.
 * @param y Current y position (in/out).
 */
static void	render_perf_header(t_render *render, int *y)
{
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		HUD_MARGIN_X + 10, *y, HUD_COLOR_HIGHLIGHT, "=== Performance ===");
	*y += HUD_LINE_HEIGHT;
}

/**
 * @brief Copy a string into a buffer with a max length.
 *
 * @param dst Destination buffer.
 * @param src Source string.
 * @param max_len Maximum buffer length.
 * @return int Number of characters copied.
 */
static int	copy_str(char *dst, char *src, int max_len)
{
	int	i;

	i = 0;
	while (src[i] && i < max_len - 1)
	{
		dst[i] = src[i];
		i++;
	}
	return (i);
}

/**
 * @brief Concatenate prefix/value/suffix and render a line.
 *
 * @param render Render context with MLX handles.
 * @param params Text components and y position.
 */
static void	concat_and_print(t_render *render, t_perf_text *params)
{
	char	line[128];
	int		len;

	len = copy_str(line, params->prefix, 128);
	len += copy_str(line + len, params->value, 128 - len);
	if (params->suffix)
		len += copy_str(line + len, params->suffix, 128 - len);
	line[len] = '\0';
	mlx_string_put(render->mlx.mlx, render->mlx.win,
		HUD_MARGIN_X + 10, *params->y, HUD_COLOR_TEXT, line);
	*params->y += HUD_LINE_HEIGHT;
}

/**
 * @brief Render basic performance metrics (FPS, frame time, BVH state).
 *
 * @param render Render context containing metrics.
 * @param y Current y position (in/out).
 */
static void	render_perf_basic(t_render *render, int *y)
{
	t_metrics	*m;
	char		buf[64];
	t_perf_text	params;

	m = &render->scene->metrics;
	params.y = y;
	hud_format_fps(buf, m->timing.fps);
	params.prefix = "FPS: ";
	params.value = buf;
	params.suffix = NULL;
	concat_and_print(render, &params);
	hud_format_time_ms(buf, m->timing.render_time_us);
	params.prefix = "Frame: ";
	params.value = buf;
	params.suffix = "ms";
	concat_and_print(render, &params);
	hud_format_bvh_status(buf, (render->scene->flags & SCENE_BVH_ENABLED) != 0);
	params.prefix = "BVH: ";
	params.value = buf;
	params.suffix = NULL;
	concat_and_print(render, &params);
}

/**
 * @brief Render the full performance section in the HUD.
 *
 * @param render Render context containing metrics.
 * @param y Current y position (in/out).
 */
void	hud_render_performance(t_render *render, int *y)
{
	render_perf_header(render, y);
	render_perf_basic(render, y);
}
