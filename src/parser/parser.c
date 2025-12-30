/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:44 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:45 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "spatial.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*
** Read one line from file descriptor.
** Reads until newline or EOF, returns dynamically allocated string.
** Returns NULL on EOF or read error.
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
	strcpy(line, buffer);
	return (line);
}

/*
** Parse single line from scene file.
** Identifies element type by prefix and calls appropriate parser.
** Returns 1 on success, 0 on error. Skips empty lines and comments.
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

/*
** Parse scene description file and populate scene structure.
** Opens file, reads line by line, and dispatches to element parsers.
** Returns 1 on success, 0 on error.
*/
int	parse_scene(const char *filename, t_scene *scene)
{
	int		fd;
	char	*line;
	int		success;

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
	close(fd);
	if (success)
	{
		success = validate_scene(scene);
		if (success)
			scene_build_bvh(scene);
	}
	return (success);
}

/*
** Validate that scene contains all required elements.
** Checks for ambient light, camera, light source, and at least one object.
*/
int	validate_scene(t_scene *scene)
{
	if (!scene->has_ambient)
		return (print_error("Missing ambient lighting (A)"));
	if (!scene->has_camera)
		return (print_error("Missing camera (C)"));
	if (!scene->has_light)
		return (print_error("Missing light (L)"));
	if (scene->sphere_count == 0 && scene->plane_count == 0
		&& scene->cylinder_count == 0)
		return (print_error("No objects in scene"));
	return (1);
}
