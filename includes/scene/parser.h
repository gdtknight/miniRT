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

# include "scene/scene.h"

/* Constants */

# define LINE_READER_BUFFER_SIZE 4096
# define MAX_LINE_LENGTH 4096
# define COORD_MAX 10000.0

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
	PARSE_ERR_IO,
	PARSE_ERR_OVERFLOW,
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
	int		error;
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

/**
 * @brief Initialize a line reader from a file descriptor.
 *
 * @param reader Line reader to initialize.
 * @param fd File descriptor to read from.
 */
void			line_reader_init(t_line_reader *reader, int fd);

/**
 * @brief Read the next line from the file using buffered I/O.
 *
 * @param reader Line reader.
 * @return Allocated line string, or NULL on EOF or error.
 */
char			*line_reader_next(t_line_reader *reader);

/**
 * @brief Get the current line number.
 *
 * @param reader Line reader.
 * @return Current line number (1-based), or 0 if not started.
 */
int				line_reader_get_line_num(t_line_reader *reader);

/*
 * Number Parsing API
 */

/**
 * @brief Parse a double value with format validation.
 *
 * @param str Input string.
 * @param value Output value (set only on success).
 * @param end Pointer to the character after the parsed number.
 * @return PARSE_OK or PARSE_ERR_NUMBER_FORMAT.
 */
t_parse_result	parse_double(const char *str, double *value, const char **end);

/**
 * @brief Parse an integer value with format validation.
 *
 * @param str Input string.
 * @param value Output value (set only on success).
 * @param end Pointer to the character after the parsed number.
 * @return PARSE_OK or PARSE_ERR_NUMBER_FORMAT.
 */
t_parse_result	parse_int(const char *str, int *value, const char **end);

/**
 * @brief Parse integer digit sequence into a double.
 *
 * @param str Input string pointer (advanced past consumed digits).
 * @param has_digits Set to 1 if any digits are consumed.
 * @return Accumulated integer value.
 */
double			parse_int_part(const char **str, int *has_digits);

/**
 * @brief Parse the fractional part following a decimal point.
 *
 * Returns -1.0 if '.' is present but no digits follow.
 *
 * @param str Input string pointer (advanced past consumed digits).
 * @param has_digits Set to 1 if any digits are consumed.
 * @return Fractional value, or -1.0 on format error.
 */
double			parse_frac_part(const char **str, int *has_digits);

/*
 * Token Utilities API
 */

/**
 * @brief Advance past spaces and tabs.
 *
 * @param str Input string.
 * @return Pointer to the first non-whitespace character.
 */
const char		*skip_whitespace(const char *str);

/**
 * @brief Check if position is at the end of meaningful content.
 *
 * Skips whitespace, then returns true for null, newline, or '#'.
 *
 * @param str Input string.
 * @return 1 if at line end, 0 otherwise.
 */
int				at_line_end(const char *str);

/*
 * Error Context API
 */

/**
 * @brief Initialize an error context with default values.
 *
 * @param ctx Error context to initialize.
 */
void			error_context_init(t_error_context *ctx);

/**
 * @brief Print an error message with context to stderr.
 *
 * @param ctx Error context containing line, element, and error code.
 */
void			error_context_print(t_error_context *ctx);

/**
 * @brief Write a signed integer to stderr.
 *
 * @param n Integer value to write.
 */
void			error_write_int(int n);

/**
 * @brief Look up the human-readable message for a parse error code.
 *
 * @param code Parse result code.
 * @return Pointer to a static error message string.
 */
const char		*get_error_message(t_parse_result code);

/*
 * Validation API (Enhanced)
 */

/**
 * @brief Parse a vector with strict "x,y,z" formatting.
 *
 * @param str Input string.
 * @param vec Output vector.
 * @param end Pointer to the character after the parsed vector.
 * @return PARSE_OK on success, error code on failure.
 */
t_parse_result	parse_vector_strict(const char *str, t_vec3 *vec,
					const char **end);

/**
 * @brief Parse a color with strict "r,g,b" formatting.
 *
 * Each component must be an integer in [0, 255].
 *
 * @param str Input string.
 * @param color Output color.
 * @param end Pointer to the character after the parsed color.
 * @return PARSE_OK on success, error code on failure.
 */
t_parse_result	parse_color_strict(const char *str, t_color *color,
					const char **end);

/**
 * @brief Validate that vector components are in [-1, 1].
 *
 * @param vec Vector to validate.
 * @return PARSE_OK or PARSE_ERR_RANGE.
 */
t_parse_result	validate_vector_range(const t_vec3 *vec);

/**
 * @brief Validate that a direction/normal vector is not zero.
 *
 * @param vec Vector to validate.
 * @return PARSE_OK or PARSE_ERR_ZERO_VECTOR.
 */
t_parse_result	validate_direction_vector(t_vec3 *vec);

/**
 * @brief Validate that coordinate components are within COORD_MAX.
 *
 * @param vec Position vector to validate.
 * @return PARSE_OK or PARSE_ERR_RANGE.
 */
t_parse_result	validate_coordinate_range(const t_vec3 *vec);

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

/**
 * @brief Parse an element type prefix and dispatch to the matching parser.
 *
 * @param line Line content to dispatch.
 * @param scene Scene to update.
 * @param ctx Error context for reporting.
 * @return PARSE_OK or a parser error code.
 */
t_parse_result	dispatch_line(char *line, t_scene *scene,
					t_error_context *ctx);

/**
 * @brief Validate that a file name ends with ".rt".
 *
 * @param filename Path to the scene file.
 * @return 1 if extension matches, 0 otherwise.
 */
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
 * @brief Count objects of a specific type in the scene.
 * @param scene Scene containing the object list.
 * @param type Object type to count.
 * @return Number of objects of the given type.
 */
int				get_type_count(t_scene *scene, t_object_type type);

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
 * @brief Parse cone object
 * @param line Line to parse
 * @param scene Scene structure
 * @return t_parse_result PARSE_OK on success, error code on failure
 */
t_parse_result	parse_cone(char *line, t_scene *scene);

/**
 * @brief Parse optional bonus tokens (checker, bump) after color.
 *
 * @param token Current parse position (updated on return).
 * @param obj Object to receive bonus options.
 * @return PARSE_OK on success, error code on failure.
 */
t_parse_result	parse_bonus_options(const char **token, t_object *obj);

/**
 * @brief Validate scene has required elements
 * @param scene Scene structure to validate
 * @return 1 if valid, 0 if invalid
 */
int				validate_scene(t_scene *scene);

/**
 * @brief Check if value is in range [min, max]
 * @param value Value to check
 * @param min Minimum value
 * @param max Maximum value
 * @return 1 if in range, 0 otherwise
 */
int				in_range(double value, double min, double max);

#endif
