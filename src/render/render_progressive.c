/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_progressive.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:15:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 14:15:36 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render_progressive.h"
#include "window.h"

/**
 * @brief Initialize progressive rendering state for tiled rendering.
 *
 * Computes the total number of tiles for the given dimensions and tile size,
 * resets counters, and enables progressive rendering.
 *
 * @param prog Progressive state to initialize.
 * @param width Target render width.
 * @param height Target render height.
 * @param tile_size Square tile size in pixels.
 */
void	progressive_init(t_progressive_state *prog, int width, int height,
		int tile_size)
{
	int	tiles_x;
	int	tiles_y;

	prog->tile_size = tile_size;
	prog->width = width;
	prog->height = height;
	tiles_x = (width + tile_size - 1) / tile_size;
	tiles_y = (height + tile_size - 1) / tile_size;
	prog->total_tiles = tiles_x * tiles_y;
	prog->current_tile = 0;
	prog->tiles_completed = 0;
	prog->enabled = 1;
}

/**
 * @brief Compute the next tile rectangle to render.
 *
 * Converts the current tile index into a tile rectangle and clamps the
 * rectangle to the render bounds. Advances the tile index on success.
 *
 * @param prog Progressive state providing tile counters.
 * @param rect Output rectangle describing the next tile.
 * @return int 1 if a tile is available, 0 if finished or disabled.
 */
int	progressive_next_tile(t_progressive_state *prog, t_tile_rect *rect)
{
	int	tiles_x;
	int	tile_x;
	int	tile_y;

	if (!prog->enabled || prog->current_tile >= prog->total_tiles)
		return (0);
	tiles_x = (prog->width + prog->tile_size - 1) / prog->tile_size;
	tile_x = prog->current_tile % tiles_x;
	tile_y = prog->current_tile / tiles_x;
	rect->x = tile_x * prog->tile_size;
	rect->y = tile_y * prog->tile_size;
	rect->w = prog->tile_size;
	rect->h = prog->tile_size;
	if (rect->x + rect->w > prog->width)
		rect->w = prog->width - rect->x;
	if (rect->y + rect->h > prog->height)
		rect->h = prog->height - rect->y;
	prog->current_tile++;
	return (1);
}

/**
 * @brief Reset progressive rendering counters.
 *
 * Resets the current tile index and completed count without changing
 * the enabled state or tile size.
 *
 * @param prog Progressive state to reset.
 */
void	progressive_reset(t_progressive_state *prog)
{
	prog->current_tile = 0;
	prog->tiles_completed = 0;
}
