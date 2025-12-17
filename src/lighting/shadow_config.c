/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_config.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/17 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"

/**
 * @brief Initialize shadow configuration with default values
 * 
 * Default configuration provides good balance between quality and performance:
 * - 4 samples for soft shadows
 * - 0.1 softness for subtle shadow edges
 * - 2.0 bias scale for preventing shadow acne
 * 
 * @return Default shadow configuration
 */
t_shadow_config	init_shadow_config(void)
{
	t_shadow_config	config;

	config.samples = 4;
	config.softness = 0.1;
	config.bias_scale = 2.0;
	config.enable_ao = 0;
	return (config);
}

/**
 * @brief Validate shadow configuration parameters
 * 
 * Checks that all parameters are within valid ranges:
 * - samples >= 1
 * - softness >= 0.0 and <= 1.0
 * - bias_scale >= 0.0
 * 
 * @param config Shadow configuration to validate
 * @return 1 if valid, 0 if invalid
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

/**
 * @brief Set number of shadow samples
 * 
 * @param config Shadow configuration
 * @param samples Number of samples (min 1)
 */
void	set_shadow_samples(t_shadow_config *config, int samples)
{
	if (config && samples >= 1)
		config->samples = samples;
}

/**
 * @brief Set shadow softness factor
 * 
 * @param config Shadow configuration
 * @param softness Softness factor (0.0-1.0)
 */
void	set_shadow_softness(t_shadow_config *config, double softness)
{
	if (config && softness >= 0.0 && softness <= 1.0)
		config->softness = softness;
}
