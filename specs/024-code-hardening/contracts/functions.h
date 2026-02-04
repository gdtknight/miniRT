/* ************************************************************************** */
/*                                                                            */
/*   Functions Contract: Code Hardening                                       */
/*   Feature: 024-code-hardening                                              */
/*   Date: 2026-01-31                                                         */
/*                                                                            */
/*   This file defines the function signatures affected by this feature.      */
/*   It serves as a contract between specification and implementation.        */
/*                                                                            */
/* ************************************************************************** */

#ifndef FUNCTIONS_CONTRACT_H
# define FUNCTIONS_CONTRACT_H

# include "minirt.h"
# include "vec3.h"

/* ========================================================================== */
/*                          FR-001: BVH Distance Init                         */
/* ========================================================================== */

/*
** bvh_leaf_intersect (MODIFIED BEHAVIOR)
**
** BEFORE: temp_hit uninitialized
** AFTER:  temp_hit.distance = hit->distance before intersect_ref() call
**
** Contract: temp_hit.distance MUST be initialized to hit->distance
**           to preserve caller's clip distance semantics.
*/

/* ========================================================================== */
/*                          FR-002: AABB Safe Slab Test                       */
/* ========================================================================== */

/*
** aabb_intersect (MODIFIED BEHAVIOR)
**
** BEFORE: Direct division by ray.direction.{x,y,z}
** AFTER:  Check |direction| < 1e-8 before division
**
** Contract: When direction component is near-zero, determine intersection
**           by checking if origin is within slab bounds instead of dividing.
*/

/* ========================================================================== */
/*                          FR-003: Camera Gimbal Lock                        */
/* ========================================================================== */

/*
** init_camera_calc (MODIFIED BEHAVIOR)
**
** BEFORE: Always uses (0,1,0) as world_up
** AFTER:  Uses (0,0,1) when |dot(direction, (0,1,0))| > 0.999
**
** Contract: Camera basis MUST be valid for all direction vectors,
**           including those parallel to world up.
*/

/* ========================================================================== */
/*                          FR-004: Progressive Tile Width                    */
/* ========================================================================== */

/*
** progressive_init (MODIFIED BEHAVIOR)
**
** BEFORE: Only stores tile_size, total_tiles
** AFTER:  Also stores width and height
**
** Contract: width and height MUST be set from parameters.
*/
void	progressive_init(t_progressive_state *prog, int width, int height,
			int tile_size);

/*
** progressive_next_tile (MODIFIED BEHAVIOR)
**
** BEFORE: Uses hardcoded 800 for tiles_x calculation
** AFTER:  Uses prog->width
**
** Contract: tiles_x MUST be calculated from prog->width, not hardcoded.
*/
int		progressive_next_tile(t_progressive_state *prog, t_tile_rect *rect);

/* ========================================================================== */
/*                          FR-005: Shadow Surface Normal                     */
/* ========================================================================== */

/*
** calc_shadow_samples (MODIFIED SIGNATURE)
**
** BEFORE: static double calc_shadow_samples(t_scene *scene, t_vec3 point,
**                                           t_vec3 light_pos, t_shadow_config *config)
** AFTER:  static double calc_shadow_samples(t_scene *scene, t_vec3 point,
**                                           t_vec3 light_pos, t_shadow_config *config,
**                                           t_vec3 surface_normal)
**
** Contract: surface_normal MUST be the actual hit normal, not (0,1,0).
*/

/*
** calculate_shadow_factor (MODIFIED SIGNATURE)
**
** BEFORE: double calculate_shadow_factor(t_scene *scene, t_vec3 point,
**                                        t_vec3 light_pos, t_shadow_config *config)
** AFTER:  double calculate_shadow_factor(t_scene *scene, t_vec3 point,
**                                        t_vec3 light_pos, t_shadow_config *config,
**                                        t_vec3 surface_normal)
**
** Contract: All callers MUST pass the actual surface normal.
*/
double	calculate_shadow_factor(t_scene *scene, t_vec3 point,
			t_vec3 light_pos, t_shadow_config *config, t_vec3 surface_normal);

/* ========================================================================== */
/*                          FR-006: I/O Error Flag                            */
/* ========================================================================== */

/*
** line_reader_init (MODIFIED BEHAVIOR)
**
** BEFORE: Initializes fd, buf_pos, buf_len, line_num, line_too_long
** AFTER:  Also initializes io_error = 0
**
** Contract: io_error MUST be initialized to 0.
*/
int		line_reader_init(t_line_reader *reader, int fd);

/*
** refill_buffer (MODIFIED BEHAVIOR)
**
** BEFORE: Returns read() result directly
** AFTER:  Sets reader->io_error = 1 if read() < 0
**
** Contract: io_error MUST be set when read() fails.
*/

/*
** process_lines (MODIFIED BEHAVIOR)
**
** BEFORE: Only checks line_too_long
** AFTER:  Also checks io_error
**
** Contract: Parser MUST fail if io_error is set.
*/

/* ========================================================================== */
/*                          FR-007: Integer Overflow Check                    */
/* ========================================================================== */

/*
** parse_int_digits (MODIFIED BEHAVIOR)
**
** BEFORE: void parse_int_digits(const char **str, int *result, int *has_digits)
** AFTER:  int  parse_int_digits(const char **str, int *result, int *has_digits)
**
** Contract: Returns 0 if overflow detected, 1 otherwise.
**           Overflow check: result > (INT_MAX - digit) / 10
*/

/*
** parse_int (MODIFIED BEHAVIOR)
**
** Contract: MUST propagate overflow error from parse_int_digits.
*/
t_parse_result	parse_int(const char *str, int *value, const char **end);

/* ========================================================================== */
/*                          FR-008: Capacity Overflow Check                   */
/* ========================================================================== */

/*
** object_list_grow (MODIFIED BEHAVIOR)
**
** BEFORE: new_capacity = list->capacity * 2; (no check)
** AFTER:  if (list->capacity > INT_MAX / 2) return (0);
**
** Contract: MUST return 0 if capacity * 2 would overflow.
*/
int		object_list_grow(t_object_list *list);

/* ========================================================================== */
/*                          FR-009: Pixel Bounds Check                        */
/* ========================================================================== */

/*
** mlx_img_put_pixel (MODIFIED BEHAVIOR)
**
** BEFORE: No bounds check
** AFTER:  if (x < 0 || x >= width || y < 0 || y >= height) return;
**
** Contract: Out-of-bounds writes MUST be silently ignored.
*/
void	mlx_img_put_pixel(t_mlx_img *img, int x, int y, int color);

/*
** mlx_img_get_pixel (MODIFIED BEHAVIOR)
**
** BEFORE: No bounds check
** AFTER:  if (x < 0 || x >= width || y < 0 || y >= height) return (0);
**
** Contract: Out-of-bounds reads MUST return 0.
*/
int		mlx_img_get_pixel(t_mlx_img *img, int x, int y);

#endif /* FUNCTIONS_CONTRACT_H */
