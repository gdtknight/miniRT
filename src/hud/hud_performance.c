/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_performance.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/31 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/31 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"
#include "metrics.h"

int	mlx_string_put(void *mlx_ptr, void *win_ptr, int x, int y,
		int color, char *string);

static void	render_perf_header(t_render *render, int *y)
{
	mlx_string_put(render->mlx, render->win,
		HUD_MARGIN_X + 10, *y, HUD_COLOR_HIGHLIGHT, "=== Performance ===");
	*y += HUD_LINE_HEIGHT;
}

static void	render_perf_basic(t_render *render, int *y)
{
	t_metrics	*m;
	char		buf[64];
	char		line[128];
	int			i;
	int			j;

	m = &render->scene->render_state.metrics;
	hud_format_fps(buf, m->fps);
	i = 0;
	j = 0;
	while ("FPS: "[j])
		line[i++] = "FPS: "[j++];
	j = 0;
	while (buf[j])
		line[i++] = buf[j++];
	line[i] = '\0';
	mlx_string_put(render->mlx, render->win,
		HUD_MARGIN_X + 10, *y, HUD_COLOR_TEXT, line);
	*y += HUD_LINE_HEIGHT;
	hud_format_time_ms(buf, m->render_time_us);
	i = 0;
	j = 0;
	while ("Frame: "[j])
		line[i++] = "Frame: "[j++];
	j = 0;
	while (buf[j])
		line[i++] = buf[j++];
	j = 0;
	while ("ms"[j])
		line[i++] = "ms"[j++];
	line[i] = '\0';
	mlx_string_put(render->mlx, render->win,
		HUD_MARGIN_X + 10, *y, HUD_COLOR_TEXT, line);
	*y += HUD_LINE_HEIGHT;
	hud_format_bvh_status(buf, render->scene->render_state.bvh_enabled);
	i = 0;
	j = 0;
	while ("BVH: "[j])
		line[i++] = "BVH: "[j++];
	j = 0;
	while (buf[j])
		line[i++] = buf[j++];
	line[i] = '\0';
	mlx_string_put(render->mlx, render->win,
		HUD_MARGIN_X + 10, *y, HUD_COLOR_TEXT, line);
	*y += HUD_LINE_HEIGHT;
}

void	hud_render_performance(t_render *render, int *y)
{
	render_perf_header(render, y);
	render_perf_basic(render, y);
}
