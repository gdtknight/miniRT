/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bounds.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "minirt.h"
#include <math.h>

/**
 * @brief Compute AABB for a sphere.
 *
 * @param s Sphere data.
 * @return t_aabb Bounding box of the sphere.
 */
static t_aabb	bounds_for_sphere(t_sphere_data *s)
{
	t_vec3	rad_vec;

	rad_vec.x = s->radius;
	rad_vec.y = s->radius;
	rad_vec.z = s->radius;
	return (aabb_create(vec3_subtract(s->center, rad_vec),
			vec3_add(s->center, rad_vec)));
}

/**
 * @brief Compute AABB for a finite cylinder.
 *
 * @param c Cylinder data.
 * @return t_aabb Bounding box of the cylinder.
 */
static t_aabb	bounds_for_cylinder(t_cylinder_data *c)
{
	t_vec3	half_axis;
	t_vec3	min;
	t_vec3	max;

	half_axis = vec3_multiply(c->axis, c->half_height);
	min.x = c->center.x - c->radius - fabs(half_axis.x);
	min.y = c->center.y - c->radius - fabs(half_axis.y);
	min.z = c->center.z - c->radius - fabs(half_axis.z);
	max.x = c->center.x + c->radius + fabs(half_axis.x);
	max.y = c->center.y + c->radius + fabs(half_axis.y);
	max.z = c->center.z + c->radius + fabs(half_axis.z);
	return (aabb_create(min, max));
}

/**
 * @brief Compute a large AABB approximation for a plane.
 *
 * @param p Plane data (unused in approximation).
 * @return t_aabb Large bounding box.
 */
static t_aabb	bounds_for_plane(t_plane_data *p)
{
	double	large;

	large = 1000000.0;
	(void)p;
	return (aabb_create((t_vec3){-large, -large, -large},
		(t_vec3){large, large, large}));
}

/**
 * @brief Compute an object's bounding box based on its type.
 *
 * @param obj Object to bound.
 * @return t_aabb Bounding box for the object.
 */
t_aabb	aabb_for_object(t_object *obj)
{
	if (obj->type == OBJ_SPHERE)
		return (bounds_for_sphere(&obj->data.sphere));
	else if (obj->type == OBJ_CYLINDER)
		return (bounds_for_cylinder(&obj->data.cylinder));
	else
		return (bounds_for_plane(&obj->data.plane));
}

/**
 * @brief Retrieve bounds for an object referenced by index.
 *
 * @param ref Object reference containing index.
 * @param scene_ptr Pointer to the scene.
 * @return t_aabb Bounding box for the referenced object.
 */
t_aabb	get_object_bounds(t_object_ref ref, void *scene_ptr)
{
	t_scene		*scene;
	t_object	*obj;

	scene = (t_scene *)scene_ptr;
	obj = &scene->objects.items[ref.index];
	return (aabb_for_object(obj));
}
