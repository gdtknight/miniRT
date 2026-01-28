/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_internal.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 18:30:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WINDOW_INTERNAL_H
# define WINDOW_INTERNAL_H

# include "window.h"

/* Platform-specific key code definitions */
# ifdef __APPLE__
/* macOS (Darwin) virtual key codes */
#  define KEY_ESC 53
#  define KEY_W 13
#  define KEY_A 0
#  define KEY_S 1
#  define KEY_D 2
#  define KEY_R 15
#  define KEY_F 3
#  define KEY_T 17
#  define KEY_G 5
#  define KEY_V 9
#  define KEY_E 14
#  define KEY_C 8
#  define KEY_X 7
#  define KEY_Z 6
#  define KEY_B 11
#  define KEY_Q 12
#  define KEY_I 34
#  define KEY_H 4
#  define KEY_TAB 48
#  define KEY_SHIFT_L 56
#  define KEY_SHIFT_R 60
#  define KEY_UP 126
#  define KEY_DOWN 125
#  define KEY_BRACKET_LEFT 33
#  define KEY_BRACKET_RIGHT 30
#  define KEY_INSERT 114
#  define KEY_HOME 115
#  define KEY_PGUP 116
#  define KEY_DELETE 117
#  define KEY_END 119
#  define KEY_PGDN 121
#  define KEY_J 38
#  define KEY_K 40
#  define KEY_N 45
#  define KEY_M 46
#  define KEY_U 32
#  define KEY_O 31
#  define KEY_Y 16
#  define KEY_P 35
#  define KEY_LEFT 123
#  define KEY_RIGHT 124
# elif defined(__linux__)
/* Linux/X11 KeySym values */
#  define KEY_ESC 65307
#  define KEY_W 119
#  define KEY_A 97
#  define KEY_S 115
#  define KEY_D 100
#  define KEY_R 114
#  define KEY_F 102
#  define KEY_T 116
#  define KEY_G 103
#  define KEY_V 118
#  define KEY_E 101
#  define KEY_C 99
#  define KEY_X 120
#  define KEY_Z 122
#  define KEY_B 98
#  define KEY_Q 113
#  define KEY_I 105
#  define KEY_H 104
#  define KEY_TAB 65289
#  define KEY_SHIFT_L 65505
#  define KEY_SHIFT_R 65506
#  define KEY_UP 65362
#  define KEY_DOWN 65364
#  define KEY_BRACKET_LEFT 91
#  define KEY_BRACKET_RIGHT 93
#  define KEY_INSERT 65379
#  define KEY_HOME 65360
#  define KEY_PGUP 65365
#  define KEY_DELETE 65535
#  define KEY_END 65367
#  define KEY_PGDN 65366
#  define KEY_J 106
#  define KEY_K 107
#  define KEY_N 110
#  define KEY_M 109
#  define KEY_U 117
#  define KEY_O 111
#  define KEY_Y 121
#  define KEY_P 112
#  define KEY_LEFT 65361
#  define KEY_RIGHT 65363
# endif

/* Camera control functions */
void	handle_camera_move(t_render *render, int keycode);
void	handle_camera_pitch(t_render *render, int keycode);
void	handle_camera_reset(t_render *render);

/* Object control functions */
void	handle_object_selection(t_render *render, int keycode);
void	handle_object_move(t_render *render, int keycode);
void	handle_light_move(t_render *render, int keycode);

/* Key handler functions */
void	handle_hud_keys(t_render *render, int keycode);
void	handle_camera_keys(t_render *render, int keycode);
void	handle_transform_keys(t_render *render, int keycode);

/* Object resize and rotation */
void	handle_object_resize(t_render *render, int keycode);
void	handle_object_rotate(t_render *render, int keycode);

/* Window expose handler */
int		handle_expose(t_render *render);

/* Selection helper functions */
void	cycle_type_forward(t_render *render);
void	cycle_backward_sphere(t_render *render);
void	cycle_backward_plane(t_render *render);
void	cycle_backward_cylinder(t_render *render);
int		next_type_from_sphere(t_scene *scene);
int		next_type_from_plane(t_scene *scene);
int		next_type_from_cylinder(t_scene *scene);

/* Forward declaration for re-rendering */
void	render_scene_to_buffer(t_scene *scene, t_render *render);

#endif
