/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:17:55 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINIRT_H
# define MINIRT_H

# include <unistd.h>
# include <fcntl.h>
# include <stdbool.h>
# include "error.h"
# include "vec3.h"
# include "objects.h"
# include "ray.h"
# include "shadow.h"
# include "metrics.h"

/* Forward declarations */
typedef struct s_render	t_render;
typedef struct s_bvh	t_bvh;

/* Scene flags (bit flags) */
# define SCENE_HAS_AMBIENT  0x01
# define SCENE_HAS_CAMERA   0x02
# define SCENE_HAS_LIGHT    0x04
# define SCENE_BVH_ENABLED  0x08

/* Default object capacity */
# define DEFAULT_OBJECT_CAPACITY 32

/* Epsilon value for floating point comparison */
# define EPSILON 0.0001

/* Helper structure for camera ray calculations */
typedef struct s_cam_calc
{
	double	aspect_ratio;
	double	fov_scale;
	t_vec3	right;
	t_vec3	up;
}	t_cam_calc;

/* Floating point color for intermediate calculations (0.0 - 1.0+) */
typedef struct s_color_f
{
	double	r;
	double	g;
	double	b;
}	t_color_f;

/* Ambient lighting that illuminates all objects uniformly */
typedef struct s_ambient
{
	double	ratio;
	t_color	color;
}	t_ambient;

/* Camera defines viewpoint and field of view for rendering */
typedef struct s_camera
{
	t_vec3	position;
	t_vec3	direction;
	t_vec3	initial_position;
	t_vec3	initial_direction;
	double	fov;
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
	t_light			light;
	t_shadow_config	shadow_config;
	t_object_list	objects;
	int				flags;
	t_bvh			*bvh;
	t_metrics		metrics;
}	t_scene;

/* Scene lifecycle */
t_scene	*scene_create(void);
void	scene_destroy(t_scene *scene);

/* Object list operations */
int		object_list_init(t_object_list *list, int capacity);
void	object_list_destroy(t_object_list *list);
int		object_list_add(t_object_list *list, t_object *obj);
int		object_list_grow(t_object_list *list);

/* Scene flag helpers */
int		scene_has_ambient(t_scene *scene);
int		scene_has_camera(t_scene *scene);
int		scene_has_light(t_scene *scene);
void	scene_set_flag(t_scene *scene, int flag);
void	scene_clear_flag(t_scene *scene, int flag);

/* Cleanup */
void	cleanup_all(t_scene *scene, void *param);

/* Utility functions */
int		ft_atoi(const char *str);
double	ft_atof(const char *str);

/* Rendering */
void	render_scene_to_buffer(t_scene *scene, t_render *render);
t_color	apply_lighting(t_scene *scene, t_hit *hit);
t_ray	create_camera_ray(t_camera *camera, double x, double y);
t_color	trace_ray(t_scene *scene, t_ray *ray);

#endif
