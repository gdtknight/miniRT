/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_traverse.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 21:30:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "minirt.h"
#include "window.h"
#include "ray.h"

static int	intersect_object(t_object_ref ref, t_ray ray, t_hit_record *hit,
		void *scene_ptr)
{
	t_scene	*scene;

	scene = (t_scene *)scene_ptr;
	if (ref.type == OBJ_SPHERE)
		return (intersect_sphere(&ray, &scene->spheres[ref.index], hit));
	else if (ref.type == OBJ_PLANE)
		return (intersect_plane(&ray, &scene->planes[ref.index], hit));
	else if (ref.type == OBJ_CYLINDER)
		return (intersect_cylinder(&ray, &scene->cylinders[ref.index], hit));
	return (0);
}

static int	bvh_leaf_intersect(t_bvh_node *node, t_ray ray, t_hit_record *hit,
		void *scene)
{
	int				i;
	int				hit_anything;
	t_hit_record	temp_hit;

	hit_anything = 0;
	i = 0;
	while (i < node->object_count)
	{
		if (intersect_object(node->objects[i], ray, &temp_hit, scene))
		{
			if (!hit_anything || temp_hit.distance < hit->distance)
			{
				*hit = temp_hit;
				hit_anything = 1;
			}
		}
		i++;
	}
	return (hit_anything);
}

int	bvh_node_intersect(t_bvh_node *node, t_ray ray, t_hit_record *hit,
		void *scene)
{
	double			t_min;
	double			t_max;
	int				hit_left;
	int				hit_right;
	t_hit_record	left_hit;
	t_hit_record	right_hit;

	if (!node)
		return (0);
	t_min = 0.001;
	t_max = 1000000.0;
	if (!aabb_intersect(node->bounds, ray, &t_min, &t_max))
		return (0);
	if (node->object_count > 0)
		return (bvh_leaf_intersect(node, ray, hit, scene));
	hit_left = bvh_node_intersect(node->left, ray, &left_hit, scene);
	hit_right = bvh_node_intersect(node->right, ray, &right_hit, scene);
	if (hit_left && hit_right)
	{
		if (left_hit.distance < right_hit.distance)
			*hit = left_hit;
		else
			*hit = right_hit;
		return (1);
	}
	if (hit_left)
	{
		*hit = left_hit;
		return (1);
	}
	if (hit_right)
	{
		*hit = right_hit;
		return (1);
	}
	return (0);
}

int	bvh_intersect(t_bvh *bvh, t_ray ray, t_hit_record *hit, void *scene)
{
	if (!bvh || !bvh->root || !bvh->enabled)
		return (0);
	return (bvh_node_intersect(bvh->root, ray, hit, scene));
}
