/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_config.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:09 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/10 16:34:48 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"
#include <stdlib.h>
#include <math.h>

/**
 * @brief Initialize shadow offset LUT for soft shadow sampling.
 *
 * Precomputes circular stratified offsets for each sample.
 * If allocation fails, offset_lut remains NULL and fallback is used.
 *
 * @param config Shadow configuration with samples set.
 */
static void	init_shadow_offset_lut(t_shadow_config *config)
{
	int		i;
	int		grid_size;
	double	angle;
	double	r;

	config->offset_lut = malloc(sizeof(t_vec3) * config->samples);
	if (!config->offset_lut)
		return ;
	grid_size = (int)sqrt((double)config->samples);
	if (grid_size < 1)
		grid_size = 1;
	i = 0;
	while (i < config->samples)
	{
		angle = 2.0 * M_PI * (i % grid_size) / (double)grid_size;
		r = (i / (double)grid_size + 0.5) / (double)grid_size;
		config->offset_lut[i].x = r * cos(angle);
		config->offset_lut[i].y = r * sin(angle);
		config->offset_lut[i].z = 0.0;
		i++;
	}
}

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
	config.offset_lut = NULL;
	init_shadow_offset_lut(&config);
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
	if (!config || samples < 1)
		return ;
	free(config->offset_lut);
	config->offset_lut = NULL;
	config->samples = samples;
	init_shadow_offset_lut(config);
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
