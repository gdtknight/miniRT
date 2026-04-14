/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/10 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common/vec3.h"
#include <math.h>

/**
 * @brief Compute a tangent vector perpendicular to a normal.
 *
 * Uses the world Y axis as reference, falling back to the X axis
 * when the normal is nearly parallel to Y.
 *
 * @param n Surface normal (assumed normalized).
 * @return t_vec3 Normalized tangent vector.
 */
t_vec3	get_tangent(t_vec3 n)
{
	if (fabs(n.y) > 0.9)
		return (vec3_normalize(vec3_cross(n, (t_vec3){1, 0, 0})));
	return (vec3_normalize(vec3_cross(n, (t_vec3){0, 1, 0})));
}
