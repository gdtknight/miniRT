/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_internal.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/03/24 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUD_INTERNAL_H
# define HUD_INTERNAL_H

# include "scene.h"

/* HUD text utilities */
void	hud_mark_dirty(t_render *render);
void	hud_print_line(t_render *r, int *y, int color, char *text);
void	hud_print_vec3(t_render *render, int *y,
			char *label, t_vec3 vec);
int		hud_append(char *dst, const char *src, int max);

/* HUD scene info sections */
void	hud_render_camera(t_render *render, int *y);
void	hud_render_camera_fov(t_render *render, int *y);
void	hud_render_ambient(t_render *render, int *y);
void	hud_render_light(t_render *render, int *y);
void	hud_render_light_bright(t_render *render, int *y);

/* HUD object list */
void	hud_render_objects(t_render *render, int *y);
void	hud_render_object_entry(t_render *render, int idx, int *y, int color);

/* HUD format helpers */
void	hud_strcpy(char *dst, const char *src);
void	hud_itoa_buf(char *buf, int n);

#endif
