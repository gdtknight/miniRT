/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trace.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:06 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "ray.h"
#include "spatial.h"
#include "metrics.h"
#include <math.h>

int	intersect_object_new(t_ray *ray, t_object *obj, t_hit_record *hit);

static int	check_all_objects(t_scene *scene, t_ray *ray, t_hit *hit)
{
	int			i;
	int			found;
	t_hit		temp;
	t_object	*obj;

	found = 0;
	i = 0;
	while (i < scene->objects.count)
	{
		obj = &scene->objects.items[i];
		temp.distance = hit->distance;
		if (intersect_object_new(ray, obj, &temp))
		{
			*hit = temp;
			found = 1;
		}
		i++;
	}
	return (found);
}

t_color	trace_ray(t_scene *scene, t_ray *ray)
{
	t_hit	hit;
	int		found;

	metrics_add_ray(&scene->metrics);
	found = 0;
	hit.distance = INFINITY;
	if (scene->bvh && scene->bvh->enabled)
	{
		if (bvh_intersect(scene->bvh, *ray, &hit, scene))
			found = 1;
	}
	if (!found)
	{
		if (check_all_objects(scene, ray, &hit))
			found = 1;
	}
	if (found)
		return (apply_lighting(scene, &hit));
	return ((t_color){0, 0, 0});
}
