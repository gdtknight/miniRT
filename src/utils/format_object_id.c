/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   format_object_id.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/29 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "format_object_id.h"
#include "utils.h"

char	*get_object_type_prefix(t_object_type type)
{
	if (type == OBJ_SPHERE)
		return ("sp-");
	else if (type == OBJ_PLANE)
		return ("pl-");
	else if (type == OBJ_CYLINDER)
		return ("cy-");
	return ("xx-");
}

int	format_object_id(char *buffer, t_object_type type, int index)
{
	char	*prefix;

	prefix = get_object_type_prefix(type);
	return (format_id(buffer, 8, prefix, index + 1));
}
