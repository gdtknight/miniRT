/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyguide.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/30 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KEYGUIDE_H
# define KEYGUIDE_H

# include "minirt.h"
# include "window.h"

/* Key guide dimensions (maximum allowed per FR-008) */
# define KEYGUIDE_WIDTH 300
# define KEYGUIDE_HEIGHT 400

/* Positioning margins from window edges */
# define KEYGUIDE_MARGIN_X 20
# define KEYGUIDE_MARGIN_Y 20

/* Text layout spacing */
# define KEYGUIDE_LINE_HEIGHT 16
# define KEYGUIDE_SECTION_GAP 8

/* Text colors (0xRRGGBB format) */
# define KEYGUIDE_COLOR_TEXT 0xFFFFFF
# define KEYGUIDE_COLOR_HEADING 0x00FFFF
# define KEYGUIDE_COLOR_BG 0x000000

/* Transparency level (0.0=transparent, 1.0=opaque) */
# define KEYGUIDE_BG_ALPHA 0.65

/* Key guide state structure defined in window.h */

int		keyguide_init(t_keyguide_state *keyguide, void *mlx, void *win);
void	keyguide_cleanup(t_keyguide_state *keyguide, void *mlx);
void	keyguide_render(t_render *render);
void	keyguide_render_background(t_render *render);
void	keyguide_render_content(t_render *render);

#endif
