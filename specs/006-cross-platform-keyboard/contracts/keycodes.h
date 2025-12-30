/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keycodes.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/30 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KEYCODES_H
# define KEYCODES_H

/*
** Contract: Platform-Specific Keyboard Code Definitions
**
** This header defines the contract for cross-platform keyboard support in miniRT.
** It specifies the complete set of keyboard codes that MUST be defined for each
** supported platform (macOS and Linux/X11).
**
** Implementation Location: src/window/window.c (lines 37-71)
** Detection Method: Compile-time preprocessor macros (__APPLE__, __linux__)
**
** REQUIREMENTS:
** 1. All KEY_* constants MUST be defined for both platforms
** 2. Values MUST correspond to platform-specific keyboard event codes
** 3. Handler functions MUST work with these constants without modification
** 4. No runtime platform detection - compile-time only
**
** VALIDATION:
** - Compile successfully on both Ubuntu 20.04+ and macOS 10.14+
** - All 30+ keyboard controls respond identically on both platforms
** - Zero warnings during compilation
** - No memory overhead or runtime cost
*/

/* Platform detection */
#ifdef __APPLE__
# define PLATFORM_NAME "macOS (Darwin)"
# define KEYCODE_TYPE "Carbon Virtual Key Codes"
#elif defined(__linux__)
# define PLATFORM_NAME "Linux (X11)"
# define KEYCODE_TYPE "X11 KeySym Values"
#else
# error "Unsupported platform - requires macOS or Linux"
#endif

/*
** FUNCTIONAL GROUP: Application Control
** Purpose: Control application state and exit
** Priority: P1 (essential)
*/

/* KEY_ESC - Exit application */
#ifdef __APPLE__
# define KEY_ESC 53
#elif defined(__linux__)
# define KEY_ESC 65307
#endif

/* KEY_H - Toggle HUD visibility */
#ifdef __APPLE__
# define KEY_H 4
#elif defined(__linux__)
# define KEY_H 104
#endif

/* KEY_TAB - Navigate HUD sections */
#ifdef __APPLE__
# define KEY_TAB 48
#elif defined(__linux__)
# define KEY_TAB 65289
#endif

/* KEY_SHIFT_L, KEY_SHIFT_R - Modifier keys for reverse navigation */
#ifdef __APPLE__
# define KEY_SHIFT_L 56
# define KEY_SHIFT_R 60
#elif defined(__linux__)
# define KEY_SHIFT_L 65505
# define KEY_SHIFT_R 65506
#endif

/* KEY_UP, KEY_DOWN - HUD scroll navigation */
#ifdef __APPLE__
# define KEY_UP 126
# define KEY_DOWN 125
#elif defined(__linux__)
# define KEY_UP 65362
# define KEY_DOWN 65364
#endif

/*
** FUNCTIONAL GROUP: Camera Control
** Purpose: Navigate camera position and orientation
** Priority: P1 (essential)
*/

/* KEY_W, KEY_A, KEY_S, KEY_D - Camera movement (forward/left/backward/right) */
#ifdef __APPLE__
# define KEY_W 13
# define KEY_A 0
# define KEY_S 1
# define KEY_D 2
#elif defined(__linux__)
# define KEY_W 119
# define KEY_A 97
# define KEY_S 115
# define KEY_D 100
#endif

/* KEY_R, KEY_F - Camera pitch rotation (up/down) */
#ifdef __APPLE__
# define KEY_R 15
# define KEY_F 3
#elif defined(__linux__)
# define KEY_R 114
# define KEY_F 102
#endif

/*
** FUNCTIONAL GROUP: Object Manipulation
** Purpose: Select and move objects in the scene
** Priority: P2 (important)
*/

/* KEY_BRACKET_LEFT, KEY_BRACKET_RIGHT - Object selection (previous/next) */
#ifdef __APPLE__
# define KEY_BRACKET_LEFT 33
# define KEY_BRACKET_RIGHT 30
#elif defined(__linux__)
# define KEY_BRACKET_LEFT 91
# define KEY_BRACKET_RIGHT 93
#endif

/* KEY_KP_* - Object movement controls (numpad keys) */
#ifdef __APPLE__
# define KEY_KP_1 83
# define KEY_KP_2 84
# define KEY_KP_3 85
# define KEY_KP_4 86
# define KEY_KP_6 88
# define KEY_KP_8 91
#elif defined(__linux__)
# define KEY_KP_1 65436
# define KEY_KP_2 65433
# define KEY_KP_3 65435
# define KEY_KP_4 65430
# define KEY_KP_6 65432
# define KEY_KP_8 65431
#endif

/*
** FUNCTIONAL GROUP: Lighting Control
** Purpose: Adjust light source position
** Priority: P3 (enhancement)
*/

/* KEY_INSERT, KEY_DELETE - Light X-axis movement */
#ifdef __APPLE__
# define KEY_INSERT 114
# define KEY_DELETE 117
#elif defined(__linux__)
# define KEY_INSERT 65379
# define KEY_DELETE 65535
#endif

/* KEY_HOME, KEY_END - Light Y-axis movement */
#ifdef __APPLE__
# define KEY_HOME 115
# define KEY_END 119
#elif defined(__linux__)
# define KEY_HOME 65360
# define KEY_END 65367
#endif

/* KEY_PGUP, KEY_PGDN - Light Z-axis movement */
#ifdef __APPLE__
# define KEY_PGUP 116
# define KEY_PGDN 121
#elif defined(__linux__)
# define KEY_PGUP 65365
# define KEY_PGDN 65366
#endif

/*
** FUNCTIONAL GROUP: Rendering Options
** Purpose: Toggle rendering optimizations and debug info
** Priority: P3 (developer features)
*/

/* KEY_B - Toggle BVH acceleration */
#ifdef __APPLE__
# define KEY_B 11
#elif defined(__linux__)
# define KEY_B 98
#endif

/* KEY_Q - Toggle adaptive sampling */
#ifdef __APPLE__
# define KEY_Q 12
#elif defined(__linux__)
# define KEY_Q 113
#endif

/* KEY_I - Toggle info display */
#ifdef __APPLE__
# define KEY_I 34
#elif defined(__linux__)
# define KEY_I 105
#endif

/*
** IMPLEMENTATION NOTES:
**
** 1. These definitions replace the existing keycode constants in window.c
** 2. No changes to handler functions required - they work with KEY_* names
** 3. Conditional compilation ensures zero runtime overhead
** 4. Both platforms use same KEY_* names for consistency
**
** TESTING CHECKLIST:
** - [ ] Compile on macOS without warnings
** - [ ] Compile on Linux without warnings
** - [ ] All camera controls work on macOS (W/A/S/D/R/F)
** - [ ] All camera controls work on Linux (unchanged)
** - [ ] All object controls work on macOS (brackets, numpad)
** - [ ] All lighting controls work on macOS (Insert/Delete/Home/End/PgUp/PgDn)
** - [ ] All application controls work on macOS (ESC/H/TAB/Shift/Arrows)
** - [ ] All rendering controls work on macOS (B/Q/I)
** - [ ] Behavior identical between platforms
**
** MAINTENANCE:
** - When adding new keyboard controls, update both platform sections
** - Maintain alphabetical order within functional groups
** - Keep comments synchronized with implementation
** - Test on both platforms before merging
*/

#endif
