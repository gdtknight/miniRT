/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_attenuation.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 16:20:30 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 16:20:31 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"

/*
** Calculate distance-based shadow attenuation.
** Softens shadows based on distance from light source.
** Uses inverse square falloff with configurable softness.
*/
double	calculate_shadow_attenuation(double distance, double max_distance, \
		double softness)
{
	double	normalized_dist;
	double	attenuation;

	if (max_distance <= 0.0)
		return (1.0);
	normalized_dist = distance / max_distance;
	attenuation = 1.0 / (1.0 + normalized_dist * normalized_dist * softness);
	if (attenuation < 0.0)
		attenuation = 0.0;
	if (attenuation > 1.0)
		attenuation = 1.0;
	return (attenuation);
}
