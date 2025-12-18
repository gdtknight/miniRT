/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:19 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:20 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vec3.h"

/*
** Add two 3D vectors component-wise.
** Returns new vector: (a.x + b.x, a.y + b.y, a.z + b.z)
*/
t_vec3	vec3_add(t_vec3 a, t_vec3 b)
{
	t_vec3	result;

	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return (result);
}

/*
** Subtract vector b from vector a component-wise.
** Returns new vector: (a.x - b.x, a.y - b.y, a.z - b.z)
*/
t_vec3	vec3_subtract(t_vec3 a, t_vec3 b)
{
	t_vec3	result;

	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return (result);
}

/*
** Multiply vector by scalar value.
** Returns new vector: (v.x * scalar, v.y * scalar, v.z * scalar)
*/
t_vec3	vec3_multiply(t_vec3 v, double scalar)
{
	t_vec3	result;

	result.x = v.x * scalar;
	result.y = v.y * scalar;
	result.z = v.z * scalar;
	return (result);
}
