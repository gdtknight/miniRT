/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:33 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:18:34 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAY_H
# define RAY_H

# include <stdbool.h>
# include <stddef.h>
# include "vec3.h"
# include "objects.h"

/* Ray with origin point and normalized direction vector */
typedef struct s_ray
{
	t_vec3	origin;
	t_vec3	direction;
}	t_ray;

/* Ray-object intersection result with hit point and surface info */
typedef struct s_hit
{
	bool	hit;
	double	distance;
	t_vec3	point;
	t_vec3	normal;
	t_color	color;
}	t_hit;

typedef t_hit	t_hit_record;

/* Helper structure for cylinder intersection calculations */
typedef struct s_cyl_calc
{
	double	a;
	double	b;
	double	c;
	double	discriminant;
	double	t;
	double	m;
}	t_cyl_calc;

/* Generic intersection function pointer type */
typedef int		(*t_intersect_fn)(t_ray *ray, void *object, t_hit *hit);

/* Helper structure for generic intersection checking */
typedef struct s_intersect_params
{
	void			*objects;
	int				count;
	size_t			obj_size;
	t_intersect_fn	intersect_fn;
	t_ray			*ray;
	t_hit			*hit;
}	t_intersect_params;

/* Intersection with unified object */
int		intersect_object_new(t_ray *ray, t_object *obj, t_hit *hit);
int		intersect_cylinder_new(t_ray *ray, t_cylinder_data *c, t_color color,
			t_hit *hit);

#endif
