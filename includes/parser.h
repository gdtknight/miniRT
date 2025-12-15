/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: USER <EMAIL@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by USER              #+#    #+#             */
/*   Updated: 2025/12/15 00:00:00 by USER             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "minirt.h"

/**
 * @brief Parse scene file and populate scene structure
 * @param filename Path to .rt file
 * @param scene Scene structure to populate
 * @return 1 on success, 0 on error
 */
int		parse_scene(const char *filename, t_scene *scene);

/**
 * @brief Parse ambient lighting element
 * @param line Line to parse
 * @param scene Scene structure
 * @return 1 on success, 0 on error
 */
int		parse_ambient(char *line, t_scene *scene);

/**
 * @brief Parse camera element
 * @param line Line to parse
 * @param scene Scene structure
 * @return 1 on success, 0 on error
 */
int		parse_camera(char *line, t_scene *scene);

/**
 * @brief Parse light element
 * @param line Line to parse
 * @param scene Scene structure
 * @return 1 on success, 0 on error
 */
int		parse_light(char *line, t_scene *scene);

/**
 * @brief Parse sphere object
 * @param line Line to parse
 * @param scene Scene structure
 * @return 1 on success, 0 on error
 */
int		parse_sphere(char *line, t_scene *scene);

/**
 * @brief Parse plane object
 * @param line Line to parse
 * @param scene Scene structure
 * @return 1 on success, 0 on error
 */
int		parse_plane(char *line, t_scene *scene);

/**
 * @brief Parse cylinder object
 * @param line Line to parse
 * @param scene Scene structure
 * @return 1 on success, 0 on error
 */
int		parse_cylinder(char *line, t_scene *scene);

/**
 * @brief Validate scene has required elements
 * @param scene Scene structure to validate
 * @return 1 if valid, 0 if invalid
 */
int		validate_scene(t_scene *scene);

/**
 * @brief Parse vector from string (x,y,z format)
 * @param str String to parse
 * @param vec Vector to populate
 * @return 1 on success, 0 on error
 */
int		parse_vector(char *str, t_vec3 *vec);

/**
 * @brief Parse color from string (R,G,B format)
 * @param str String to parse
 * @param color Color to populate
 * @return 1 on success, 0 on error
 */
int		parse_color(char *str, t_color *color);

/**
 * @brief Check if value is in range [min, max]
 * @param value Value to check
 * @param min Minimum value
 * @param max Maximum value
 * @return 1 if in range, 0 otherwise
 */
int		in_range(double value, double min, double max);

#endif
