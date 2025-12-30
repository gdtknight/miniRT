/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spatial.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 21:30:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPATIAL_H
# define SPATIAL_H

# include "vec3.h"
# include "ray.h"
# include "objects.h"

/* Forward declarations */
typedef struct s_scene	t_scene;

/* Axis-Aligned Bounding Box for spatial queries */
typedef struct s_aabb
{
	t_vec3	min;
	t_vec3	max;
}	t_aabb;

/* Reference to any object in the scene */
typedef struct s_object_ref
{
	int	type;
	int	index;
}	t_object_ref;

/* BVH tree node for spatial acceleration */
typedef struct s_bvh_node
{
	t_aabb				bounds;
	struct s_bvh_node	*left;
	struct s_bvh_node	*right;
	t_object_ref		*objects;
	int					object_count;
}	t_bvh_node;

/* BVH tree root structure */
typedef struct s_bvh
{
	t_bvh_node	*root;
	int			enabled;
	int			total_nodes;
	int			max_depth;
}	t_bvh;

/* AABB operations */
t_aabb		aabb_create(t_vec3 min, t_vec3 max);
t_aabb		aabb_merge(t_aabb a, t_aabb b);
t_aabb		aabb_for_sphere(t_vec3 center, double radius);
t_aabb		aabb_for_cylinder(t_vec3 center, t_vec3 axis, double radius,
				double height);
t_aabb		aabb_for_plane(t_vec3 point, t_vec3 normal);
int			aabb_intersect(t_aabb box, t_ray ray, double *t_min, double *t_max);
double		aabb_surface_area(t_aabb box);

/* BVH construction */
t_bvh		*bvh_create(void);
void		bvh_destroy(t_bvh *bvh);
void		bvh_build(t_bvh *bvh, t_object_ref *objects, int count,
				void *scene);
t_bvh_node	*bvh_build_recursive(t_object_ref *objects, int count,
				void *scene, int depth);

/* BVH traversal */
int			bvh_intersect(t_bvh *bvh, t_ray ray, t_hit_record *hit,
				void *scene);
int			bvh_node_intersect(t_bvh_node *node, t_ray ray, t_hit_record *hit,
				void *scene);

/* Object bounds calculation */
t_aabb		get_object_bounds(t_object_ref ref, void *scene);
t_vec3		get_object_center(t_object_ref ref, void *scene);

/* Scene BVH initialization */
void		scene_build_bvh(t_scene *scene);

#endif
