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
typedef struct s_render
{
	t_mlx_context		mlx;
	t_scene				*scene;
	t_selection			selection;
	int					state_flags;
	t_hud_state			hud;
	t_keyguide_state	keyguide;
	t_debounce_state	debounce;
	t_key_binds			key_binds;
}	t_render;

/* Render lifecycle */

/**
 * @brief Create and initialize a render context.
 *
 * Allocates the render context, sets up the MLX window and buffers,
 * initializes render state and UI components, and registers event hooks.
 *
 * @param scene Scene to render.
 * @return Newly created render context, or NULL on failure.
 */
t_render	*render_create(t_scene *scene);

/**
 * @brief Destroy a render context and release all resources.
 *
 * @param render Render context to destroy.
 */
void		render_destroy(t_render *render);

/* Render state flag helpers */

/**
 * @brief Check whether a render state flag is set.
 *
 * @param render Render context to query.
 * @param flag Flag bit to test.
 * @return 1 if the flag is set, 0 otherwise.
 */
int			render_has_flag(t_render *render, int flag);

/**
 * @brief Set a render state flag.
 *
 * @param render Render context to update.
 * @param flag Flag bit to set.
 */
void		render_set_flag(t_render *render, int flag);

/**
 * @brief Clear a render state flag.
 *
 * @param render Render context to update.
 * @param flag Flag bit to clear.
 */
void		render_clear_flag(t_render *render, int flag);

/* Render pipeline */

/**
 * @brief Render the full scene into the image buffer.
 *
 * Chooses low-quality or full-quality rendering based on render flags
 * and iterates over all pixels.
 *
 * @param scene Scene to render.
 * @param render Render context containing output buffer and state.
 */
void		render_scene_to_buffer(t_scene *scene, t_render *render);

/**
 * @brief Create a camera ray for normalized device coordinates.
 *
 * @param camera Camera describing the view.
 * @param x Normalized horizontal coordinate in [-1, 1].
 * @param y Normalized vertical coordinate in [-1, 1].
 * @return Ray originating at the camera and passing through (x, y).
 */
t_ray		create_camera_ray(t_camera *camera, double x, double y);

/**
 * @brief Trace a ray through the scene and compute its color.
 *
 * Uses BVH traversal when available, otherwise falls back to a
 * brute-force scan over all objects.
 *
 * @param scene Scene containing geometry and lighting.
 * @param ray Ray to trace.
 * @return Resulting color (black if no hit).
 */
t_color		trace_ray(t_scene *scene, t_ray *ray);

/**
 * @brief MLX loop hook for continuous rendering updates.
 *
 * Updates debounce state, rebuilds BVH if needed, renders when dirty,
 * and draws HUD overlays when requested.
 *
 * @param param Pointer to the render context.
 * @return Always 0 (as required by the MLX loop hook signature).
 */
int			render_loop(void *param);

#endif
