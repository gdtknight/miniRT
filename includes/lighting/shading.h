/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shading.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/03/24 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADING_H
# define SHADING_H

# include "scene/scene.h"
# include "common/ray.h"

/* Floating point color for intermediate calculations (0.0 - 1.0+) */
typedef struct s_color_f
{
	double	r;
	double	g;
	double	b;
}	t_color_f;

/* Shading functions */
/**
 * @brief Compute the final pixel color using Phong shading.
 *
 * Applies checkerboard and bump map textures when present,
 * accumulates per-light contributions, and combines them with
 * ambient light to produce a clamped RGB color.
 *
 * @param scene Scene containing lights and ambient settings.
 * @param hit Hit record with surface point, normal, and color.
 * @return Final clamped RGB color.
 */
t_color	apply_lighting(t_scene *scene, t_hit *hit);

/**
 * @brief Clamp RGB color components into the valid 0-255 range.
 *
 * Prevents overflow or wraparound when writing final pixel colors.
 *
 * @param result Color to clamp in place.
 */
void	clamp_color(t_color *result);

/**
 * @brief Compute x^32 using repeated squaring.
 *
 * Faster alternative to pow(x, 32.0) for integer exponent.
 *
 * @param x Base value.
 * @return x raised to the power of 32.
 */
double	fast_pow32(double x);

#endif
