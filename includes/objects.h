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
	OBJ_CYLINDER,
	OBJ_CONE
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

/* Cone-specific data */
typedef struct s_cone_data
{
	t_vec3	center;
	t_vec3	axis;
	double	radius;
	double	radius_sq;
	double	half_height;
}	t_cone_data;

/* Forward declaration for bump map */
typedef struct s_bump_map	t_bump_map;

/* Unified object structure using union */
typedef struct s_object
{
	t_object_type	type;
	t_color			color;
	t_color			checker_color;
	int				has_checker;
	char			*bump_path;
	t_bump_map		*bump_map;
	char			id[8];
	union u_object_data
	{
		t_sphere_data	sphere;
		t_plane_data	plane;
		t_cylinder_data	cylinder;
		t_cone_data		cone;
	}	data;
}	t_object;

#endif
