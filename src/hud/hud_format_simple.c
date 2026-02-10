/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_format_simple.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

/**
 * @brief Format FPS value for HUD display.
 *
 * @param buf Output buffer.
 * @param fps Frames per second value.
 */
void	hud_format_fps(char *buf, double fps)
{
	hud_format_float(buf, fps);
}

/**
 * @brief Format time in microseconds as milliseconds.
 *
 * @param buf Output buffer.
 * @param time_us Time in microseconds.
 */
void	hud_format_time_ms(char *buf, long time_us)
{
	double	time_ms;

	time_ms = time_us / 1000.0;
	hud_format_float(buf, time_ms);
}

/**
 * @brief Format BVH enabled state as "ON"/"OFF".
 *
 * @param buf Output buffer.
 * @param enabled Non-zero if BVH is enabled.
 */
void	hud_format_bvh_status(char *buf, int enabled)
{
	if (enabled)
		ft_strcpy(buf, "ON");
	else
		ft_strcpy(buf, "OFF");
}
