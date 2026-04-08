/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:44 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:18:45 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VEC3_H
# define VEC3_H

/* 3D vector or point in space with x, y, z coordinates */
typedef struct s_vec3
{
	double	x;
	double	y;
	double	z;
}	t_vec3;

/* Add two vectors component-wise */
t_vec3	vec3_add(t_vec3 a, t_vec3 b);
/* Subtract vector b from vector a component-wise */
t_vec3	vec3_subtract(t_vec3 a, t_vec3 b);
/* Multiply vector by scalar value */
t_vec3	vec3_multiply(t_vec3 v, double scalar);
/* Calculate dot product of two vectors */
double	vec3_dot(t_vec3 a, t_vec3 b);
/* Calculate cross product of two vectors */
t_vec3	vec3_cross(t_vec3 a, t_vec3 b);
/* Calculate magnitude (length) of vector */
double	vec3_magnitude(t_vec3 v);
/* Return normalized vector (length = 1) */
t_vec3	vec3_normalize(t_vec3 v);

#endif
