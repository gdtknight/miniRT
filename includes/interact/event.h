/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/04/08 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_H
# define EVENT_H

/* MLX event callbacks */

/**
 * @brief Handle window close event and shut down cleanly.
 *
 * @param param Pointer to the render context.
 * @return Always returns 0 for MLX event handling.
 */
int		close_window(void *param);

/**
 * @brief Handle key press events via dispatch table lookup.
 *
 * @param keycode Key code of the pressed key.
 * @param param Pointer to the render context.
 * @return Always returns 0 for MLX event handling.
 */
int		handle_key(int keycode, void *param);

/**
 * @brief Handle window expose events by presenting the framebuffer.
 *
 * @param param Pointer to the render context.
 * @return Always returns 0 for MLX event handling.
 */
int		handle_expose(void *param);

#endif
