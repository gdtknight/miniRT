/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_internal.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:50:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 18:50:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BVH_INTERNAL_H
# define BVH_INTERNAL_H

# include "spatial/spatial.h"

/* Internal BVH lifecycle functions */
void		bvh_node_destroy(t_bvh_node *node);

/* Internal BVH build partition functions */
t_aabb		compute_bounds(t_object_ref *objects, int count, void *scene);
int			partition_objects(t_partition_params *params);

/* Internal BVH build split functions */
int			choose_split_axis(t_aabb bounds);
double		calculate_split_position(t_aabb bounds, int axis);
t_bvh_node	*create_split_node(t_split_params *sp);

/* Internal BVH build core functions */
t_bvh_node	*create_leaf_node(t_object_ref *objects, int count, void *scene);

#endif
