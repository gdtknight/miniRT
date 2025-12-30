/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:17:55 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:21:25 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINIRT_H
# define MINIRT_H

# include <unistd.h>
# include <fcntl.h>
# include <stdbool.h>
# include "vec3.h"
# include "objects.h"
# include "ray.h"
# include "shadow.h"
# include "render_state.h"

/* Forward declarations */
typedef struct s_render	t_render;

/* Epsilon value for floating point comparison to avoid numerical errors */
# define EPSILON 0.0001

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
	double	fov;
}	t_camera;

/* Point light source emitting from a single point in all directions */
typedef struct s_light
{
	t_vec3	position;
	double	brightness;
	t_color	color;
}	t_light;

/* Complete scene: ambient light, camera, light, and geometric objects */
typedef struct s_scene
{
	t_ambient		ambient;
	t_camera		camera;
	t_light			light;
	t_shadow_config	shadow_config;
	t_sphere		spheres[100];
	int				sphere_count;
	t_plane			planes[100];
	int				plane_count;
	t_cylinder		cylinders[100];
	int				cylinder_count;
	int				has_ambient;
	int				has_camera;
	int				has_light;
	t_render_state	render_state;
}	t_scene;

/* Print error message to stderr and return 0 */
int		print_error(const char *message);

/* Convert string to integer */
int		ft_atoi(const char *str);
/* Convert string to double */
double	ft_atof(const char *str);

/* Free allocated memory for scene structure */
void	cleanup_scene(t_scene *scene);
/* Free allocated memory for render structure */
void	cleanup_render(void *render);
/* Free all allocated memory (scene and render) */
void	cleanup_all(t_scene *scene, void *render);

/* Render entire scene by iterating over all pixels.
** Resolution hardcoded to 800x600.
** Each pixel is ray traced independently.
** Uses image buffer for fast rendering.
** Supports low quality mode (half resolution) for interactive preview.
*/
void	render_scene(t_scene *scene, void *mlx, void *win);
void	render_scene_to_buffer(t_scene *scene, t_render *render);
/* Calculate final color at hit point using lighting and shadows */
t_color	apply_lighting(t_scene *scene, t_hit *hit);

/* Test ray-sphere intersection and update hit info if closer */
int		intersect_sphere(t_ray *ray, t_sphere *sphere, t_hit *hit);
/* Test ray-plane intersection and update hit info if closer */
int		intersect_plane(t_ray *ray, t_plane *plane, t_hit *hit);
/* Test ray-cylinder intersection and update hit info if closer */
int		intersect_cylinder(t_ray *ray, t_cylinder *cylinder, t_hit *hit);

#endif
