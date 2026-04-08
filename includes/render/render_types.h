/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_types.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/04/08 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RENDER_TYPES_H
# define RENDER_TYPES_H

# include "common/objects.h"

/* Forward declaration */
typedef struct s_render	t_render;

/* Selected object information */
typedef struct s_selection
{
	t_object_type	type;
	int				index;
}	t_selection;

/* HUD state structure */
typedef struct s_hud_state
{
	int		visible;
	int		current_page;
	int		total_pages;
	int		dirty;
}	t_hud_state;

/* Key guide state structure */
typedef struct s_keyguide_state
{
	int		visible;
	int		x;
	int		y;
	int		dirty;
}	t_keyguide_state;

/* Key dispatch table */
# define KEY_BIND_COUNT 38

typedef void			(*t_key_handler)(t_render *, int);

typedef struct s_key_bind
{
	int				keycode;
	t_key_handler	handler;
}	t_key_bind;

typedef struct s_key_binds
{
	t_key_bind	entries[KEY_BIND_COUNT];
	int			count;
}	t_key_binds;

#endif
