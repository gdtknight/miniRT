/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:33 by yoshin            #+#    #+#             */
/*   Updated: 2026/03/22 16:24:09 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAY_H
# define RAY_H

# include <stdbool.h>
# include <stddef.h>
# include "common/vec3.h"
# include "common/objects.h"

/* Intersection tolerance constants */
# define EPSILON 0.0001
# define RAY_T_MIN 0.001
# define COEFF_EPSILON 0.0001

/* Ray with origin point and normalized direction vector */
typedef struct s_ray
{
	t_vec3	origin;
	t_vec3	direction;
	t_vec3	inv_dir;
}	t_ray;

/* Ray-object intersection result with hit point and surface info */
typedef struct s_hit
{
	double		distance;
	t_vec3		point;
	t_vec3		normal;
	t_color		color;
	t_object	*obj;
}	t_hit;

/* Helper structure for cylinder intersection calculations */
typedef struct s_cyl_calc
{
	double	a;
	double	b;
	double	c;
	double	discriminant;
	double	t;
	double	m;
	double	t2;
	double	m2;
}	t_cyl_calc;

#endif
