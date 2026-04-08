/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shading.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:59 by yoshin            #+#    #+#             */
/*   Updated: 2026/04/09 00:44:23 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lighting/shading.h"
#include "lighting/shadow.h"
#include "lighting/texture.h"

/**
 * @brief Compute the Phong specular intensity.
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
 * @brief Compute combined diffuse, specular, and shadow factor
 *        for a single light source.
 */
static double	calc_lighting_factor(t_scene *scene, t_hit *hit,
		t_light *light, t_vec3 view_dir)
{
	double			diffuse;
	double			specular;
	double			shadow_factor;
	t_shadow_query	query;
	t_vec3			light_dir;

	light_dir = vec3_normalize(vec3_subtract(light->position, hit->point));
	diffuse = vec3_dot(hit->normal, light_dir);
	if (diffuse < 0)
		diffuse = 0;
	query = (t_shadow_query){hit->point, hit->normal};
	shadow_factor = calculate_shadow_factor(scene, query,
			light->position, &scene->shadow_config);
	specular = calculate_specular(light_dir, hit->normal, view_dir) * 0.5;
	return ((diffuse + specular) * (1.0 - shadow_factor));
}

/**
 * @brief Sum lighting contributions from all lights in the scene.
 */
static void	accumulate_lights(t_scene *scene, t_hit *hit,
		t_vec3 view_dir, t_color_f *acc)
{
	int		i;
	t_light	*light;
	double	factor;

	i = 0;
	while (i < scene->light_count)
	{
		light = &scene->lights[i];
		factor = calc_lighting_factor(scene, hit, light, view_dir);
		acc->r += factor * light->brightness * (light->color.r / 255.0);
		acc->g += factor * light->brightness * (light->color.g / 255.0);
		acc->b += factor * light->brightness * (light->color.b / 255.0);
		i++;
	}
}

/**
 * @brief Compute the final pixel color using Phong shading.
 *
 * Applies checkerboard and bump map textures when present,
 * accumulates per-light contributions, and combines them with
 * ambient light to produce a clamped RGB color.
 *
 * @param scene Scene containing lights and ambient settings.
 * @param hit Hit record with surface point, normal, and color.
 * @return t_color Final clamped RGB color.
 */
t_color	apply_lighting(t_scene *scene, t_hit *hit)
{
	t_vec3		view_dir;
	t_color_f	acc;
	t_color		result;

	acc = (t_color_f){0, 0, 0};
	if (hit->obj && hit->obj->has_checker)
		hit->color = checkerboard_color(hit->obj, hit);
	if (hit->obj && hit->obj->bump_map)
		hit->normal = bump_perturb_normal(hit->obj, hit);
	view_dir = vec3_normalize(vec3_subtract(scene->camera.position,
				hit->point));
	accumulate_lights(scene, hit, view_dir, &acc);
	result.r = (hit->color.r / 255.0) * (scene->ambient.ratio
			* (scene->ambient.color.r / 255.0) + acc.r) * 255.0;
	result.g = (hit->color.g / 255.0) * (scene->ambient.ratio
			* (scene->ambient.color.g / 255.0) + acc.g) * 255.0;
	result.b = (hit->color.b / 255.0) * (scene->ambient.ratio
			* (scene->ambient.color.b / 255.0) + acc.b) * 255.0;
	clamp_color(&result);
	return (result);
}
