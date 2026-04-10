/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 11:36:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/29 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include <stddef.h>
# include <sys/time.h>

/* Timer utilities */

/**
 * @brief Compute elapsed time in microseconds since a start time.
 *
 * @param start Start time captured with gettimeofday.
 * @return Elapsed microseconds.
 */
long	timer_elapsed_us(struct timeval *start);

/* Format helpers for libft-compliant string formatting */

/**
 * @brief Format an ID by concatenating a prefix and integer.
 *
 * @param buf Destination buffer.
 * @param size Destination buffer size.
 * @param prefix Prefix string.
 * @param n Numeric suffix.
 * @return 1 on success, 0 on allocation failure.
 */
int		format_id(char *buf, size_t size, const char *prefix, int n);

/**
 * @brief Format a floating-point number into a buffer.
 *
 * Supports 1 or 2 decimal digits with rounding.
 *
 * @param buf Destination buffer.
 * @param size Destination buffer size.
 * @param value Value to format.
 * @param precision Number of decimal digits (1 or 2).
 * @return 1 on success, 0 on failure.
 */
int		float_to_str(char *buf, size_t size, double value, int precision);

#endif
