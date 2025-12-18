/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lighting.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:59 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "vec3.h"
#include "ray.h"
#include "shadow.h"
#include <math.h>

/*
** Clamp color values to valid range [0, 255].
** Prevents overflow in final pixel color values.
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

/*
** Calculate specular reflection (Phong model).
** Creates shiny highlights on surfaces.
*/
static double	calculate_specular(t_vec3 light_dir, t_vec3 normal, \
		t_vec3 view_dir)
{
	t_vec3	reflect_dir;
	double	spec;
	double	dot_ln;

	dot_ln = vec3_dot(light_dir, normal);
	reflect_dir = vec3_subtract(vec3_multiply(normal, 2.0 * dot_ln), \
		light_dir);
	spec = vec3_dot(reflect_dir, view_dir);
	if (spec < 0.0)
		spec = 0.0;
	spec = pow(spec, 32.0);
	return (spec);
}

/*
** Calculate combined lighting factor (diffuse + specular - shadow).
*/
static double	calc_lighting_factor(t_scene *scene, t_hit *hit, \
		t_vec3 light_dir, t_vec3 view_dir)
{
	double	diffuse;
	double	specular;
	double	shadow_factor;

	diffuse = vec3_dot(hit->normal, light_dir);
	if (diffuse < 0)
		diffuse = 0;
	shadow_factor = calculate_shadow_factor(scene, hit->point, \
		scene->light.position, &scene->shadow_config);
	specular = calculate_specular(light_dir, hit->normal, view_dir) * 0.5;
	return ((diffuse + specular) * (1.0 - shadow_factor));
}

/*
** Apply Phong lighting model to calculate final color at hit point.
** Combines ambient light with diffuse reflection and specular highlights.
** Takes into account shadow factor to darken occluded areas.
*/
t_color	apply_lighting(t_scene *scene, t_hit *hit)
{
	t_vec3	light_dir;
	t_vec3	view_dir;
	double	lighting_factor;
	t_color	result;

	light_dir = vec3_normalize(vec3_subtract(scene->light.position, \
		hit->point));
	view_dir = vec3_normalize(vec3_subtract(scene->camera.position, \
		hit->point));
	lighting_factor = calc_lighting_factor(scene, hit, light_dir, view_dir);
	result.r = hit->color.r * (scene->ambient.ratio + \
		lighting_factor * scene->light.brightness);
	result.g = hit->color.g * (scene->ambient.ratio + \
		lighting_factor * scene->light.brightness);
	result.b = hit->color.b * (scene->ambient.ratio + \
		lighting_factor * scene->light.brightness);
	clamp_color(&result);
	return (result);
}
