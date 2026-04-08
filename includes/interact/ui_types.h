/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_types.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/04/08 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UI_TYPES_H
# define UI_TYPES_H

# include "common/objects.h"

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

#endif
