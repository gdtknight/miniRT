/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:26 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/30 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "minirt.h"

/* Constants */

# define LINE_READER_BUFFER_SIZE 4096
# define MAX_LINE_LENGTH 4096

/*
 * Parse Result Codes
 */

typedef enum e_parse_result
{
	PARSE_OK = 0,
	PARSE_ERR_FORMAT,
	PARSE_ERR_RANGE,
	PARSE_ERR_COMPONENT_COUNT,
	PARSE_ERR_DUPLICATE,
	PARSE_ERR_LINE_TOO_LONG,
	PARSE_ERR_ZERO_VECTOR,
	PARSE_ERR_TRAILING_TOKEN,
	PARSE_ERR_UNKNOWN_ELEMENT,
	PARSE_ERR_NUMBER_FORMAT,
	PARSE_ERR_MISSING_ELEMENT,
	PARSE_ERR_COUNT
}	t_parse_result;

/*
 * Line Reader Structure
 */

typedef struct s_line_reader
{
	int		fd;
	char	buffer[LINE_READER_BUFFER_SIZE];
	int		buf_pos;
	int		buf_len;
	int		line_num;
	int		line_too_long;
}	t_line_reader;

/*
 * Error Context Structure
 */

typedef struct s_error_context
{
	int				line_num;
	const char		*element_type;
	t_parse_result	error_code;
}	t_error_context;

/*
 * Line Reader API
 */

int				line_reader_init(t_line_reader *reader, int fd);
char			*line_reader_next(t_line_reader *reader);
int				line_reader_get_line_num(t_line_reader *reader);

/*
 * Number Parsing API
 */

t_parse_result	parse_double(const char *str, double *value, const char **end);
t_parse_result	parse_int(const char *str, int *value, const char **end);
int				parse_is_digit(char c);
double			parse_int_part(const char **str, int *has_digits);
double			parse_frac_part(const char **str, int *has_digits);

/*
 * Token Utilities API
 */

const char		*skip_whitespace(const char *str);
const char		*next_token(const char *str, const char **end);
int				at_line_end(const char *str);

/*
 * Error Context API
 */

void			error_context_init(t_error_context *ctx);
void			error_context_set_line(t_error_context *ctx, int line);
void			error_context_set_element(t_error_context *ctx,
					const char *type);
void			error_context_print(t_error_context *ctx);
void			error_write_str(const char *str);
void			error_write_int(int n);
const char		*get_error_message(t_parse_result code);

/*
 * Validation API (Enhanced)
 */

t_parse_result	parse_vector_strict(const char *str, t_vec3 *vec,
					const char **end);
t_parse_result	parse_color_strict(const char *str, t_color *color,
					const char **end);
t_parse_result	validate_direction_vector(t_vec3 *vec);

/*
 * Original Parser API
 */

/**
 * @brief Parse scene file and populate scene structure
 * @param filename Path to .rt file
 * @param scene Scene structure to populate
 * @return 1 on success, 0 on error
 */
int				parse_scene(const char *filename, t_scene *scene);
t_parse_result	dispatch_element(char *line, t_scene *scene,
					t_error_context *ctx);
int				validate_extension(const char *filename);

/**
 * @brief Parse ambient lighting element
 * @param line Line to parse
 * @param scene Scene structure
 * @return t_parse_result PARSE_OK on success, error code on failure
 */
t_parse_result	parse_ambient(char *line, t_scene *scene);

/**
 * @brief Parse camera element
 * @param line Line to parse
 * @param scene Scene structure
 * @return t_parse_result PARSE_OK on success, error code on failure
 */
t_parse_result	parse_camera(char *line, t_scene *scene);

/**
 * @brief Parse light element
 * @param line Line to parse
 * @param scene Scene structure
 * @return t_parse_result PARSE_OK on success, error code on failure
 */
t_parse_result	parse_light(char *line, t_scene *scene);

/**
 * @brief Parse sphere object
 * @param line Line to parse
 * @param scene Scene structure
 * @return t_parse_result PARSE_OK on success, error code on failure
 */
t_parse_result	parse_sphere(char *line, t_scene *scene);

/**
 * @brief Parse plane object
 * @param line Line to parse
 * @param scene Scene structure
 * @return t_parse_result PARSE_OK on success, error code on failure
 */
t_parse_result	parse_plane(char *line, t_scene *scene);

/**
 * @brief Parse cylinder object
 * @param line Line to parse
 * @param scene Scene structure
 * @return t_parse_result PARSE_OK on success, error code on failure
 */
t_parse_result	parse_cylinder(char *line, t_scene *scene);

/**
 * @brief Validate scene has required elements
 * @param scene Scene structure to validate
 * @return 1 if valid, 0 if invalid
 */
int				validate_scene(t_scene *scene);

/**
 * @brief Parse vector from string (x,y,z format) - legacy
 * @param str String to parse
 * @param vec Vector to populate
 * @return 1 on success, 0 on error
 */
int				parse_vector(char *str, t_vec3 *vec);

/**
 * @brief Parse color from string (R,G,B format) - legacy
 * @param str String to parse
 * @param color Color to populate
 * @return 1 on success, 0 on error
 */
int				parse_color(char *str, t_color *color);

/**
 * @brief Check if value is in range [min, max]
 * @param value Value to check
 * @param min Minimum value
 * @param max Maximum value
 * @return 1 if in range, 0 otherwise
 */
int				in_range(double value, double min, double max);

#endif
