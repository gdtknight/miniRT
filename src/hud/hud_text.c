/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_text.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render.h"
#include "hud.h"

void	hud_mark_dirty(t_render *render)
{
	render->hud.dirty = 1;
}

void	hud_print_line(t_render *r, int *y, int color, char *text)
{
	mlx_string_put(r->mlx.mlx, r->mlx.win,
		HUD_MARGIN_X + 10, *y, color, text);
	*y += HUD_LINE_HEIGHT;
}

void	hud_print_vec3(t_render *render, int *y,
		char *label, t_vec3 vec)
{
	char	buf[128];
	int		i;

	i = 0;
	while (i < 8)
		buf[i++] = ' ';
	while (*label && i < 80)
		buf[i++] = *label++;
	buf[i] = '\0';
	hud_format_vec3(buf + i, vec);
	hud_print_line(render, y, HUD_COLOR_TEXT, buf);
}

int	hud_append(char *dst, const char *src, int max)
{
	int	i;

	i = 0;
	while (src[i] && i < max)
	{
		dst[i] = src[i];
		i++;
	}
	return (i);
}
