/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_attenuation.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 16:20:30 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:45:26 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"

/**
 * @brief Compute distance-based shadow attenuation.
 *
 * Uses an inverse-square style falloff scaled by a softness parameter
 * and clamps the result to [0, 1].
 *
 * @param distance Distance from the shaded point to the light.
 * @param max_distance Maximum distance considered for attenuation.
 * @param softness Softness factor controlling falloff steepness.
 * @return double Attenuation factor in [0, 1].
 */
double	calculate_shadow_attenuation(double distance, double max_distance,
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
