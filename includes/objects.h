/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   objects.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:18 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:18:21 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OBJECTS_H
# define OBJECTS_H

# include "vec3.h"

/* RGB color with integer components in range [0, 255] */
typedef struct s_color
{
	int	r;
	int	g;
	int	b;
}	t_color;

/* Sphere object defined by center point, diameter, and color */
typedef struct s_sphere
{
	t_vec3	center;
	double	diameter;
	t_color	color;
	double	radius;
	double	radius_squared;
}	t_sphere;

/* Infinite plane defined by a point, normal vector, and color */
typedef struct s_plane
{
	t_vec3	point;
	t_vec3	normal;
	t_color	color;
}	t_plane;

/* Finite cylinder defined by center, axis, diameter, height, and color */
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
}	t_cylinder;

#endif
