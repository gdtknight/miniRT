/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   objects.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:18 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OBJECTS_H
# define OBJECTS_H

# include "vec3.h"

/* Object type enumeration */
typedef enum e_object_type
{
	OBJ_NONE = 0,
	OBJ_SPHERE,
	OBJ_PLANE,
	OBJ_CYLINDER
}	t_object_type;

/* RGB color with integer components in range [0, 255] */
typedef struct s_color
{
	int	r;
	int	g;
	int	b;
}	t_color;

/* Sphere-specific data */
typedef struct s_sphere_data
{
	t_vec3	center;
	double	radius;
	double	radius_sq;
}	t_sphere_data;

/* Plane-specific data */
typedef struct s_plane_data
{
	t_vec3	point;
	t_vec3	normal;
}	t_plane_data;

/* Cylinder-specific data */
typedef struct s_cylinder_data
{
	t_vec3	center;
	t_vec3	axis;
	double	radius;
	double	radius_sq;
	double	half_height;
}	t_cylinder_data;

/* Unified object structure using union */
typedef struct s_object
{
	t_object_type	type;
	t_color			color;
	char			id[8];
	union
	{
		t_sphere_data	sphere;
		t_plane_data	plane;
		t_cylinder_data	cylinder;
	}	data;
}	t_object;

/* Legacy type aliases for backward compatibility */
typedef struct s_sphere
{
	t_vec3	center;
	double	diameter;
	t_color	color;
	double	radius;
	double	radius_squared;
	char	id[8];
}	t_sphere;

typedef struct s_plane
{
	t_vec3	point;
	t_vec3	normal;
	t_color	color;
	char	id[8];
}	t_plane;

typedef struct s_cylinder
{
	t_vec3	center;
	t_vec3	axis;
	double	diameter;
	double	height;
	t_color	color;
	double	radius;
	double	radius_squared;
	double	half_height;
	char	id[8];
}	t_cylinder;

#endif
