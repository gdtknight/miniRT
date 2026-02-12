/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_calc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:04 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:45:30 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"
#include "minirt.h"
#include "vec3.h"
#include <math.h>

/**
 * @brief Compute adaptive shadow bias based on angle to the light.
 *
 * Increases bias for grazing angles to reduce shadow acne artifacts.
 *
 * @param normal Surface normal at the shaded point.
 * @param light_dir Normalized direction toward the light.
 * @param base_bias Base bias value.
 * @return double Adjusted bias value.
 */
double	calculate_shadow_bias(t_vec3 normal, t_vec3 light_dir,
		double base_bias)
{
	double	dot_nl;
	double	angle_factor;
	double	bias;

	dot_nl = vec3_dot(normal, light_dir);
	if (dot_nl < 0.0)
		dot_nl = -dot_nl;
	angle_factor = 1.0 - dot_nl;
	bias = base_bias * (1.0 + angle_factor * 2.0);
	return (bias);
}

/*
** Generate offset vector for soft shadow sampling.
** Uses stratified sampling in circular pattern around light.
** Returns zero vector if only one sample requested.
*/
/**
 * @brief Generate a stratified offset for soft shadow sampling.
 *
 * Produces a 2D offset in a circular pattern to jitter the light position.
 *
 * @param radius Sampling radius around the light.
 * @param sample_index Index of the current sample.
 * @param total_samples Total number of samples.
 * @return t_vec3 Offset vector for the sample.
 */
t_vec3	generate_shadow_sample_offset(double radius, int sample_index,
		int total_samples)
{
	t_vec3	offset;
	double	angle;
	double	r;
	int		grid_size;

	if (total_samples <= 1)
		return ((t_vec3){0.0, 0.0, 0.0});
	grid_size = (int)sqrt((double)total_samples);
	if (grid_size < 1)
		grid_size = 1;
	angle = 2.0 * M_PI * (sample_index % grid_size)
		/ (double)grid_size;
	r = radius * (sample_index / (double)grid_size + 0.5) / (double)grid_size;
	offset.x = r * cos(angle);
	offset.y = r * sin(angle);
	offset.z = 0.0;
	return (offset);
}

/*
** Cast single shadow ray with offset.
*/
/**
 * @brief Cast a single shadow ray with soft shadow offset.
 *
 * Computes a sample offset and tests whether the point is occluded toward
 * the jittered light position.
 *
 * @param params Shadow sampling parameters.
 * @param index Sample index.
 * @return int 1 if in shadow, 0 if lit.
 */
static int	sample_shadow_ray(t_shadow_sample *params, int index)
{
	t_vec3	offset;
	t_vec3	sample_light_pos;
	double	radius;

	radius = params->config->softness * 2.0;
	if (params->config->offset_lut)
		offset = vec3_multiply(params->config->offset_lut[index], radius);
	else
		offset = generate_shadow_sample_offset(radius, index,
				params->config->samples);
	sample_light_pos = vec3_add(params->light_pos, offset);
	return (is_in_shadow(params->scene, params->point,
			sample_light_pos, params->bias));
}

/*
** Calculate shadow samples by casting rays to light positions.
*/
/**
 * @brief Count shadowed samples for soft shadows.
 *
 * Casts multiple shadow rays toward jittered light positions and returns
 * the number of occluded samples.
 *
 * @param scene Scene containing objects for occlusion tests.
 * @param point Point being shaded.
 * @param light_pos Light position in world space.
 * @param config Shadow configuration including sample count.
 * @return double Number of samples that are in shadow.
 */
static double	calc_shadow_samples(t_scene *scene, t_shadow_query query,
		t_vec3 light_pos, t_shadow_config *config)
{
	t_shadow_sample	params;
	double			shadow_count;
	t_vec3			light_dir;
	int				i;

	shadow_count = 0.0;
	light_dir = vec3_normalize(vec3_subtract(light_pos, query.point));
	params.scene = scene;
	params.point = query.point;
	params.light_pos = light_pos;
	params.config = config;
	params.bias = calculate_shadow_bias(query.normal, light_dir, 0.001);
	i = 0;
	while (i < config->samples)
	{
		if (sample_shadow_ray(&params, i))
			shadow_count += 1.0;
		i++;
	}
	return (shadow_count);
}

/*
** Calculate shadow factor using multiple shadow rays.
** Casts multiple rays to determine partial occlusion.
** Returns 0.0 (fully lit) to 1.0 (fully shadowed).
*/
/**
 * @brief Compute shadow factor as the ratio of occluded samples.
 *
 * Returns 0.0 for fully lit and 1.0 for fully shadowed.
 *
 * @param scene Scene containing objects for occlusion tests.
 * @param point Point being shaded.
 * @param light_pos Light position in world space.
 * @param config Shadow configuration including sample count.
 * @return double Shadow factor in [0, 1].
 */
double	calculate_shadow_factor(t_scene *scene, t_shadow_query query,
		t_vec3 light_pos, t_shadow_config *config)
{
	double	shadow_count;

	shadow_count = calc_shadow_samples(scene, query, light_pos, config);
	return (shadow_count / (double)config->samples);
}
