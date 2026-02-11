/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bump_map_perturb.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/10 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "texture.h"
#include "vec3.h"
#include <math.h>

static double	sample_height(t_bump_map *bmap, int px, int py)
{
	int		offset;
	char	*pixel;

	offset = py * bmap->size_line + px * (bmap->bpp / 8);
	pixel = bmap->data + offset;
	return ((unsigned char)pixel[0] / 255.0);
}

static void	get_surface_uv(t_object *obj, t_hit *hit, double *uv)
{
	t_vec3	local;
	t_vec3	tangent;

	if (obj->type == OBJ_SPHERE)
	{
		local = vec3_normalize(vec3_subtract(hit->point,
					obj->data.sphere.center));
		uv[0] = 0.5 + atan2(local.z, local.x) / (2.0 * M_PI);
		uv[1] = 0.5 - asin(local.y) / M_PI;
		return ;
	}
	tangent = get_tangent(hit->normal);
	uv[0] = vec3_dot(hit->point, tangent) * 0.5;
	uv[1] = vec3_dot(hit->point,
			vec3_cross(hit->normal, tangent)) * 0.5;
}

static void	compute_gradient(t_bump_map *bmap, double *uv, double *grad)
{
	int		px;
	int		py;
	double	h;

	px = (int)(uv[0] * bmap->width) % bmap->width;
	py = (int)(uv[1] * bmap->height) % bmap->height;
	if (px < 0)
		px += bmap->width;
	if (py < 0)
		py += bmap->height;
	h = sample_height(bmap, px, py);
	grad[0] = sample_height(bmap, (px + 1) % bmap->width, py) - h;
	grad[1] = sample_height(bmap, px, (py + 1) % bmap->height) - h;
}

t_vec3	bump_perturb_normal(t_object *obj, t_hit *hit)
{
	double	uv[2];
	double	grad[2];
	t_vec3	tangent;
	t_vec3	bitangent;
	t_vec3	perturb;

	get_surface_uv(obj, hit, uv);
	compute_gradient(obj->bump_map, uv, grad);
	tangent = get_tangent(hit->normal);
	bitangent = vec3_cross(hit->normal, tangent);
	perturb = vec3_add(vec3_multiply(tangent, grad[0]),
			vec3_multiply(bitangent, grad[1]));
	return (vec3_normalize(vec3_add(hit->normal, perturb)));
}
