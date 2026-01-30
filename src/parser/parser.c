/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:44 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * @brief Read a single line from a file descriptor.
 *
 * Reads up to a newline or buffer limit and returns a heap-allocated string.
 *
 * @param fd File descriptor to read from.
 * @return char* Newly allocated line string, or NULL on EOF/error.
 */
static char	*read_line(int fd)
{
	char	*line;
	char	buffer[1024];
	int		bytes_read;
	int		i;

	bytes_read = read(fd, buffer, 1);
	if (bytes_read <= 0)
		return (NULL);
	i = 0;
	while (bytes_read > 0 && buffer[i] != '\n' && i < 1023)
	{
		i++;
		bytes_read = read(fd, &buffer[i], 1);
	}
	buffer[i] = '\0';
	line = malloc(i + 1);
	if (!line)
		return (NULL);
	ft_strlcpy(line, buffer, i + 1);
	return (line);
}

/**
 * @brief Parse a single scene line and dispatch to element parsers.
 *
 * Skips whitespace and comments and routes to the correct parser based on
 * the element identifier.
 *
 * @param line Line content to parse.
 * @param scene Scene to update.
 * @return int 1 on success, 0 on failure.
 */
static int	parse_line(char *line, t_scene *scene)
{
	while (*line == ' ' || *line == '\t')
		line++;
	if (*line == '\0' || *line == '\n' || *line == '#')
		return (1);
	if (line[0] == 'A' && line[1] == ' ')
		return (parse_ambient(line, scene));
	else if (line[0] == 'C' && line[1] == ' ')
		return (parse_camera(line, scene));
	else if (line[0] == 'L' && line[1] == ' ')
		return (parse_light(line, scene));
	else if (line[0] == 's' && line[1] == 'p' && line[2] == ' ')
		return (parse_sphere(line, scene));
	else if (line[0] == 'p' && line[1] == 'l' && line[2] == ' ')
		return (parse_plane(line, scene));
	else if (line[0] == 'c' && line[1] == 'y' && line[2] == ' ')
		return (parse_cylinder(line, scene));
	return (print_error("Invalid element identifier"));
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
		return (print_error("Missing ambient lighting (A)"));
	if (!scene_has_camera(scene))
		return (print_error("Missing camera (C)"));
	if (!scene_has_light(scene))
		return (print_error("Missing light (L)"));
	if (scene->objects.count == 0)
		return (print_error("No objects in scene"));
	return (1);
}

/**
 * @brief Validate that a file name ends with ".rt".
 *
 * @param filename Path to the scene file.
 * @return int 1 if extension matches, 0 otherwise.
 */
static int	validate_extension(const char *filename)
{
	int	len;

	len = 0;
	while (filename[len])
		len++;
	if (len < 3)
		return (0);
	if (filename[len - 3] != '.' || filename[len - 2] != 'r'
		|| filename[len - 1] != 't')
		return (0);
	return (1);
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
	int		fd;
	char	*line;
	int		success;

	if (!validate_extension(filename))
		return (print_error("Invalid file extension (expected .rt)"));
	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return (print_error("Cannot open file"));
	success = 1;
	line = read_line(fd);
	while (success && line != NULL)
	{
		success = parse_line(line, scene);
		free(line);
		line = read_line(fd);
	}
	if (line != NULL)
		free(line);
	close(fd);
	if (success)
		success = validate_scene(scene);
	return (success);
}
