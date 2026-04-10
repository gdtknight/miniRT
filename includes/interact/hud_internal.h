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

# include "common/vec3.h"

typedef struct s_render	t_render;

/* HUD text utilities */

/**
 * @brief Mark the HUD as needing a redraw.
 *
 * @param render Render context owning the HUD state.
 */
void	hud_mark_dirty(t_render *render);

/**
 * @brief Draw a single text line on the HUD and advance y.
 *
 * @param r Render context for MLX output.
 * @param y Current y position (in/out).
 * @param color Text color value.
 * @param text String to display.
 */
void	hud_print_line(t_render *r, int *y, int color, char *text);

/**
 * @brief Print a labeled vec3 value on the HUD.
 *
 * @param render Render context for text output.
 * @param y Current y position (in/out).
 * @param label Text label preceding the vector.
 * @param vec Vector to format and display.
 */
void	hud_print_vec3(t_render *render, int *y,
			char *label, t_vec3 vec);

/**
 * @brief Copy src into dst up to max chars without null-terminating.
 *
 * @param dst Destination buffer.
 * @param src Source string.
 * @param max Maximum characters to copy.
 * @return Number of characters copied.
 */
int		hud_append(char *dst, const char *src, int max);

/* HUD scene info sections */

/**
 * @brief Render camera position and direction in the HUD.
 *
 * @param render Render context containing scene.
 * @param y Current y position (in/out).
 */
void	hud_render_camera(t_render *render, int *y);

/**
 * @brief Render camera field-of-view in the HUD.
 *
 * @param render Render context containing scene.
 * @param y Current y position (in/out).
 */
void	hud_render_camera_fov(t_render *render, int *y);

/**
 * @brief Render ambient light section in the HUD.
 *
 * @param render Render context containing scene.
 * @param y Current y position (in/out).
 */
void	hud_render_ambient(t_render *render, int *y);

/**
 * @brief Render light position section in the HUD.
 *
 * @param render Render context containing scene.
 * @param y Current y position (in/out).
 */
void	hud_render_light(t_render *render, int *y);

/**
 * @brief Render light brightness and color section in the HUD.
 *
 * @param render Render context containing scene.
 * @param y Current y position (in/out).
 */
void	hud_render_light_bright(t_render *render, int *y);

/* HUD object list */

/**
 * @brief Render the HUD object list for the current page.
 *
 * @param render Render context containing HUD and scene.
 * @param y Current y position (in/out).
 */
void	hud_render_objects(t_render *render, int *y);

/**
 * @brief Render a single object entry in the HUD list.
 *
 * Formats the object ID and position and renders it at the
 * current y with the specified color.
 *
 * @param render Render context containing scene and MLX handles.
 * @param idx Object index in the scene list.
 * @param y Current y position (in/out).
 * @param color Text color for the entry.
 */
void	hud_render_object_entry(t_render *render, int idx, int *y, int color);

/* HUD format helpers */

/**
 * @brief Copy a string into a destination buffer.
 *
 * Uses ft_strlcpy to ensure the destination is null-terminated.
 *
 * @param dst Destination buffer.
 * @param src Source string.
 */
void	hud_strcpy(char *dst, const char *src);

/**
 * @brief Convert an integer to string and write into a buffer.
 *
 * Writes "0" on allocation failure.
 *
 * @param buf Destination buffer.
 * @param n Integer to format.
 */
void	hud_itoa_buf(char *buf, int n);

#endif
