/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spatial.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
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

/* Reference to object in unified object list (simplified) */
typedef struct s_object_ref
{
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
	int					depth;
	int					split_axis;
}	t_bvh_node;

/* BVH plane reference list for separated plane intersection */
typedef struct s_plane_refs
{
	int		*indices;
	int		count;
}	t_plane_refs;

/* BVH tree root structure */
typedef struct s_bvh
{
	t_bvh_node		*root;
	int				enabled;
	int				visualize;
	t_plane_refs	plane_refs;
}	t_bvh;

/* Axis intersection check parameters */
typedef struct s_axis_check
{
	double	box_min;
	double	box_max;
	double	ray_origin;
	double	inv_dir;
	double	*tmin;
	double	*tmax;
}	t_axis_check;

/* BVH partition parameters */
typedef struct s_partition_params
{
	t_object_ref	*objects;
	int				count;
	int				axis;
	double			split;
	void			*scene;
}	t_partition_params;

/* BVH split node parameters */
typedef struct s_split_params
{
	t_aabb			bounds;
	t_object_ref	*objects;
	int				mid;
	int				count;
	void			*scene;
	int				depth;
	int				axis;
}	t_split_params;

/* Helper functions */
double		min_double(double a, double b);
double		max_double(double a, double b);

/* AABB operations */
t_aabb		aabb_create(t_vec3 min, t_vec3 max);
t_aabb		aabb_merge(t_aabb a, t_aabb b);
t_aabb		aabb_for_object(t_object *obj);
int			aabb_intersect(t_aabb box, t_ray ray, double *t_min, double *t_max);

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
int			bvh_intersect_any(t_bvh *bvh, t_ray ray, double max_dist,
				void *scene);

/* Ray direction helper */
int			ray_goes_positive(t_vec3 dir, int axis);

/* Object bounds calculation */
t_aabb		get_object_bounds(t_object_ref ref, void *scene);
t_vec3		get_object_center(t_object_ref ref, void *scene);

/* Scene BVH initialization */
void		build_scene_bvh(t_scene *scene);

#endif
