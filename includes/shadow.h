/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:38 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:18:40 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADOW_H
# define SHADOW_H

# include "vec3.h"

# define SHADOW_BVH_THRESHOLD 5

/* Forward declaration */
typedef struct s_scene	t_scene;

/**
 * @brief Shadow configuration structure
 *
 * Controls shadow rendering quality and appearance.
 * samples: Number of shadow rays (1 = hard shadows, >1 = soft shadows)
 * softness: Shadow edge softness factor (0.0-1.0)
 */
typedef struct s_shadow_config
{
	int		samples;
	double	softness;
	t_vec3	*offset_lut;
}	t_shadow_config;

/* Shadow query: surface point + normal for bias calculation */
typedef struct s_shadow_query
{
	t_vec3	point;
	t_vec3	normal;
}	t_shadow_query;

/* Helper structure for shadow sampling parameters */
typedef struct s_shadow_sample
{
	t_scene			*scene;
	t_shadow_query	query;
	t_vec3			light_pos;
	t_shadow_config	*config;
	double			bias;
}	t_shadow_sample;

/**
 * @brief Initialize shadow configuration with default values
 * 
 * @return Default shadow configuration
 */
t_shadow_config	init_shadow_config(void);

/**
 * @brief Calculate adaptive shadow bias
 * 
 * Computes shadow bias based on surface angle to prevent shadow acne
 * on grazing angles while maintaining shadow accuracy.
 * 
 * @param normal Surface normal vector
 * @param light_dir Direction to light source
 * @param base_bias Base bias value
 * @return Adjusted bias value
 */
double			calculate_shadow_bias(t_vec3 normal, t_vec3 light_dir,
					double base_bias);

/**
 * @brief Generate shadow sample offset
 * 
 * Creates a random offset for shadow ray sampling to produce soft shadows.
 * Uses stratified sampling for better distribution.
 * 
 * @param radius Sampling radius
 * @param sample_index Current sample index
 * @param total_samples Total number of samples
 * @return Offset vector
 */
t_vec3			generate_shadow_sample_offset(double radius,
					int sample_index, int total_samples);

/**
 * @brief Calculate shadow factor using multiple shadow rays
 * 
 * Casts multiple shadow rays to determine shadow intensity.
 * Returns 0.0 for no shadow, 1.0 for full shadow.
 * 
 * @param scene Scene data
 * @param point Point to test for shadows
 * @param light_pos Light source position
 * @param config Shadow configuration
 * @return Shadow factor (0.0-1.0)
 */
double			calculate_shadow_factor(t_scene *scene, t_shadow_query query,
					t_vec3 light_pos, t_shadow_config *config);
/**
 * @brief Test if point is in shadow (single ray)
 * 
 * Casts a single shadow ray to determine if point is occluded.
 * 
 * @param scene Scene data
 * @param point Point to test
 * @param light_pos Light source position
 * @param bias Shadow bias offset
 * @return 1 if in shadow, 0 if lit
 */
int				shadow_is_occluded(t_scene *scene, t_shadow_query query,
					t_vec3 light_pos, double bias);

#endif
