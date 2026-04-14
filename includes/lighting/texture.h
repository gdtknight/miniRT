/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/02/10 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEXTURE_H
# define TEXTURE_H

# include "common/objects.h"
# include "common/ray.h"

# define CHECKER_SCALE 2.0

/* Bump map texture data loaded from XPM */
typedef struct s_bump_map
{
	void	*img;
	char	*data;
	int		width;
	int		height;
	int		bpp;
	int		size_line;
	int		endian;
}	t_bump_map;

/* Tangent vector utility for UV computation */
/**
 * @brief Compute a tangent vector perpendicular to a normal.
 *
 * Uses the world Y axis as reference, falling back to the X axis
 * when the normal is nearly parallel to Y.
 *
 * @param n Surface normal (assumed normalized).
 * @return Normalized tangent vector.
 */
t_vec3		get_tangent(t_vec3 n);

/* Checkerboard pattern */
/**
 * @brief Return the checkerboard-modulated color for an object.
 *
 * Dispatches to the appropriate checker function based on the
 * object type. Falls back to the base color for unknown types.
 *
 * @param obj Object with checkerboard enabled.
 * @param hit Hit record with the surface point.
 * @return Checkerboard-patterned color.
 */
t_color		checkerboard_color(t_object *obj, t_hit *hit);

/* Bump map normal perturbation */
/**
 * @brief Perturb the surface normal using a bump map.
 *
 * Samples the bump map gradient at the hit point UV and offsets
 * the normal along the tangent and bitangent directions.
 *
 * @param obj Object with an attached bump map.
 * @param hit Hit record whose normal will be perturbed.
 * @return Perturbed and normalized surface normal.
 */
t_vec3		bump_perturb_normal(t_object *obj, t_hit *hit);

/* Deferred bump map loading for all objects */
/**
 * @brief Load bump maps for all objects that have a bump path.
 *
 * @param scene_ptr Opaque pointer to the scene.
 * @param mlx MLX connection pointer.
 * @return 1 on success, 0 if any bump map fails to load.
 */
int			load_all_bump_maps(void *scene_ptr, void *mlx);

/**
 * @brief Destroy and free all bump maps and their file paths.
 *
 * @param scene_ptr Opaque pointer to the scene.
 * @param mlx MLX connection pointer.
 */
void		cleanup_all_bump_maps(void *scene_ptr, void *mlx);

#endif
