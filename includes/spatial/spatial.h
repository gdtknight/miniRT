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

# include "common/vec3.h"
# include "common/ray.h"
# include "common/objects.h"

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

/* Helper functions */
/**
 * @brief Return the smaller of two doubles (NaN-safe).
 *
 * @param a First value.
 * @param b Second value.
 * @return Minimum of a and b.
 */
double		min_double(double a, double b);

/**
 * @brief Return the larger of two doubles (NaN-safe).
 *
 * @param a First value.
 * @param b Second value.
 * @return Maximum of a and b.
 */
double		max_double(double a, double b);

/* AABB operations */
/**
 * @brief Create an AABB from min and max corners.
 *
 * @param min Minimum corner.
 * @param max Maximum corner.
 * @return Constructed bounding box.
 */
t_aabb		aabb_create(t_vec3 min, t_vec3 max);

/**
 * @brief Merge two AABBs into a bounding box containing both.
 *
 * @param a First AABB.
 * @param b Second AABB.
 * @return Merged bounding box.
 */
t_aabb		aabb_merge(t_aabb a, t_aabb b);

/**
 * @brief Test a ray for intersection with an axis-aligned bounding box.
 *
 * Uses the slab method and updates t_min/t_max to the overlap range.
 *
 * @param box AABB to test.
 * @param ray Ray to test.
 * @param t_min Input/output minimum t value.
 * @param t_max Input/output maximum t value.
 * @return 1 if the ray intersects the box, 0 otherwise.
 */
int			aabb_intersect(t_aabb box, t_ray ray, double *t_min, double *t_max);

/* BVH construction */
/**
 * @brief Allocate and initialize a BVH structure.
 *
 * @return Newly allocated BVH or NULL on failure.
 */
t_bvh		*bvh_create(void);

/**
 * @brief Destroy a BVH and free all resources.
 *
 * @param bvh BVH to destroy.
 */
void		bvh_destroy(t_bvh *bvh);

/**
 * @brief Build or rebuild the BVH from object references.
 *
 * Destroys any existing BVH tree and constructs a new one.
 *
 * @param bvh BVH structure to populate.
 * @param objects Array of object references.
 * @param count Number of objects.
 * @param scene Pointer to the scene for bounds computation.
 */
void		bvh_build(t_bvh *bvh, t_object_ref *objects, int count,
				t_scene *scene);

/**
 * @brief Recursively build a BVH subtree.
 *
 * @param objects Array of object references.
 * @param count Number of objects in the current subset.
 * @param scene Pointer to the scene for bounds computation.
 * @param depth Current recursion depth.
 * @return Root node of the constructed subtree.
 */
t_bvh_node	*bvh_build_recursive(t_object_ref *objects, int count,
				t_scene *scene, int depth);

/* BVH traversal */
/**
 * @brief Intersect a ray against the BVH root if enabled.
 *
 * @param bvh BVH structure.
 * @param ray Ray to test.
 * @param hit Output hit record.
 * @param scene Pointer to the scene.
 * @return 1 if any hit is found, 0 otherwise.
 */
int			bvh_intersect(t_bvh *bvh, t_ray ray, t_hit *hit,
				t_scene *scene);

/**
 * @brief Test if any object intersects the ray within max_dist.
 *
 * Optimized for shadow rays: returns immediately on first hit.
 *
 * @param bvh BVH structure.
 * @param ray Ray to test.
 * @param max_dist Maximum distance (typically distance to light).
 * @param scene Pointer to the scene.
 * @return 1 if any intersection found, 0 if clear.
 */
int			bvh_intersect_any(t_bvh *bvh, t_ray ray, double max_dist,
				t_scene *scene);

/* Ray direction helper */
/**
 * @brief Check if ray direction is positive along a given axis.
 *
 * @param dir Ray direction vector.
 * @param axis Axis index (0=x, 1=y, 2=z).
 * @return 1 if positive, 0 otherwise.
 */
int			ray_goes_positive(t_vec3 dir, int axis);

/* Object bounds calculation */
/**
 * @brief Retrieve bounds for an object referenced by index.
 *
 * @param ref Object reference containing index.
 * @param scene Pointer to the scene.
 * @return Bounding box for the referenced object.
 */
t_aabb		get_object_bounds(t_object_ref ref, t_scene *scene);

/**
 * @brief Get the geometric center of an object referenced by index.
 *
 * @param ref Object reference containing index.
 * @param scene Pointer to the scene.
 * @return Center point of the object.
 */
t_vec3		get_object_center(t_object_ref ref, t_scene *scene);

/* Scene BVH initialization */
/**
 * @brief Build or rebuild the BVH for the scene with plane separation.
 *
 * Separates planes from bounded objects, builds BVH with bounded
 * objects only, and stores plane indices for separate intersection.
 *
 * @param scene Scene containing objects and BVH state.
 */
void		build_scene_bvh(t_scene *scene);

#endif
