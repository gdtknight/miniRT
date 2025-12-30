/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_state.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 21:30:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RENDER_STATE_H
# define RENDER_STATE_H

# include "metrics.h"
# include "spatial.h"
# include <sys/time.h>

/* Quality mode for adaptive rendering */
typedef enum e_quality_mode
{
	QUALITY_LOW,
	QUALITY_HIGH
}	t_quality_mode;

/* Interaction state for adaptive quality */
typedef struct s_interaction_state
{
	int				is_interacting;
	struct timeval	last_interaction;
	int				interaction_count;
}	t_interaction_state;

/* Progressive rendering state */
typedef struct s_progressive_state
{
	int		enabled;
	int		current_tile;
	int		total_tiles;
	int		tiles_completed;
	int		tile_size;
}	t_progressive_state;

/* Complete render state management */
typedef struct s_render_state
{
	t_quality_mode			quality;
	t_quality_mode			target_quality;
	t_interaction_state		interaction;
	t_progressive_state		progressive;
	t_metrics				metrics;
	t_bvh					*bvh;
	int						adaptive_enabled;
	int						bvh_enabled;
	int						show_info;
}	t_render_state;

/* Render state operations */
void	render_state_init(t_render_state *state);
void	render_state_cleanup(t_render_state *state);
void	render_state_update(t_render_state *state);

/* Quality management */
void	quality_set_mode(t_render_state *state, t_quality_mode mode);
int		quality_should_upgrade(t_render_state *state);
void	quality_handle_interaction(t_render_state *state);

/* Progressive rendering */
void	progressive_init(t_progressive_state *prog, int width, int height,
			int tile_size);
int		progressive_next_tile(t_progressive_state *prog, int *x, int *y,
			int *w, int *h);
void	progressive_reset(t_progressive_state *prog);

#endif
