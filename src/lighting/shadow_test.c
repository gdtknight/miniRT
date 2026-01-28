/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_test.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:13 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"
#include "minirt.h"
#include "vec3.h"
#include "ray.h"

int	intersect_object_new(t_ray *ray, t_object *obj, t_hit_record *hit);

static int	check_object_shadow(t_scene *scene, t_ray *ray, t_hit *hit)
{
	int			i;
	t_object	*obj;

	i = 0;
	while (i < scene->objects.count)
	{
		obj = &scene->objects.items[i];
		if (intersect_object_new(ray, obj, hit))
			return (1);
		i++;
	}
	return (0);
}

int	is_in_shadow(t_scene *scene, t_vec3 point, t_vec3 light_pos, double bias)
{
	t_ray	shadow_ray;
	t_hit	shadow_hit;
	t_vec3	to_light;
	t_vec3	light_dir;

	to_light = vec3_subtract(light_pos, point);
	shadow_hit.distance = vec3_magnitude(to_light);
	light_dir = vec3_normalize(to_light);
	shadow_ray.origin = vec3_add(point, vec3_multiply(light_dir, bias));
	shadow_ray.direction = light_dir;
	return (check_object_shadow(scene, &shadow_ray, &shadow_hit));
}
