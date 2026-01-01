/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard_layout.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KEYBOARD_LAYOUT_H
# define KEYBOARD_LAYOUT_H

/**
 * @file keyboard_layout.h
 * @brief Keyboard control layout contract for miniRT
 * 
 * This header defines the complete keyboard control scheme for the miniRT
 * ray tracer, including camera navigation, object manipulation, and UI controls.
 * 
 * Key Layout Design:
 * - Camera controls use W/X/A/D/Q/Z for translation, E/C for rotation, S for reset
 * - Object movement uses R/T/F/G/V/B in a compact ergonomic layout
 * - UI controls use H/TAB/Arrow keys for HUD and selection
 * 
 * Platform Support:
 * - macOS: Virtual key codes (Carbon framework)
 * - Linux: X11 KeySym values
 * 
 * Cross-platform compatibility achieved through conditional compilation.
 */

/* ========================================================================== */
/*                          PLATFORM KEY CODES                                */
/* ========================================================================== */

#ifdef __APPLE__
/* macOS (Darwin) Virtual Key Codes */

/* Camera Translation Keys */
# define KEY_W 13          /* Camera forward */
# define KEY_X 7           /* Camera backward */
# define KEY_A 0           /* Camera left */
# define KEY_D 2           /* Camera right */
# define KEY_Q 12          /* Camera up (world Y+) */
# define KEY_Z 6           /* Camera down (world Y-) */

/* Camera Rotation Keys */
# define KEY_E 14          /* Camera pitch up */
# define KEY_C 8           /* Camera pitch down */
# define KEY_S 1           /* Camera reset to initial */

/* Object Movement Keys */
# define KEY_R 15          /* Object move X- (left) */
# define KEY_T 17          /* Object move X+ (right) */
# define KEY_F 3           /* Object move Y- (down) */
# define KEY_G 5           /* Object move Y+ (up) */
# define KEY_V 9           /* Object move Z- (backward) */
# define KEY_B 11          /* Object move Z+ (forward) */

/* UI Control Keys */
# define KEY_H 4           /* Toggle HUD visibility */
# define KEY_TAB 48        /* Cycle HUD selection / Next object */
# define KEY_ESC 53        /* Exit application */
# define KEY_I 34          /* Info/Inspector (reserved) */

/* Object Selection Keys */
# define KEY_BRACKET_LEFT 33   /* Previous object */
# define KEY_BRACKET_RIGHT 30  /* Next object */

/* Navigation Keys */
# define KEY_UP 126        /* HUD page up */
# define KEY_DOWN 125      /* HUD page down */

/* Modifier Keys */
# define KEY_SHIFT_L 56    /* Left Shift */
# define KEY_SHIFT_R 60    /* Right Shift */

/* Light Movement Keys (6-key cluster) */
# define KEY_INSERT 114    /* Light move X+ */
# define KEY_DELETE 117    /* Light move X- */
# define KEY_HOME 115      /* Light move Y+ */
# define KEY_END 119       /* Light move Y- */
# define KEY_PGUP 116      /* Light move Z+ */
# define KEY_PGDN 121      /* Light move Z- */

#elif defined(__linux__)
/* Linux X11 KeySym Values */

/* Camera Translation Keys */
# define KEY_W 119         /* Camera forward */
# define KEY_X 120         /* Camera backward */
# define KEY_A 97          /* Camera left */
# define KEY_D 100         /* Camera right */
# define KEY_Q 113         /* Camera up (world Y+) */
# define KEY_Z 122         /* Camera down (world Y-) */

/* Camera Rotation Keys */
# define KEY_E 101         /* Camera pitch up */
# define KEY_C 99          /* Camera pitch down */
# define KEY_S 115         /* Camera reset to initial */

/* Object Movement Keys */
# define KEY_R 114         /* Object move X- (left) */
# define KEY_T 116         /* Object move X+ (right) */
# define KEY_F 102         /* Object move Y- (down) */
# define KEY_G 103         /* Object move Y+ (up) */
# define KEY_V 118         /* Object move Z- (backward) */
# define KEY_B 98          /* Object move Z+ (forward) */

/* UI Control Keys */
# define KEY_H 104         /* Toggle HUD visibility */
# define KEY_TAB 65289     /* Cycle HUD selection / Next object */
# define KEY_ESC 65307     /* Exit application */
# define KEY_I 105         /* Info/Inspector (reserved) */

/* Object Selection Keys */
# define KEY_BRACKET_LEFT 91   /* Previous object */
# define KEY_BRACKET_RIGHT 93  /* Next object */

/* Navigation Keys */
# define KEY_UP 65362      /* HUD page up */
# define KEY_DOWN 65364    /* HUD page down */

/* Modifier Keys */
# define KEY_SHIFT_L 65505 /* Left Shift */
# define KEY_SHIFT_R 65506 /* Right Shift */

/* Light Movement Keys (6-key cluster) */
# define KEY_INSERT 65379  /* Light move X+ */
# define KEY_DELETE 65535  /* Light move X- */
# define KEY_HOME 65360    /* Light move Y+ */
# define KEY_END 65367     /* Light move Y- */
# define KEY_PGUP 65365    /* Light move Z+ */
# define KEY_PGDN 65366    /* Light move Z- */

#endif

/* ========================================================================== */
/*                        FUNCTIONAL KEY GROUPS                               */
/* ========================================================================== */

/**
 * @brief Camera Control Keys
 * 
 * Translation (6 degrees of freedom):
 * - W/X: Forward/Backward along view direction
 * - A/D: Left/Right perpendicular to view direction
 * - Q/Z: Up/Down in world space (absolute Y axis)
 * 
 * Rotation (pitch only):
 * - E/C: Pitch up/down (rotate around right vector)
 * 
 * Reset:
 * - S: Return to initial camera position and orientation
 */

/**
 * @brief Object Movement Keys
 * 
 * Compact 2x3 layout for ergonomic left-hand operation:
 * 
 *   R   T      (X axis: left/right)
 *   F   G      (Y axis: down/up)
 *   V   B      (Z axis: backward/forward)
 * 
 * All movements are in world space coordinates.
 * Requires an object to be selected (via bracket keys).
 */

/**
 * @brief UI Control Keys
 * 
 * HUD Management:
 * - H: Toggle HUD visibility on/off
 * - TAB: Cycle to next selectable item
 * - Shift+TAB: Cycle to previous selectable item
 * - Up/Down: Navigate HUD pages
 * 
 * Object Selection:
 * - [ (Left Bracket): Select previous object
 * - ] (Right Bracket): Select next object
 * 
 * Application:
 * - ESC: Exit application
 */

/* ========================================================================== */
/*                         MOVEMENT CONSTANTS                                 */
/* ========================================================================== */

/**
 * @brief Default movement step size for camera and objects
 * 
 * This value determines how far the camera or object moves
 * with each key press. Measured in world space units.
 */
# define MOVEMENT_STEP_SIZE 1.0

/**
 * @brief Camera rotation angle per key press (in radians)
 * 
 * Defines the pitch angle increment for E/C keys.
 * 5 degrees = 0.0872665 radians
 */
# define ROTATION_ANGLE_RADIANS 0.0872665

/**
 * @brief Rotation angle in degrees (for display purposes)
 */
# define ROTATION_ANGLE_DEGREES 5.0

/* ========================================================================== */
/*                         FUNCTIONAL ENUMERATIONS                            */
/* ========================================================================== */

/**
 * @brief Key function categories for event routing
 */
typedef enum e_key_function
{
	KEY_FUNC_CAMERA_TRANSLATE,
	KEY_FUNC_CAMERA_ROTATE,
	KEY_FUNC_CAMERA_RESET,
	KEY_FUNC_OBJECT_MOVE,
	KEY_FUNC_LIGHT_MOVE,
	KEY_FUNC_UI_CONTROL,
	KEY_FUNC_SELECTION,
	KEY_FUNC_UNKNOWN
}	t_key_function;

/**
 * @brief Movement axes for translation operations
 */
typedef enum e_movement_axis
{
	AXIS_X,
	AXIS_Y,
	AXIS_Z,
	AXIS_NONE
}	t_movement_axis;

/**
 * @brief Movement directions (positive or negative along axis)
 */
typedef enum e_movement_direction
{
	DIR_NEGATIVE = -1,
	DIR_NONE = 0,
	DIR_POSITIVE = 1
}	t_movement_direction;

/* ========================================================================== */
/*                         USAGE DOCUMENTATION                                */
/* ========================================================================== */

/**
 * USAGE GUIDELINES:
 * 
 * 1. Camera Navigation:
 *    - Use W/X/A/D/Q/Z for first-person style movement
 *    - Use E/C to look up/down
 *    - Press S to reset to starting viewpoint
 * 
 * 2. Object Manipulation:
 *    - First select an object using [ or ] keys
 *    - Use R/T/F/G/V/B to move the selected object
 *    - Keep left hand on these keys for efficient workflow
 * 
 * 3. Light Positioning:
 *    - Use Insert/Delete/Home/End/PgUp/PgDn cluster
 *    - Independent from object movement controls
 * 
 * 4. Performance:
 *    - BVH acceleration is permanently enabled
 *    - Scene automatically rebuilds spatial structures after object movement
 *    - Low quality preview during continuous movement for responsiveness
 * 
 * IMPLEMENTATION NOTES:
 * 
 * - Platform detection via __APPLE__ and __linux__ preprocessor macros
 * - All key codes defined at compile time (no runtime lookup)
 * - Movement step size configurable via MOVEMENT_STEP_SIZE constant
 * - Rotation angle configurable via ROTATION_ANGLE_RADIANS constant
 * 
 * VALIDATION REQUIREMENTS:
 * 
 * - No key conflicts between functional groups
 * - All movement axes covered in both positive and negative directions
 * - Cross-platform key codes verified on both macOS and Linux
 * - norminette compliant (42 School coding standards)
 */

#endif
