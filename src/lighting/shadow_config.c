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
	config.offset_lut = NULL;
	init_shadow_offset_lut(&config);
	if (!config.offset_lut)
		config.samples = 1;
	return (config);
}

