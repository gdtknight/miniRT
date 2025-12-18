/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_calc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:04 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:05 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"
#include "minirt.h"
#include "vec3.h"
#include <math.h>

/*
** Calculate adaptive shadow bias based on surface angle.
** Increases bias for grazing angles to prevent shadow acne.
** Returns adjusted bias value scaled by angle factor.
*/
double	calculate_shadow_bias(t_vec3 normal, t_vec3 light_dir, \
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
t_vec3	generate_shadow_sample_offset(double radius, int sample_index, \
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
	angle = 2.0 * 3.14159265358979323846 * (sample_index % grid_size) \
		/ (double)grid_size;
	r = radius * (sample_index / (double)grid_size + 0.5) / (double)grid_size;
	offset.x = r * cos(angle);
	offset.y = r * sin(angle);
	offset.z = 0.0;
	return (offset);
}

/*
** Helper structure for shadow sampling parameters.
*/
typedef struct s_shadow_sample
{
	t_scene			*scene;
	t_vec3			point;
	t_vec3			light_pos;
	t_shadow_config	*config;
	double			bias;
}	t_shadow_sample;

/*
** Cast single shadow ray with offset.
*/
static int	sample_shadow_ray(t_shadow_sample *params, int index)
{
	t_vec3	offset;
	t_vec3	sample_light_pos;

	offset = generate_shadow_sample_offset(params->config->softness * 2.0, \
		index, params->config->samples);
	sample_light_pos = vec3_add(params->light_pos, offset);
	return (is_in_shadow(params->scene, params->point, \
		sample_light_pos, params->bias));
}

/*
** Calculate shadow samples by casting rays to light positions.
*/
static double	calc_shadow_samples(t_scene *scene, t_vec3 point, \
		t_vec3 light_pos, t_shadow_config *config)
{
	t_shadow_sample	params;
	double			shadow_count;
	t_vec3			normal;
	t_vec3			light_dir;
	int				i;

	shadow_count = 0.0;
	light_dir = vec3_normalize(vec3_subtract(light_pos, point));
	normal = (t_vec3){0.0, 1.0, 0.0};
	params.scene = scene;
	params.point = point;
	params.light_pos = light_pos;
	params.config = config;
	params.bias = calculate_shadow_bias(normal, light_dir, 0.001);
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
double	calculate_shadow_factor(t_scene *scene, t_vec3 point, \
		t_vec3 light_pos, t_shadow_config *config)
{
	double	shadow_count;

	shadow_count = calc_shadow_samples(scene, point, light_pos, config);
	return (shadow_count / (double)config->samples);
}
