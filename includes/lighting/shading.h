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
t_color	apply_lighting(t_scene *scene, t_hit *hit);
void	clamp_color(t_color *result);
double	fast_pow32(double x);

#endif
