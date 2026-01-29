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

/* Format helpers for libft-compliant string formatting */
int		format_id(char *buf, size_t size, const char *prefix, int n);
int		float_to_str(char *buf, size_t size, double value, int precision);

#endif
