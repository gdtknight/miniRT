/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUD_H
# define HUD_H

# include "interact/ui_types.h"
# include "scene/scene.h"

/* Forward declaration */
typedef struct s_render	t_render;

/* HUD display dimensions and layout constants */
# define HUD_WIDTH 400
# define HUD_HEIGHT 600
# define HUD_MARGIN_X 10
# define HUD_MARGIN_Y 10
# define HUD_LINE_HEIGHT 18
# define HUD_OBJECTS_PER_PAGE 8

/* HUD color scheme (0xRRGGBB format) */
# define HUD_COLOR_TEXT 0xFFFFFF
# define HUD_COLOR_HIGHLIGHT 0x00FF00
# define HUD_COLOR_BG 0x000000
# define HUD_BG_ALPHA 0.65

/* ========================================================================== */
/*                            PUBLIC API FUNCTIONS                            */
/* ========================================================================== */

/**
 * @brief Initialize HUD state with default values.
 *
 * Sets visible=1, current_page=0, dirty=1.
 *
 * @param hud Pointer to HUD state structure to initialize.
 * @return 0 on success.
 */
int		hud_init(t_hud_state *hud);

/**
 * @brief Render HUD overlay on top of rendered scene.
 *
 * Returns immediately if the HUD is hidden. Otherwise draws all
 * sections and clears the dirty flag.
 *
 * @param render Render context containing scene data and HUD state.
 */
void	hud_render(t_render *render);

/**
 * @brief Toggle HUD visibility and sync key guide visibility.
 *
 * Marks the render as dirty to force a redraw.
 *
 * @param render Render context containing HUD and key guide state.
 */
void	hud_toggle(t_render *render);

/**
 * @brief Select the next object in the scene (Tab key navigation).
 *
 * Wraps around at the end of the object list and marks HUD dirty.
 *
 * @param render Render context with selection and scene data.
 */
void	hud_select_next(t_render *render);

/**
 * @brief Change HUD object list page.
 *
 * Increments or decrements the current page with wraparound.
 * No-op if total_pages <= 1.
 *
 * @param render Render context with HUD pagination state.
 * @param direction Positive for next page, negative for previous.
 */
void	hud_change_page(t_render *render, int direction);

/* ========================================================================== */
/*                          INTERNAL HELPER FUNCTIONS                         */
/*                      (Not part of public API contract)                     */
/* ========================================================================== */

/**
 * @brief Convert a global linear index to a selection (type, index).
 *
 * Used internally for Tab navigation logic.
 *
 * @param sel Output selection to populate.
 * @param idx Global object index.
 * @param scene Scene containing objects.
 */
void	hud_get_selection_from_global(t_selection *sel, int idx,
			t_scene *scene);

/**
 * @brief Format a floating-point value with two decimal places.
 *
 * Buffer must be at least 32 bytes.
 *
 * @param buffer Output buffer.
 * @param value Value to format.
 */
void	hud_format_float(char *buffer, double value);

/**
 * @brief Format a vec3 as "(x, y, z)" with two decimals each.
 *
 * Buffer must be at least 64 bytes.
 *
 * @param buffer Output buffer.
 * @param vec Vector to format.
 */
void	hud_format_vec3(char *buffer, t_vec3 vec);

/**
 * @brief Format a color as "R:.. G:.. B:..".
 *
 * Buffer must be at least 32 bytes.
 *
 * @param buffer Output buffer.
 * @param color Color to format.
 */
void	hud_format_color(char *buffer, t_color color);

/**
 * @brief Render the performance section of the HUD.
 *
 * Shows FPS, frame time, and BVH status. Called internally by
 * hud_render_content().
 *
 * @param render Render context for text output.
 * @param y Current y position (in/out) for line placement.
 */
void	hud_render_performance(t_render *render, int *y);

/**
 * @brief Render all HUD sections in order.
 *
 * Called internally by hud_render().
 *
 * @param render Render context containing HUD and scene.
 */
void	hud_render_content(t_render *render);

/* Scene and object rendering declared in hud_scene.h and hud_objects.h */

/**
 * @brief Calculate the number of object list pages for the HUD.
 *
 * @param scene Scene containing object list.
 * @return Total number of pages (minimum 1).
 */
int		hud_calculate_total_pages(t_scene *scene);

#endif
