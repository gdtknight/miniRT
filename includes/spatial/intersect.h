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

# include "common/ray.h"

/* Ray-object intersection functions */
/**
 * @brief Dispatch ray intersection based on object type.
 *
 * Calls the appropriate intersection routine for the object geometry.
 *
 * @param ray Ray to test.
 * @param obj Scene object to intersect.
 * @param hit Hit record to update.
 * @return 1 if an intersection is found, 0 otherwise.
 */
int		intersect_object(t_ray *ray, t_object *obj, t_hit *hit);

/**
 * @brief Intersect a ray with a finite cylinder (body + caps).
 *
 * Tests the cylindrical body and both end caps, keeping the closest
 * hit.
 *
 * @param ray Ray to test.
 * @param c Cylinder data.
 * @param color Surface color.
 * @param hit Hit record to update.
 * @return 1 if any intersection is found, 0 otherwise.
 */
int		intersect_cylinder(t_ray *ray, t_cylinder_data *c, t_color color,
			t_hit *hit);

/**
 * @brief Test ray intersection with a complete cone (body + cap).
 *
 * Tests both the lateral surface and base cap, keeping the nearest
 * hit.
 *
 * @param ray Incoming ray.
 * @param obj Cone object.
 * @param hit Output hit record (updated if closer hit found).
 * @return 1 if any intersection found, 0 otherwise.
 */
int		intersect_cone(t_ray *ray, t_object *obj, t_hit *hit);

/**
 * @brief Test ray intersection with the cone lateral surface.
 *
 * Solves the quadratic, validates height bounds, and selects the
 * nearest valid hit.
 *
 * @param ray Incoming ray.
 * @param c Cone geometric data.
 * @param hit Output hit record (updated if closer hit found).
 * @return 1 if intersection found, 0 otherwise.
 */
int		intersect_cone_body(t_ray *ray, t_cone_data *c, t_hit *hit);

#endif
