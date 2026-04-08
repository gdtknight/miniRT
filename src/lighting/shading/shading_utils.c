/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shading_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/04/09 00:44:45 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common/objects.h"

/**
 * @brief Clamp RGB color components into the valid 0–255 range.
 *
 * Prevents overflow or wraparound when writing final pixel colors.
 *
 * @param result Color to clamp in place.
 */
void	clamp_color(t_color *result)
{
	if (result->r > 255)
		result->r = 255;
	if (result->g > 255)
		result->g = 255;
	if (result->b > 255)
		result->b = 255;
	if (result->r < 0)
		result->r = 0;
	if (result->g < 0)
		result->g = 0;
	if (result->b < 0)
		result->b = 0;
}

/**
 * @brief Compute x^32 using repeated squaring.
 *
 * Faster alternative to pow(x, 32.0) for integer exponent.
 *
 * @param x Base value.
 * @return double x raised to the power of 32.
 */
double	fast_pow32(double x)
{
	x = x * x;
	x = x * x;
	x = x * x;
	x = x * x;
	x = x * x;
	return (x);
}
