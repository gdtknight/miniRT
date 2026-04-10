/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:17:55 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCENE_H
# define SCENE_H

# include "common/vec3.h"
# include "common/objects.h"
# include "metrics.h"

/* Forward declarations */
typedef struct s_render	t_render;
typedef struct s_bvh	t_bvh;

/* Shadow configuration (owned by scene, used by lighting) */
typedef struct s_shadow_config
{
	int		samples;
	double	softness;
	t_vec3	*offset_lut;
}	t_shadow_config;

/* Scene flags (bit flags) */
# define SCENE_HAS_AMBIENT  0x01
# define SCENE_HAS_CAMERA   0x02
# define SCENE_BVH_ENABLED  0x08

/* Default object capacity */
# define DEFAULT_OBJECT_CAPACITY 32

/* Maximum number of light sources */
# define MAX_LIGHTS 16

/* Camera basis cache with dirty flag for per-frame caching */
typedef struct s_camera_cache
{
	t_vec3	right;
	t_vec3	up;
	double	aspect_ratio;
	double	fov_scale;
	int		valid;
}	t_camera_cache;

/* Ambient lighting that illuminates all objects uniformly */
typedef struct s_ambient
{
	double	ratio;
	t_color	color;
}	t_ambient;

/* Camera defines viewpoint and field of view for rendering */
typedef struct s_camera
{
	t_vec3			position;
	t_vec3			direction;
	t_vec3			initial_position;
	t_vec3			initial_direction;
	double			fov;
	double			pitch;
	double			yaw;
	t_camera_cache	cache;
}	t_camera;

/* Point light source emitting from a single point in all directions */
typedef struct s_light
{
	t_vec3	position;
	double	brightness;
	t_color	color;
}	t_light;

/* Object storage with dynamic array */
typedef struct s_object_list
{
	t_object	*items;
	int			count;
	int			capacity;
}	t_object_list;

/* Complete scene structure (refactored) */
typedef struct s_scene
{
	t_ambient		ambient;
	t_camera		camera;
	t_light			lights[MAX_LIGHTS];
	int				light_count;
	int				selected_light;
	t_shadow_config	shadow_config;
	t_object_list	objects;
	int				flags;
	t_bvh			*bvh;
	t_metrics		metrics;
}	t_scene;

/* Scene lifecycle */

/**
 * @brief Allocate and initialize a new scene.
 *
 * Initializes object list capacity and default settings.
 *
 * @return Newly created scene, or NULL on failure.
 */
t_scene	*scene_create(void);

/**
 * @brief Destroy a scene and free all associated resources.
 *
 * Releases object list storage, BVH data, and the scene structure.
 *
 * @param scene Scene to destroy.
 */
void	scene_destroy(t_scene *scene);

/* Object list operations */

/**
 * @brief Initialize an object list with a given capacity.
 *
 * @param list Object list to initialize.
 * @param capacity Initial capacity (must be > 0).
 * @return 1 on success, 0 on failure.
 */
int		object_list_init(t_object_list *list, int capacity);

/**
 * @brief Free object list storage and reset counters.
 *
 * @param list Object list to destroy.
 */
void	object_list_destroy(t_object_list *list);

/**
 * @brief Append an object to the list, growing storage if needed.
 *
 * @param list Object list to update.
 * @param obj Object to append.
 * @return 1 on success, 0 on failure.
 */
int		object_list_add(t_object_list *list, t_object *obj);

#endif
