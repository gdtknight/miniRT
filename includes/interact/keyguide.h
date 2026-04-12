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

# include "interact/ui_types.h"

/* Key guide dimensions (maximum allowed per FR-008) */
# define KEYGUIDE_WIDTH 300
# define KEYGUIDE_HEIGHT 500

/* Positioning margins from window edges */
# define KEYGUIDE_MARGIN_X 20
# define KEYGUIDE_MARGIN_Y 20

/* Text layout spacing */
# define KEYGUIDE_LINE_HEIGHT 16
# define KEYGUIDE_SECTION_GAP 8
# define KEYGUIDE_CONTENT1_LINES 8

/* Text colors (0xRRGGBB format) */
# define KEYGUIDE_COLOR_TEXT 0xFFFFFF
# define KEYGUIDE_COLOR_HEADING 0x00FFFF
# define KEYGUIDE_COLOR_BG 0x000000

/* Transparency level (0.0=transparent, 1.0=opaque) */
# define KEYGUIDE_BG_ALPHA 0.65

/* Key guide state structure defined in window.h */

/**
 * @brief Initialize the key guide overlay state.
 *
 * Computes the on-screen position and sets initial visibility.
 *
 * @param keyguide Key guide state to initialize.
 * @return 0 on success.
 */
int		keyguide_init(t_keyguide_state *keyguide);

/**
 * @brief Render the full key guide overlay.
 *
 * Draws all sections if the key guide is visible and dirty.
 *
 * @param render Render context containing key guide state.
 */
void	keyguide_render(t_render *render);

/**
 * @brief Render extra key guide sections (resize, rotation, etc.).
 *
 * Displays resize, rotation, light, and HUD page controls.
 *
 * @param render Render context containing key guide state.
 * @param y Current y position (in/out) for text rendering.
 */
void	keyguide_render_extra(t_render *render, int *y);

#endif
