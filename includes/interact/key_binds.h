/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_binds.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/04/08 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KEY_BINDS_H
# define KEY_BINDS_H

/* Forward declaration */
typedef struct s_render	t_render;

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
