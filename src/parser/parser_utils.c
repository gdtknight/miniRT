/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"

const char	*skip_whitespace(const char *str)
{
	while (*str == ' ' || *str == '\t')
		str++;
	return (str);
}

int	at_line_end(const char *str)
{
	str = skip_whitespace(str);
	return (*str == '\0' || *str == '\n' || *str == '#');
}

/**
 * @brief Initialize line reader with file descriptor.
 *
 * @param reader Line reader to initialize.
 * @param fd File descriptor to read from.
 * @return int 1 on success, 0 on failure.
 */
void	line_reader_init(t_line_reader *reader, int fd)
{
	reader->fd = fd;
	reader->buf_pos = 0;
	reader->buf_len = 0;
	reader->line_num = 0;
	reader->line_too_long = 0;
	reader->error = 0;
}

/**
 * @brief Get current line number.
 *
 * @param reader Line reader.
 * @return int Current line number (1-based) or 0 if not started.
 */
int	line_reader_get_line_num(t_line_reader *reader)
{
	if (!reader)
		return (0);
	return (reader->line_num);
}

/**
 * @brief Validate that a file name ends with ".rt".
 *
 * @param filename Path to the scene file.
 * @return int 1 if extension matches, 0 otherwise.
 */
int	validate_extension(const char *filename)
{
	int	len;

	len = 0;
	while (filename[len])
		len++;
	if (len < 4)
		return (0);
	if (filename[len - 3] != '.' || filename[len - 2] != 'r'
		|| filename[len - 1] != 't')
		return (0);
	return (1);
}
