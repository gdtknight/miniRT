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

# include "vec3.h"
# include "objects.h"
# include <stdbool.h>

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

/* Generic intersection function pointer type */
typedef int	(*t_intersect_fn)(t_ray *ray, void *object, t_hit *hit);

/* Function declarations */
int		intersect_sphere(t_ray *ray, t_sphere *sphere, t_hit *hit);
int		intersect_plane(t_ray *ray, t_plane *plane, t_hit *hit);
int		intersect_cylinder(t_ray *ray, t_cylinder *cylinder, t_hit *hit);
int		intersect_cylinder_body(t_ray *ray, t_cylinder *cyl, t_hit *hit);
int		intersect_cylinder_cap(t_ray *ray, t_cylinder *cyl, t_hit *hit, \
		double cap_m);

#endif
