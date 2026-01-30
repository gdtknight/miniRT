/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_config.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:09 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:45:37 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"

/**
 * @brief Initialize shadow configuration with default values.
 *
 * Provides a balanced default for soft shadow quality and acne prevention.
 *
 * @return t_shadow_config Initialized configuration.
 */
t_shadow_config	init_shadow_config(void)
{
	t_shadow_config	config;

	config.samples = 16;
	config.softness = 0.3;
	config.bias_scale = 2.0;
	config.enable_ao = 0;
	return (config);
}

/**
 * @brief Validate shadow configuration ranges.
 *
 * Ensures samples >= 1, softness in [0, 1], and bias_scale >= 0.
 *
 * @param config Configuration to validate.
 * @return int 1 if valid, 0 otherwise.
 */
int	validate_shadow_config(t_shadow_config *config)
{
	if (!config)
		return (0);
	if (config->samples < 1)
		return (0);
	if (config->softness < 0.0 || config->softness > 1.0)
		return (0);
	if (config->bias_scale < 0.0)
		return (0);
	return (1);
}

/*
** Set number of shadow samples for soft shadow quality.
** More samples = smoother shadows but slower rendering.
*/
/**
 * @brief Set the number of shadow samples.
 *
 * Higher sample counts yield softer shadows at a performance cost.
 *
 * @param config Configuration to update.
 * @param samples Number of samples (must be >= 1).
 */
void	set_shadow_samples(t_shadow_config *config, int samples)
{
	if (config && samples >= 1)
		config->samples = samples;
}

/*
** Set shadow edge softness factor.
** 0.0 = hard edges, 1.0 = very soft edges.
*/
/**
 * @brief Set shadow edge softness.
 *
 * 0.0 yields hard edges; 1.0 yields very soft edges.
 *
 * @param config Configuration to update.
 * @param softness Softness value in [0, 1].
 */
void	set_shadow_softness(t_shadow_config *config, double softness)
{
	if (config && softness >= 0.0 && softness <= 1.0)
		config->softness = softness;
}
