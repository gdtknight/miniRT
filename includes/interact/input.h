/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 18:30:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INPUT_H
# define INPUT_H

# include "render/render_types.h"

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
#  define KEY_UP 126
#  define KEY_DOWN 125
#  define KEY_BRACKET_LEFT 33
#  define KEY_BRACKET_RIGHT 30
#  define KEY_SEMICOLON 41
#  define KEY_QUOTE 39
#  define KEY_COMMA 43
#  define KEY_PERIOD 47
#  define KEY_J 38
#  define KEY_K 40
#  define KEY_L 37
#  define KEY_N 45
#  define KEY_M 46
#  define KEY_U 32
#  define KEY_O 31
#  define KEY_Y 16
#  define KEY_P 35
#  define KEY_1 18
#  define KEY_3 20
#  define KEY_EQUAL 24
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
#  define KEY_UP 65362
#  define KEY_DOWN 65364
#  define KEY_BRACKET_LEFT 91
#  define KEY_BRACKET_RIGHT 93
#  define KEY_SEMICOLON 59
#  define KEY_QUOTE 39
#  define KEY_COMMA 44
#  define KEY_PERIOD 46
#  define KEY_J 106
#  define KEY_K 107
#  define KEY_L 108
#  define KEY_N 110
#  define KEY_M 109
#  define KEY_U 117
#  define KEY_O 111
#  define KEY_Y 121
#  define KEY_P 112
#  define KEY_1 49
#  define KEY_3 51
#  define KEY_EQUAL 61
# else
#  error "Unsupported platform: define key codes for your OS"
# endif

/* Key bind initialization */
void	init_hud_binds(t_key_binds *binds);
void	init_camera_binds(t_key_binds *binds);
void	init_transform_move_binds(t_key_binds *binds);
void	init_transform_edit_binds(t_key_binds *binds);

/* Camera control functions */
void	handle_camera_move(t_render *render, int keycode);
void	handle_camera_pitch(t_render *render, int keycode);
void	handle_camera_reset(t_render *render);
void	handle_camera_yaw(t_render *render, int keycode);

/* Object control functions */
void	handle_object_move(t_render *render, int keycode);
void	handle_light_move(t_render *render, int keycode);

/* Key handler functions */
void	handle_hud_keys(t_render *render, int keycode);
void	handle_camera_keys(t_render *render, int keycode);
void	handle_transform_keys(t_render *render, int keycode);

/* Object resize and rotation */
void	handle_object_resize(t_render *render, int keycode);
void	handle_object_rotate(t_render *render, int keycode);

/* Rodrigues rotation */
t_vec3	rodrigues_rotate(t_vec3 v, t_vec3 k, double angle);

#endif
