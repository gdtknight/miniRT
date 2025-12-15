/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/15 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINIRT_H
# define MINIRT_H

# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdio.h>
# include <math.h>
# include <stdbool.h>
# include "vec3.h"
# include "objects.h"
# include "ray.h"

# define EPSILON 0.0001
# define PI 3.14159265358979323846

typedef struct s_color_f
{
	double	r;
	double	g;
	double	b;
}	t_color_f;

typedef struct s_ambient
{
	double	ratio;
	t_color	color;
}	t_ambient;

typedef struct s_camera
{
	t_vec3	position;
	t_vec3	direction;
	double	fov;
}	t_camera;

typedef struct s_light
{
	t_vec3	position;
	double	brightness;
	t_color	color;
}	t_light;

typedef struct s_scene
{
	t_ambient	ambient;
	t_camera	camera;
	t_light		light;
	t_sphere	spheres[100];
	int			sphere_count;
	t_plane		planes[100];
	int			plane_count;
	t_cylinder	cylinders[100];
	int			cylinder_count;
	int			has_ambient;
	int			has_camera;
	int			has_light;
}	t_scene;

/* Error handling */
int		print_error(const char *message);

/* Memory cleanup */
void	cleanup_scene(t_scene *scene);
void	cleanup_render(void *render);
void	cleanup_all(t_scene *scene, void *render);

/* Rendering */
void	render_scene(t_scene *scene, void *mlx, void *win);
t_color	apply_lighting(t_scene *scene, t_hit *hit);

/* Intersections */
int		intersect_sphere(t_ray *ray, t_sphere *sphere, t_hit *hit);
int		intersect_plane(t_ray *ray, t_plane *plane, t_hit *hit);
int		intersect_cylinder(t_ray *ray, t_cylinder *cylinder, t_hit *hit);

#endif
