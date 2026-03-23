/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metrics_shadow.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/05 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "metrics.h"

/**
 * @brief Increment the shadow intersection test counter.
 *
 * @param metrics Metrics structure to update.
 */
void	metrics_add_shadow_intersect(t_metrics *metrics)
{
	if (metrics)
		metrics->ray.shadow_intersect_tests++;
}
