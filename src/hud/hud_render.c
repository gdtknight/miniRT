/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_render.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.h"
#include "hud.h"
#include "hud_text.h"
#include "hud_scene.h"
#include "hud_objects.h"

/**
 * @brief hud render content 함수 - 렌더링 수행
 *
 * @param render 파라미터
 */
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
	y += HUD_LINE_HEIGHT;
	hud_render_performance(render, &y);
}

/**
 * @brief hud render 함수 - 렌더링 수행
 *
 * @param render 파라미터
 */
void	hud_render(t_render *render)
{
	if (!render->hud.visible)
		return ;
	hud_render_content(render);
	render->hud.dirty = 0;
}
