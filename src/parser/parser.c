/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:44 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * @brief Parse a single scene line and dispatch to element parsers.
 *
 * Skips whitespace and comments and routes to the correct parser based on
 * the element identifier.
 *
 * @param line Line content to parse.
 * @param scene Scene to update.
 * @param ctx Error context for reporting.
 * @return int 1 on success, 0 on failure.
 */
static int	parse_line(char *line, t_scene *scene, t_error_context *ctx)
{
	t_parse_result	result;

	ctx->element_type = NULL;
	result = dispatch_element(line, scene, ctx);
	if (result != PARSE_OK)
	{
		ctx->error_code = result;
		error_context_print(ctx);
		return (0);
	}
	return (1);
}

/**
 * @brief Validate required scene elements after parsing.
 *
 * Ensures ambient light, camera, light source, and at least one object exist.
 *
 * @param scene Scene to validate.
 * @return int 1 if valid, 0 otherwise.
 */
int	validate_scene(t_scene *scene)
{
	if (!scene_has_ambient(scene))
		return (error_print(ERR_PARSE_MISSING), 0);
	if (!scene_has_camera(scene))
		return (error_print(ERR_PARSE_MISSING), 0);
	if (scene->light_count == 0)
		return (error_print(ERR_PARSE_MISSING), 0);
	if (scene->objects.count == 0)
		return (error_print(ERR_PARSE_MISSING), 0);
	return (1);
}

/**
 * @brief Check if line was too long and report error.
 *
 * @param reader Line reader.
 * @param ctx Error context.
 * @return int 1 if error occurred, 0 otherwise.
 */
static int	check_line_too_long(t_line_reader *reader, t_error_context *ctx)
{
	if (reader->line_too_long)
	{
		error_context_set_line(ctx, reader->line_num);
		ctx->error_code = PARSE_ERR_LINE_TOO_LONG;
		error_context_print(ctx);
		return (1);
	}
	return (0);
}

/**
 * @brief Process lines from reader until EOF or error.
 *
 * @param reader Line reader.
 * @param scene Scene to populate.
 * @param ctx Error context.
 * @return int 1 on success, 0 on failure.
 */
static int	process_lines(t_line_reader *reader, t_scene *scene,
	t_error_context *ctx)
{
	char	*line;
	int		success;

	success = 1;
	line = line_reader_next(reader);
	while (success)
	{
		if (check_line_too_long(reader, ctx))
			return (0);
		if (line == NULL)
			break ;
		error_context_set_line(ctx, reader->line_num);
		success = parse_line(line, scene, ctx);
		free(line);
		line = line_reader_next(reader);
	}
	free(line);
	if (reader->io_error)
	{
		ctx->error_code = PARSE_ERR_IO;
		error_context_print(ctx);
		return (0);
	}
	return (success);
}

/**
 * @brief Parse a scene file into a scene structure.
 *
 * Validates the extension, reads each line, parses elements, and verifies
 * required components are present.
 *
 * @param filename Path to the scene file.
 * @param scene Scene to populate.
 * @return int 1 on success, 0 on failure.
 */
int	parse_scene(const char *filename, t_scene *scene)
{
	int				fd;
	t_line_reader	reader;
	t_error_context	ctx;
	int				success;

	if (!validate_extension(filename))
		return (error_print(ERR_FILE_EXT), 0);
	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return (error_print(ERR_FILE_OPEN), 0);
	error_context_init(&ctx);
	if (!line_reader_init(&reader, fd))
	{
		close(fd);
		return (error_print(ERR_MALLOC), 0);
	}
	success = process_lines(&reader, scene, &ctx);
	close(fd);
	if (success)
		success = validate_scene(scene);
	return (success);
}
