/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_debounce.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 16:21:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 16:21:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RENDER_DEBOUNCE_H
# define RENDER_DEBOUNCE_H

# include <sys/time.h>

/* Default configuration values */
# define DEBOUNCE_DEFAULT_DELAY_MS			150
# define DEBOUNCE_COOLDOWN_MS				350
# define DEBOUNCE_PREVIEW_MIN_INTERVAL_MS	50

/* Forward declaration */
typedef struct s_render	t_render;

/* Debounce state machine states */
typedef enum e_debounce_state_enum
{
	DEBOUNCE_IDLE,
	DEBOUNCE_ACTIVE,
	DEBOUNCE_FINAL,
	DEBOUNCE_COOLDOWN
}	t_debounce_state_enum;

/* Debounce timer for tracking input delay */
typedef struct s_debounce_timer
{
	struct timeval	last_input_time;
	int				is_active;
	long			delay_ms;
}	t_debounce_timer;

/* Complete debounce state management */
typedef struct s_debounce_state
{
	t_debounce_state_enum	state;
	t_debounce_timer		timer;
	struct timeval			last_preview_time;
}	t_debounce_state;

/* Initialization and cleanup */

/**
 * @brief Initialize debounce state to idle.
 *
 * @param state Debounce state to initialize.
 */
void	debounce_init(t_debounce_state *state);

/* Input handling */

/**
 * @brief Handle user input by advancing or resetting the FSM.
 *
 * Transitions IDLE to ACTIVE, refreshes the timer on repeated input,
 * and triggers a low-quality preview render when throttled.
 *
 * @param state Current debounce state.
 * @param render Render context for setting quality flags.
 */
void	debounce_on_input(t_debounce_state *state, t_render *render);

/* State machine update */

/**
 * @brief Tick the debounce FSM each frame.
 *
 * Drives state transitions: ACTIVE to FINAL, FINAL to COOLDOWN,
 * and COOLDOWN to IDLE.
 *
 * @param state Current debounce state.
 * @param render Render context for checking and setting flags.
 */
void	debounce_update(t_debounce_state *state, t_render *render);

/* Timer utilities */

/**
 * @brief Check whether the debounce timer has expired.
 *
 * @param timer Timer to check.
 * @return 1 if elapsed time exceeds the configured delay.
 */
int		debounce_timer_expired(t_debounce_timer *timer);

/**
 * @brief Check if enough time has passed for a preview render.
 *
 * @param state Debounce state holding the last preview timestamp.
 * @return 1 if the minimum preview interval has elapsed.
 */
int		debounce_check_preview_throttle(t_debounce_state *state);

#endif
