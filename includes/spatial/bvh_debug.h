/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_debug.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 15:09:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 15:09:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BVH_DEBUG_H
# define BVH_DEBUG_H

# include "spatial/spatial.h"

/**
 * @brief Visualize the BVH tree in the terminal.
 *
 * Prints tree structure and statistics to stdout.
 *
 * @param bvh BVH to visualize.
 * @param scene Scene for object label lookup.
 */
void	bvhd_run(t_bvh *bvh, t_scene *scene);

#endif
