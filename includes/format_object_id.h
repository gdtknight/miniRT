/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   format_object_id.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FORMAT_OBJECT_ID_H
# define FORMAT_OBJECT_ID_H

# include "window.h"

int		format_object_id(char *buffer, t_object_type type, int index);
char	*get_object_type_prefix(t_object_type type);

#endif
