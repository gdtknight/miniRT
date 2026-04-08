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

#include "render/render.h"
#include "interact/hud.h"
#include "interact/hud_internal.h"
#include "metrics.h"

/**
 * @brief Render FPS, frame time, and BVH status lines.
 *
 * @param render Render context with metrics and MLX state.
 * @param y Pointer to current vertical draw position.
 */
static void	render_perf_basic(t_render *render, int *y)
{
	t_metrics	*m;
	char		buf[64];
	char		line[128];
	int			len;

	m = &render->scene->metrics;
	len = hud_append(line, "FPS: ", 128);
	hud_format_float(buf, m->timing.fps);
	len += hud_append(line + len, buf, 128 - len);
	line[len] = '\0';
	hud_print_line(render, y, HUD_COLOR_TEXT, line);
	len = hud_append(line, "Frame: ", 128);
	hud_format_float(buf, m->timing.render_time_us / 1000.0);
	len += hud_append(line + len, buf, 128 - len);
	len += hud_append(line + len, "ms", 128 - len);
	line[len] = '\0';
	hud_print_line(render, y, HUD_COLOR_TEXT, line);
	len = hud_append(line, "BVH: ", 128);
	if ((render->scene->flags & SCENE_BVH_ENABLED) != 0)
		len += hud_append(line + len, "ON", 128 - len);
	else
		len += hud_append(line + len, "OFF", 128 - len);
	line[len] = '\0';
	hud_print_line(render, y, HUD_COLOR_TEXT, line);
}

/**
 * @brief Render the performance section of the HUD.
 *
 * @param render Render context for text output.
 * @param y Current y position (in/out) for line placement.
 */
void	hud_render_performance(t_render *render, int *y)
{
	hud_print_line(render, y, HUD_COLOR_HIGHLIGHT, "=== Performance ===");
	render_perf_basic(render, y);
}
