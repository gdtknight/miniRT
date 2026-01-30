/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyguide_cleanup.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/30 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "keyguide.h"
#include "window.h"

/**
 * @brief Destroy key guide resources.
 *
 * Frees the background image and clears cached pointers.
 *
 * @param keyguide Key guide state to clean up.
 * @param mlx MLX connection pointer.
 */
void	keyguide_cleanup(t_keyguide_state *keyguide, void *mlx)
{
	if (keyguide->bg_img)
	{
		mlx_destroy_image(mlx, keyguide->bg_img);
		keyguide->bg_img = NULL;
		keyguide->bg_data = NULL;
	}
}
