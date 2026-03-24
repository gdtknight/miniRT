/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersect.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/03/24 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INTERSECT_H
# define INTERSECT_H

# include "ray.h"

/* Ray-object intersection functions */
int		intersect_object_new(t_ray *ray, t_object *obj, t_hit *hit);
int		intersect_cylinder_new(t_ray *ray, t_cylinder_data *c, t_color color,
			t_hit *hit);
int		intersect_cone_new(t_ray *ray, t_object *obj, t_hit *hit);
int		intersect_cone_body(t_ray *ray, t_cone_data *c, t_hit *hit);

#endif
