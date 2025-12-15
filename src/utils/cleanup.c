/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/15 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include <stdlib.h>

void	cleanup_scene(t_scene *scene)
{
	if (!scene)
		return ;
	free(scene);
}

void	cleanup_render(void *param)
{
	t_render	*render;

	render = (t_render *)param;
	if (!render)
		return ;
	free(render);
}

void	cleanup_all(t_scene *scene, void *param)
{
	cleanup_render(param);
	cleanup_scene(scene);
}
