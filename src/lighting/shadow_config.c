/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_config.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:09 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:09 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"

/*
** Initialize shadow configuration with default values.
** 16 samples for soft shadows with subtle edge softness.
** Bias scale of 2.0 prevents shadow acne artifacts.
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

/*
** Validate shadow configuration parameters are within valid ranges.
** Ensures samples >= 1, softness in [0.0, 1.0], and bias_scale >= 0.
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
void	set_shadow_samples(t_shadow_config *config, int samples)
{
	if (config && samples >= 1)
		config->samples = samples;
}

/*
** Set shadow edge softness factor.
** 0.0 = hard edges, 1.0 = very soft edges.
*/
void	set_shadow_softness(t_shadow_config *config, double softness)
{
	if (config && softness >= 0.0 && softness <= 1.0)
		config->softness = softness;
}
