/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:50 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WINDOW_H
# define WINDOW_H

# include "minirt.h"
# include "mlx.h"
# include "mlx_context.h"
# include "pixel_timing.h"
# include "render_debounce.h"

/* Window resolution constants */
# define WINDOW_WIDTH 1440
# define WINDOW_HEIGHT 900
# define ASPECT_RATIO_NUM 1440.0
# define ASPECT_RATIO_DEN 900.0

/* Render state flags (bit flags) */
# define RENDER_DIRTY       0x01
# define RENDER_RENDERING   0x02
# define RENDER_LOW_QUALITY 0x04
# define RENDER_SHIFT_HELD  0x08
# define RENDER_SHOW_INFO  0x10
# define RENDER_BVH_DIRTY  0x20

/* Selected object information */
typedef struct s_selection
{
	t_object_type	type;
	int				index;
}	t_selection;

/* HUD state structure */
typedef struct s_hud_state
{
	int		visible;
	int		current_page;
	int		objects_per_page;
	int		total_pages;
	int		dirty;
}	t_hud_state;

/* Key guide state structure */
typedef struct s_keyguide_state
{
	int		visible;
	int		x;
	int		y;
}	t_keyguide_state;

/* Render context (refactored with legacy compatibility) */
typedef struct s_render
{
	t_mlx_context		mlx;
	t_scene				*scene;
	t_selection			selection;
	int					state_flags;
	t_hud_state			hud;
	t_keyguide_state	keyguide;
	t_pixel_timing		pixel_timing;
	t_debounce_state	debounce;
}	t_render;

/* Render lifecycle */
t_render	*render_create(t_scene *scene);
void		render_destroy(t_render *render);

/* Render state flag helpers */
int			render_has_flag(t_render *render, int flag);
void		render_set_flag(t_render *render, int flag);
void		render_clear_flag(t_render *render, int flag);

/* Window event handlers */
int			close_window(void *param);
int			handle_key(int keycode, void *param);
int			handle_key_release(int keycode, void *param);
int			render_loop(void *param);

#endif
