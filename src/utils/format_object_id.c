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

/**
 * @brief Return the prefix string for an object type.
 *
 * @param type Object type.
 * @return char* Prefix string (e.g., "sp-").
 */
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

/**
 * @brief Format an object ID into a buffer.
 *
 * Builds a prefix based on type and appends a 1-based index.
 *
 * @param buffer Destination buffer.
 * @param type Object type.
 * @param index Zero-based index.
 * @return int 1 on success, 0 on failure.
 */
int	format_object_id(char *buffer, t_object_type type, int index)
{
	char	*prefix;

	prefix = get_object_type_prefix(type);
	return (format_id(buffer, 8, prefix, index + 1));
}
