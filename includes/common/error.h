/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
# define ERROR_H

/**
 * @brief Error codes for miniRT
 */
typedef enum e_error_code
{
	ERR_NONE = 0,
	ERR_MALLOC,
	ERR_FILE_OPEN,
	ERR_FILE_READ,
	ERR_FILE_EXT,
	ERR_PARSE_FORMAT,
	ERR_PARSE_AMBIENT,
	ERR_PARSE_CAMERA,
	ERR_PARSE_LIGHT,
	ERR_PARSE_SPHERE,
	ERR_PARSE_PLANE,
	ERR_PARSE_CYLINDER,
	ERR_PARSE_DUPLICATE,
	ERR_PARSE_MISSING,
	ERR_WINDOW_INIT,
	ERR_IMAGE_INIT,
	ERR_BVH_BUILD,
	ERR_INVALID_PARAM,
	ERR_COUNT
}	t_error_code;

/* Error handling functions */
void		error_write_str(const char *str);
int			error_print(t_error_code code);
const char	*error_get_message(t_error_code code);
void		error_exit(t_error_code code);
void		error_print_ctx(const char *prefix, const char *detail);

#endif
