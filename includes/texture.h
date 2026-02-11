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

# include "objects.h"
# include "ray.h"

# define CHECKER_SCALE 2.0

/* Bump map texture data loaded from XPM */
struct s_bump_map
{
	void	*img;
	char	*data;
	int		width;
	int		height;
	int		bpp;
	int		size_line;
	int		endian;
};

/* Checkerboard pattern */
t_color		checkerboard_color(t_object *obj, t_hit *hit);

/* Bump map loading and destruction */
t_bump_map	*bump_map_load(void *mlx, char *filepath);
void		bump_map_destroy(void *mlx, t_bump_map *bmap);

/* Bump map normal perturbation */
t_vec3		bump_perturb_normal(t_object *obj, t_hit *hit);

/* Deferred bump map loading for all objects */
int			load_all_bump_maps(void *scene_ptr, void *mlx);
void		cleanup_all_bump_maps(void *scene_ptr, void *mlx);

#endif
