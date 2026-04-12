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

# include "scene/scene.h"

# define SHADOW_BVH_THRESHOLD 5

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
