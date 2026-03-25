/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_intersect.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 00:00:00 by miniRT            #+#    #+#             */
/*   Updated: 2026/03/25 00:00:00 by miniRT           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Requires: src/math/vec3.c  src/math/vec3_ops.c
**           src/intersect/intersect_object.c
**           src/intersect/intersect_cylinder.c
**           src/intersect/intersect_cone_body.c
**           src/intersect/intersect_cone_cap.c
** Headers:  includes/  lib/libft/includes/
** Libraries: -lm  (libft not actually called at runtime)
*/

#include "intersect.h"
#include "vec3.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>

#define HIT_INF  1e20
#define ISECT_EPS 1e-6

/* ---- helpers ---- */

static t_ray	make_ray(double ox, double oy, double oz,
						double dx, double dy, double dz)
{
	t_ray	ray;

	ray.origin.x = ox;
	ray.origin.y = oy;
	ray.origin.z = oz;
	ray.direction.x = dx;
	ray.direction.y = dy;
	ray.direction.z = dz;
	ray.inv_dir.x = (dx != 0.0) ? 1.0 / dx : 1e30;
	ray.inv_dir.y = (dy != 0.0) ? 1.0 / dy : 1e30;
	ray.inv_dir.z = (dz != 0.0) ? 1.0 / dz : 1e30;
	return (ray);
}

static t_hit	make_hit(void)
{
	t_hit	hit;

	hit.distance = HIT_INF;
	hit.obj = NULL;
	hit.point.x = 0.0;
	hit.point.y = 0.0;
	hit.point.z = 0.0;
	hit.normal.x = 0.0;
	hit.normal.y = 0.0;
	hit.normal.z = 0.0;
	hit.color.r = 0;
	hit.color.g = 0;
	hit.color.b = 0;
	return (hit);
}

static t_object	make_sphere(double cx, double cy, double cz, double radius)
{
	t_object	obj;

	obj.type = OBJ_SPHERE;
	obj.data.sphere.center.x = cx;
	obj.data.sphere.center.y = cy;
	obj.data.sphere.center.z = cz;
	obj.data.sphere.radius = radius;
	obj.data.sphere.radius_sq = radius * radius;
	obj.color.r = 255;
	obj.color.g = 0;
	obj.color.b = 0;
	obj.has_checker = 0;
	obj.bump_map = NULL;
	obj.bump_path = NULL;
	return (obj);
}

static t_object	make_plane(double px, double py, double pz,
							double nx, double ny, double nz)
{
	t_object	obj;
	t_vec3		normal;

	obj.type = OBJ_PLANE;
	obj.data.plane.point.x = px;
	obj.data.plane.point.y = py;
	obj.data.plane.point.z = pz;
	normal = vec3_normalize((t_vec3){nx, ny, nz});
	obj.data.plane.normal = normal;
	obj.color.r = 0;
	obj.color.g = 255;
	obj.color.b = 0;
	obj.has_checker = 0;
	obj.bump_map = NULL;
	obj.bump_path = NULL;
	return (obj);
}

static t_object	make_cylinder(double cx, double cy, double cz,
								double ax, double ay, double az,
								double radius, double height)
{
	t_object	obj;
	t_vec3		axis;

	obj.type = OBJ_CYLINDER;
	obj.data.cylinder.center.x = cx;
	obj.data.cylinder.center.y = cy;
	obj.data.cylinder.center.z = cz;
	axis = vec3_normalize((t_vec3){ax, ay, az});
	obj.data.cylinder.axis = axis;
	obj.data.cylinder.radius = radius;
	obj.data.cylinder.radius_sq = radius * radius;
	obj.data.cylinder.half_height = height / 2.0;
	obj.color.r = 0;
	obj.color.g = 0;
	obj.color.b = 255;
	obj.has_checker = 0;
	obj.bump_map = NULL;
	obj.bump_path = NULL;
	return (obj);
}

static t_object	make_cone(double cx, double cy, double cz,
							double ax, double ay, double az,
							double radius, double height)
{
	t_object	obj;
	t_vec3		axis;

	obj.type = OBJ_CONE;
	obj.data.cone.center.x = cx;
	obj.data.cone.center.y = cy;
	obj.data.cone.center.z = cz;
	axis = vec3_normalize((t_vec3){ax, ay, az});
	obj.data.cone.axis = axis;
	obj.data.cone.radius = radius;
	obj.data.cone.radius_sq = radius * radius;
	obj.data.cone.half_height = height / 2.0;
	obj.color.r = 255;
	obj.color.g = 255;
	obj.color.b = 0;
	obj.has_checker = 0;
	obj.bump_map = NULL;
	obj.bump_path = NULL;
	return (obj);
}

/* ---- sphere tests ---- */

static void	test_sphere_direct_hit(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing sphere - direct hit from (-z)...\n");
	ray = make_ray(0.0, 0.0, -5.0, 0.0, 0.0, 1.0);
	obj = make_sphere(0.0, 0.0, 0.0, 1.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 1);
	assert(fabs(hit.distance - 4.0) < ISECT_EPS);
	assert(fabs(hit.point.z - (-1.0)) < ISECT_EPS);
	assert(fabs(hit.normal.z - (-1.0)) < ISECT_EPS);
	assert(hit.obj == &obj);
	printf("  ✓ sphere direct hit: dist=%.4f, normal.z=%.4f\n",
		hit.distance, hit.normal.z);
}

static void	test_sphere_miss(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing sphere - miss (ray offset above)...\n");
	ray = make_ray(0.0, 5.0, -5.0, 0.0, 0.0, 1.0);
	obj = make_sphere(0.0, 0.0, 0.0, 1.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 0);
	assert(hit.distance == HIT_INF);
	printf("  ✓ sphere miss: no hit recorded\n");
}

static void	test_sphere_behind_ray(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing sphere - sphere entirely behind ray origin...\n");
	ray = make_ray(0.0, 0.0, 5.0, 0.0, 0.0, 1.0);
	obj = make_sphere(0.0, 0.0, 0.0, 1.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 0);
	printf("  ✓ sphere behind ray: no hit\n");
}

static void	test_sphere_ray_inside(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing sphere - ray origin inside sphere...\n");
	ray = make_ray(0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	obj = make_sphere(0.0, 0.0, 0.0, 2.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 1);
	assert(hit.distance > 0.0);
	printf("  ✓ ray inside sphere: hit at dist=%.4f\n", hit.distance);
}

static void	test_sphere_tangent(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing sphere - tangent ray (grazing)...\n");
	ray = make_ray(0.0, 1.0, -5.0, 0.0, 0.0, 1.0);
	obj = make_sphere(0.0, 0.0, 0.0, 1.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 1);
	assert(hit.distance > 0.0);
	printf("  ✓ tangent sphere: hit at dist=%.4f\n", hit.distance);
}

static void	test_sphere_closer_wins(void)
{
	t_ray		ray;
	t_object	obj1;
	t_object	obj2;
	t_hit		hit;

	printf("Testing sphere - closer hit wins, farther does not update...\n");
	ray = make_ray(0.0, 0.0, -10.0, 0.0, 0.0, 1.0);
	obj1 = make_sphere(0.0, 0.0, 0.0, 1.0);
	obj2 = make_sphere(0.0, 0.0, -6.0, 1.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj2, &hit) == 1);
	assert(intersect_object_new(&ray, &obj1, &hit) == 0);
	assert(hit.obj == &obj2);
	printf("  ✓ closer sphere (obj2 at t=3) wins; farther (obj1 at t=9) skipped\n");
}

/* ---- plane tests ---- */

static void	test_plane_direct_hit(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing plane - ray from above hitting y=0...\n");
	ray = make_ray(0.0, 5.0, 0.0, 0.0, -1.0, 0.0);
	obj = make_plane(0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 1);
	assert(fabs(hit.distance - 5.0) < ISECT_EPS);
	assert(fabs(hit.point.y) < ISECT_EPS);
	assert(hit.obj == &obj);
	printf("  ✓ plane hit: dist=%.4f, point.y=%.6f\n",
		hit.distance, hit.point.y);
}

static void	test_plane_parallel_ray(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing plane - ray parallel to plane...\n");
	ray = make_ray(0.0, 1.0, 0.0, 1.0, 0.0, 0.0);
	obj = make_plane(0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 0);
	printf("  ✓ parallel ray: no hit\n");
}

static void	test_plane_normal_flipped_from_below(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing plane - ray from below (normal should flip)...\n");
	ray = make_ray(0.0, -5.0, 0.0, 0.0, 1.0, 0.0);
	obj = make_plane(0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 1);
	assert(fabs(hit.distance - 5.0) < ISECT_EPS);
	assert(hit.normal.y < 0.0);
	printf("  ✓ plane normal flipped: normal.y=%.4f\n", hit.normal.y);
}

static void	test_plane_oblique_hit(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;
	double		sq2;

	printf("Testing plane - oblique ray at 45 degrees...\n");
	sq2 = sqrt(2.0) / 2.0;
	ray = make_ray(0.0, 5.0, -5.0, 0.0, -sq2, sq2);
	obj = make_plane(0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 1);
	assert(hit.distance > 0.0);
	assert(fabs(hit.point.y) < ISECT_EPS);
	printf("  ✓ oblique plane hit: dist=%.4f\n", hit.distance);
}

/* ---- cylinder tests ---- */

static void	test_cylinder_body_hit(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing cylinder - ray hits body from side...\n");
	ray = make_ray(3.0, 0.0, 0.0, -1.0, 0.0, 0.0);
	obj = make_cylinder(0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 4.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 1);
	assert(fabs(hit.distance - 2.0) < ISECT_EPS);
	assert(fabs(hit.point.x - 1.0) < ISECT_EPS);
	assert(hit.obj == &obj);
	printf("  ✓ cylinder body hit: dist=%.4f, point.x=%.4f\n",
		hit.distance, hit.point.x);
}

static void	test_cylinder_miss_wide(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing cylinder - ray passes wide (miss)...\n");
	ray = make_ray(5.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	obj = make_cylinder(0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 4.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 0);
	printf("  ✓ cylinder wide miss: no hit\n");
}

static void	test_cylinder_miss_height(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing cylinder - ray misses outside height range...\n");
	ray = make_ray(3.0, 5.0, 0.0, -1.0, 0.0, 0.0);
	obj = make_cylinder(0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 4.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 0);
	printf("  ✓ cylinder out-of-height miss: no hit\n");
}

static void	test_cylinder_cap_hit(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing cylinder - ray hits top cap...\n");
	ray = make_ray(0.0, 5.0, 0.0, 0.0, -1.0, 0.0);
	obj = make_cylinder(0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 4.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 1);
	assert(hit.distance > 0.0);
	assert(fabs(hit.point.y - 2.0) < ISECT_EPS);
	printf("  ✓ cylinder top cap hit: dist=%.4f\n", hit.distance);
}

static void	test_cylinder_bottom_cap_hit(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing cylinder - ray hits bottom cap...\n");
	ray = make_ray(0.0, -5.0, 0.0, 0.0, 1.0, 0.0);
	obj = make_cylinder(0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 4.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 1);
	assert(hit.distance > 0.0);
	assert(fabs(hit.point.y - (-2.0)) < ISECT_EPS);
	printf("  ✓ cylinder bottom cap hit: dist=%.4f\n", hit.distance);
}

/* ---- cone tests ---- */

static void	test_cone_body_hit(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing cone - ray hits cone body from side...\n");
	ray = make_ray(3.0, 0.0, 0.0, -1.0, 0.0, 0.0);
	obj = make_cone(0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 2.0, 4.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 1);
	assert(hit.distance > 0.0);
	printf("  ✓ cone body hit: dist=%.4f\n", hit.distance);
}

static void	test_cone_miss(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing cone - ray misses cone entirely...\n");
	ray = make_ray(10.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	obj = make_cone(0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 2.0, 4.0);
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 0);
	printf("  ✓ cone miss: no hit\n");
}

/* ---- object dispatch tests ---- */

static void	test_dispatch_unknown_type(void)
{
	t_ray		ray;
	t_object	obj;
	t_hit		hit;

	printf("Testing dispatch - OBJ_NONE returns 0...\n");
	ray = make_ray(0.0, 0.0, -5.0, 0.0, 0.0, 1.0);
	obj = make_sphere(0.0, 0.0, 0.0, 1.0);
	obj.type = OBJ_NONE;
	hit = make_hit();
	assert(intersect_object_new(&ray, &obj, &hit) == 0);
	printf("  ✓ OBJ_NONE: no intersection dispatched\n");
}

/* ---- main ---- */

int	main(void)
{
	printf("\n=== Intersection Unit Tests ===\n\n");

	printf("--- sphere ---\n");
	test_sphere_direct_hit();
	test_sphere_miss();
	test_sphere_behind_ray();
	test_sphere_ray_inside();
	test_sphere_tangent();
	test_sphere_closer_wins();

	printf("\n--- plane ---\n");
	test_plane_direct_hit();
	test_plane_parallel_ray();
	test_plane_normal_flipped_from_below();
	test_plane_oblique_hit();

	printf("\n--- cylinder ---\n");
	test_cylinder_body_hit();
	test_cylinder_miss_wide();
	test_cylinder_miss_height();
	test_cylinder_cap_hit();
	test_cylinder_bottom_cap_hit();

	printf("\n--- cone ---\n");
	test_cone_body_hit();
	test_cone_miss();

	printf("\n--- dispatch ---\n");
	test_dispatch_unknown_type();

	printf("\n=== All intersection tests passed! ===\n\n");
	return (0);
}
