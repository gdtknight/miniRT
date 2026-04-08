/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:50 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RENDER_H
# define RENDER_H

# include "scene/scene.h"
# include "common/ray.h"
# include "render/window.h"
# include "render/render_debounce.h"
# include "interact/ui_types.h"
# include "interact/key_binds.h"

/* Render state flags (bit flags) */
# define RENDER_DIRTY       0x01
# define RENDER_RENDERING   0x02
# define RENDER_LOW_QUALITY 0x04
/* 0x08 and 0x10 reserved for future use */
# define RENDER_BVH_DIRTY  0x20
# define RENDER_ENABLE_METRICS_PRINT 0x80

/* Render context */
struct s_render
{
	t_mlx_context		mlx;
	t_scene				*scene;
	t_selection			selection;
	int					state_flags;
	t_hud_state			hud;
	t_keyguide_state	keyguide;
	t_debounce_state	debounce;
	t_key_binds			key_binds;
};

/* Render lifecycle */
t_render	*render_create(t_scene *scene);
void		render_destroy(t_render *render);

/* Render state flag helpers */
int			render_has_flag(t_render *render, int flag);
void		render_set_flag(t_render *render, int flag);
void		render_clear_flag(t_render *render, int flag);

/* Render pipeline */
void		render_scene_to_buffer(t_scene *scene, t_render *render);
t_ray		create_camera_ray(t_camera *camera, double x, double y);
t_color		trace_ray(t_scene *scene, t_ray *ray);
int			render_loop(void *param);

#endif
