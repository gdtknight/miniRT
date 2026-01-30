/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line_reader.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include <unistd.h>
#include <stdlib.h>

/**
 * @brief Refill the read buffer from file.
 *
 * @param reader Line reader.
 * @return int Number of bytes read, 0 on EOF, -1 on error.
 */
static int	refill_buffer(t_line_reader *reader)
{
	reader->buf_len = read(reader->fd, reader->buffer,
			LINE_READER_BUFFER_SIZE);
	reader->buf_pos = 0;
	return (reader->buf_len);
}

/**
 * @brief Append character to line buffer with bounds check.
 *
 * @param line Line buffer.
 * @param pos Current position in buffer.
 * @param c Character to append.
 * @param reader Reader for error flagging.
 * @return int 1 on success, 0 if line too long.
 */
static int	append_char(char *line, int *pos, char c, t_line_reader *reader)
{
	if (*pos >= MAX_LINE_LENGTH)
	{
		reader->line_too_long = 1;
		return (0);
	}
	line[*pos] = c;
	(*pos)++;
	return (1);
}

/**
 * @brief Copy line content from buffer to output.
 *
 * @param reader Line reader.
 * @param line Output line buffer.
 * @param pos Current position pointer.
 * @return int 1 if newline found, 0 if need more data, -1 on error.
 */
static int	copy_line_content(t_line_reader *reader, char *line, int *pos)
{
	while (reader->buf_pos < reader->buf_len)
	{
		if (reader->buffer[reader->buf_pos] == '\n')
		{
			reader->buf_pos++;
			return (1);
		}
		if (!append_char(line, pos, reader->buffer[reader->buf_pos], reader))
		{
			while (reader->buf_pos < reader->buf_len
				&& reader->buffer[reader->buf_pos] != '\n')
				reader->buf_pos++;
			if (reader->buf_pos < reader->buf_len)
				reader->buf_pos++;
			return (-1);
		}
		reader->buf_pos++;
	}
	return (0);
}

/**
 * @brief Allocate and return the final line string.
 *
 * @param line Temporary line buffer.
 * @param len Length of line.
 * @return char* Allocated string or NULL on error.
 */
static char	*finalize_line(char *line, int len)
{
	char	*result;

	result = malloc(len + 1);
	if (!result)
		return (NULL);
	ft_strlcpy(result, line, len + 1);
	return (result);
}

/**
 * @brief Read next line from file using buffered I/O.
 *
 * @param reader Line reader.
 * @return char* Allocated line string, or NULL on EOF/error.
 */
char	*line_reader_next(t_line_reader *reader)
{
	char	line[MAX_LINE_LENGTH + 1];
	int		pos;
	int		status;

	if (!reader)
		return (NULL);
	reader->line_num++;
	reader->line_too_long = 0;
	pos = 0;
	while (1)
	{
		if (reader->buf_pos >= reader->buf_len)
		{
			if (refill_buffer(reader) <= 0)
				break ;
		}
		status = copy_line_content(reader, line, &pos);
		if (status == 1)
			return (finalize_line(line, pos));
		if (status == -1)
			return (NULL);
	}
	if (pos > 0)
		return (finalize_line(line, pos));
	return (NULL);
}
