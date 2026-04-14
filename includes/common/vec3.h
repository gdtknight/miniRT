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

/**
 * @brief Add two vectors component-wise.
 *
 * @param a First vector.
 * @param b Second vector.
 * @return Sum vector.
 */
t_vec3	vec3_add(t_vec3 a, t_vec3 b);

/**
 * @brief Subtract two vectors component-wise.
 *
 * @param a Minuend vector.
 * @param b Subtrahend vector.
 * @return Difference vector.
 */
t_vec3	vec3_subtract(t_vec3 a, t_vec3 b);

/**
 * @brief Multiply a vector by a scalar.
 *
 * @param v Input vector.
 * @param scalar Scalar multiplier.
 * @return Scaled vector.
 */
t_vec3	vec3_multiply(t_vec3 v, double scalar);

/**
 * @brief Compute dot product of two vectors.
 *
 * @param a First vector.
 * @param b Second vector.
 * @return Dot product value.
 */
double	vec3_dot(t_vec3 a, t_vec3 b);

/**
 * @brief Compute cross product of two vectors.
 *
 * @param a First vector.
 * @param b Second vector.
 * @return Vector perpendicular to both inputs.
 */
t_vec3	vec3_cross(t_vec3 a, t_vec3 b);

/**
 * @brief Compute vector magnitude (length).
 *
 * @param v Input vector.
 * @return Magnitude of the vector.
 */
double	vec3_magnitude(t_vec3 v);

/**
 * @brief Normalize a vector to unit length.
 *
 * Returns a zero vector if the input length is zero.
 *
 * @param v Input vector.
 * @return Normalized vector.
 */
t_vec3	vec3_normalize(t_vec3 v);

#endif
