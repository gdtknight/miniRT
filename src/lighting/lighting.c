/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lighting.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:59 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 11:45:20 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "vec3.h"
#include "ray.h"
#include "shadow.h"
#include <math.h>

/**
 * @brief Clamp RGB color components into the valid 0–255 range.
 *
 * Prevents overflow or wraparound when writing final pixel colors.
 *
 * @param result Color to clamp in place.
 */
static void	clamp_color(t_color *result)
{
	if (result->r > 255)
		result->r = 255;
	if (result->g > 255)
		result->g = 255;
	if (result->b > 255)
		result->b = 255;
}

/**
 * @brief Compute x^32 using repeated squaring.
 *
 * Faster alternative to pow(x, 32.0) for integer exponent.
 *
 * @param x Base value.
 * @return double x raised to the power of 32.
 */
static double	fast_pow32(double x)
{
	x = x * x;
	x = x * x;
	x = x * x;
	x = x * x;
	x = x * x;
	return (x);
}

/**
 * @brief Compute Phong specular term for a surface point.
 *
 * Calculates the reflection vector and raises the view alignment to a
 * fixed shininess exponent to produce specular highlights.
 *
 * @param light_dir Normalized direction toward the light.
 * @param normal Surface normal at the hit point.
 * @param view_dir Normalized direction toward the camera.
 * @return double Specular intensity in [0, 1].
 */
static double	calculate_specular(t_vec3 light_dir, t_vec3 normal,
		t_vec3 view_dir)
{
	t_vec3	reflect_dir;
	double	spec;
	double	dot_ln;

	dot_ln = vec3_dot(light_dir, normal);
	reflect_dir = vec3_subtract(vec3_multiply(normal, 2.0 * dot_ln),
			light_dir);
	spec = vec3_dot(reflect_dir, view_dir);
	if (spec < 0.0)
		spec = 0.0;
	spec = fast_pow32(spec);
	return (spec);
}

/**
 * @brief Compute diffuse/specular lighting factor with shadow attenuation.
 *
 * Evaluates Lambertian diffuse term, Phong specular term, and applies
 * shadow factor to reduce illumination in occluded regions.
 *
 * @param scene Scene containing light and shadow configuration.
 * @param hit Hit record for the shaded point.
 * @param light_dir Normalized direction toward the light.
 * @param view_dir Normalized direction toward the camera.
 * @return double Combined lighting factor.
 */
static double	calc_lighting_factor(t_scene *scene, t_hit *hit,
		t_vec3 light_dir, t_vec3 view_dir)
{
	double			diffuse;
	double			specular;
	double			shadow_factor;
	t_shadow_query	query;

	diffuse = vec3_dot(hit->normal, light_dir);
	if (diffuse < 0)
		diffuse = 0;
	query = (t_shadow_query){hit->point, hit->normal};
	shadow_factor = calculate_shadow_factor(scene, query,
			scene->light.position, &scene->shadow_config);
	specular = calculate_specular(light_dir, hit->normal, view_dir) * 0.5;
	return ((diffuse + specular) * (1.0 - shadow_factor));
}

/**
 * @brief Apply lighting model to compute final shaded color.
 *
 * Combines ambient contribution with diffuse and specular lighting and
 * clamps the resulting RGB components to the displayable range.
 *
 * @param scene Scene containing lights and camera.
 * @param hit Hit record describing intersection point and material color.
 * @return t_color Final shaded color.
 */
t_color	apply_lighting(t_scene *scene, t_hit *hit)
{
	t_vec3	light_dir;
	t_vec3	view_dir;
	double	lighting_factor;
	t_color	result;

	light_dir = vec3_normalize(vec3_subtract(scene->light.position,
				hit->point));
	view_dir = vec3_normalize(vec3_subtract(scene->camera.position,
				hit->point));
	lighting_factor = calc_lighting_factor(scene, hit, light_dir, view_dir);
	result.r = (hit->color.r / 255.0) * (scene->ambient.ratio
			* (scene->ambient.color.r / 255.0)
			+ lighting_factor * scene->light.brightness
			* (scene->light.color.r / 255.0)) * 255.0;
	result.g = (hit->color.g / 255.0) * (scene->ambient.ratio
			* (scene->ambient.color.g / 255.0)
			+ lighting_factor * scene->light.brightness
			* (scene->light.color.g / 255.0)) * 255.0;
	result.b = (hit->color.b / 255.0) * (scene->ambient.ratio
			* (scene->ambient.color.b / 255.0)
			+ lighting_factor * scene->light.brightness
			* (scene->light.color.b / 255.0)) * 255.0;
	clamp_color(&result);
	return (result);
}
