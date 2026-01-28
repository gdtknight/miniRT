/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   overlay.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OVERLAY_H
# define OVERLAY_H

# include "mlx_context.h"

/**
 * @brief HUD-specific data (non-image related)
 */
typedef struct s_hud_data
{
	int	visible;
	int	current_page;
	int	objects_per_page;
	int	total_pages;
	int	dirty;
}	t_hud_data;

/**
 * @brief Key guide-specific data (non-image related)
 */
typedef struct s_keyguide_data
{
	int	visible;
	int	x;
	int	y;
}	t_keyguide_data;

/**
 * @brief HUD overlay structure
 */
typedef struct s_hud_overlay
{
	t_mlx_img	img;
	t_hud_data	data;
}	t_hud_overlay;

/**
 * @brief Key guide overlay structure
 */
typedef struct s_keyguide_overlay
{
	t_mlx_img		img;
	t_keyguide_data	data;
}	t_keyguide_overlay;

/* Overlay operations */
int		hud_overlay_init(t_hud_overlay *hud, void *mlx, int w, int h);
void	hud_overlay_destroy(t_hud_overlay *hud, void *mlx);
void	hud_overlay_toggle(t_hud_overlay *hud);
void	hud_overlay_next_page(t_hud_overlay *hud);
void	hud_overlay_prev_page(t_hud_overlay *hud);

int		keyguide_overlay_init(t_keyguide_overlay *kg, void *mlx, int w, int h);
void	keyguide_overlay_destroy(t_keyguide_overlay *kg, void *mlx);
void	keyguide_overlay_toggle(t_keyguide_overlay *kg);

#endif
