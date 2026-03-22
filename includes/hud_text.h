/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_text.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUD_TEXT_H
# define HUD_TEXT_H

# include "minirt.h"

void	hud_mark_dirty(t_render *render);
void	hud_print_line(t_render *r, int *y, int color, char *text);
void	format_and_print_vec3(t_render *render, int *y,
			char *label, t_vec3 vec);
int		hud_append(char *dst, const char *src, int max);

#endif
