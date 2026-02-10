/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_api.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT <miniRT@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 00:00:00 by miniRT            #+#    #+#             */
/*   Updated: 2026/01/30 00:00:00 by miniRT           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_API_H
# define PARSER_API_H

/*
** =============================================================================
** CONTRACT: Parser API for 023-parsing-robustness
** =============================================================================
** This file defines the public API contract for the improved parser.
** Implementation MUST conform to these signatures and behaviors.
*/

/*
** -----------------------------------------------------------------------------
** Constants
** -----------------------------------------------------------------------------
*/

# define LINE_READER_BUFFER_SIZE 4096
# define MAX_LINE_LENGTH 4096

/*
** -----------------------------------------------------------------------------
** Parse Result Codes
** -----------------------------------------------------------------------------
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
** -----------------------------------------------------------------------------
** Line Reader Structure
** -----------------------------------------------------------------------------
** Buffered file reader with line number tracking.
**
** fd:       File descriptor (must be valid open file)
** buffer:   Internal read buffer (4KB)
** buf_pos:  Current position in buffer
** buf_len:  Valid data length in buffer
** line_num: Current line number (1-based, 0 = not started)
*/

typedef struct s_line_reader
{
	int		fd;
	char	buffer[LINE_READER_BUFFER_SIZE];
	int		buf_pos;
	int		buf_len;
	int		line_num;
}	t_line_reader;

/*
** -----------------------------------------------------------------------------
** Error Context Structure
** -----------------------------------------------------------------------------
** Context information for error reporting.
**
** line_num:     Line number where error occurred (0 = unknown)
** element_type: Element type string ("Sphere", "Camera", etc.) or NULL
** error_code:   Parse result code indicating error type
*/

typedef struct s_error_context
{
	int				line_num;
	const char		*element_type;
	t_parse_result	error_code;
}	t_error_context;

/*
** -----------------------------------------------------------------------------
** Line Reader API
** -----------------------------------------------------------------------------
*/

/*
** Initialize line reader with file descriptor.
** Returns: 1 on success, 0 on failure
** Postcondition: reader->line_num = 0, buffers cleared
*/
int				line_reader_init(t_line_reader *reader, int fd);

/*
** Read next line from file.
** Returns: Allocated string (caller must free) or NULL on EOF/error
** Postcondition: reader->line_num incremented
** Error: Returns NULL and sets error if line > MAX_LINE_LENGTH
*/
char			*line_reader_next(t_line_reader *reader);

/*
** Get current line number.
** Returns: Line number (1-based) or 0 if not started
*/
int				line_reader_get_line_num(t_line_reader *reader);

/*
** -----------------------------------------------------------------------------
** Number Parsing API
** -----------------------------------------------------------------------------
*/

/*
** Parse double value with format validation.
** Allowed: 123, -45, +1, 0.5, -3.14, .5, -.5, +0., -0., 0.
** Rejected: --1, ++1, 12..3, 1e10, 123abc, +, -, .
**
** str:   Input string
** value: Output value (only set on success)
** end:   Pointer to character after parsed number (only set on success)
**
** Returns: PARSE_OK or PARSE_ERR_NUMBER_FORMAT
*/
t_parse_result	parse_double(const char *str, double *value, const char **end);

/*
** Parse integer value with format validation.
** Allowed: 123, -45, +1, 0
** Rejected: --1, ++1, 123abc, +, -
**
** Returns: PARSE_OK or PARSE_ERR_NUMBER_FORMAT
*/
t_parse_result	parse_int(const char *str, int *value, const char **end);

/*
** -----------------------------------------------------------------------------
** Token Utilities API
** -----------------------------------------------------------------------------
*/

/*
** Skip whitespace characters (space, tab).
** Returns: Pointer to first non-whitespace character
*/
const char		*skip_whitespace(const char *str);

/*
** Get next space-separated token.
** str: Current position
** end: Set to character after token
** Returns: Pointer to token start, or NULL if at end
*/
const char		*next_token(const char *str, const char **end);

/*
** Check if at end of meaningful content.
** Returns: 1 if *str is whitespace, '\0', or '#' (comment)
*/
int				at_line_end(const char *str);

/*
** -----------------------------------------------------------------------------
** Error Context API
** -----------------------------------------------------------------------------
*/

/*
** Initialize error context with default values.
** Postcondition: line_num=0, element_type=NULL, error_code=PARSE_OK
*/
void			error_context_init(t_error_context *ctx);

/*
** Set line number in error context.
*/
void			error_context_set_line(t_error_context *ctx, int line);

/*
** Set element type in error context.
** type: "Ambient", "Camera", "Light", "Sphere", "Plane", "Cylinder", or NULL
*/
void			error_context_set_element(t_error_context *ctx,
					const char *type);

/*
** Print error message with context to stderr.
** Format with element:  "Error\nLine N (Element): Description\n"
** Format without:       "Error\nLine N: Description\n"
*/
void			error_context_print(t_error_context *ctx);

/*
** -----------------------------------------------------------------------------
** Validation API (Enhanced)
** -----------------------------------------------------------------------------
*/

/*
** Parse vector with strict validation.
** Format: "x,y,z" (no spaces around commas, exactly 3 components)
**
** Returns: PARSE_OK, PARSE_ERR_COMPONENT_COUNT, or PARSE_ERR_NUMBER_FORMAT
*/
t_parse_result	parse_vector_strict(const char *str, t_vec3 *vec,
					const char **end);

/*
** Parse color with strict validation.
** Format: "r,g,b" (no spaces, exactly 3 components, each in [0,255])
**
** Returns: PARSE_OK, PARSE_ERR_COMPONENT_COUNT, PARSE_ERR_RANGE,
**          or PARSE_ERR_NUMBER_FORMAT
*/
t_parse_result	parse_color_strict(const char *str, t_color *color,
					const char **end);

/*
** Validate direction/normal vector is not zero.
** Returns: PARSE_OK or PARSE_ERR_ZERO_VECTOR
*/
t_parse_result	validate_direction_vector(t_vec3 *vec);

#endif
