/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   object_list.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/27 12:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include <stdlib.h>
#include <limits.h>

/**
 * @brief Initialize an object list with a given capacity.
 *
 * Allocates storage for the specified number of objects and resets counters.
 *
 * @param list Object list to initialize.
 * @param capacity Initial capacity (must be > 0).
 * @return int 1 on success, 0 on failure.
 */
int	object_list_init(t_object_list *list, int capacity)
{
	if (!list || capacity <= 0)
		return (0);
	list->items = malloc(sizeof(t_object) * capacity);
	if (!list->items)
		return (0);
	list->count = 0;
	list->capacity = capacity;
	return (1);
}

/**
 * @brief Free object list storage and reset counters.
 *
 * @param list Object list to destroy.
 */
void	object_list_destroy(t_object_list *list)
{
	if (!list)
		return ;
	if (list->items)
	{
		free(list->items);
		list->items = NULL;
	}
	list->count = 0;
	list->capacity = 0;
}

/**
 * @brief Grow object list capacity by a factor of 2.
 *
 * Allocates a new buffer, copies existing objects, and swaps pointers.
 *
 * @param list Object list to grow.
 * @return int 1 on success, 0 on failure.
 */
int	object_list_grow(t_object_list *list)
{
	t_object	*new_items;
	int			new_capacity;

	if (!list)
		return (0);
	if (list->capacity > INT_MAX / 2)
		return (0);
	new_capacity = list->capacity * 2;
	new_items = malloc(sizeof(t_object) * new_capacity);
	if (!new_items)
		return (0);
	ft_memcpy(new_items, list->items, sizeof(t_object) * list->count);
	free(list->items);
	list->items = new_items;
	list->capacity = new_capacity;
	return (1);
}

/**
 * @brief Add an object to the list, growing if needed.
 *
 * @param list Object list to update.
 * @param obj Object to append.
 * @return int 1 on success, 0 on failure.
 */
int	object_list_add(t_object_list *list, t_object *obj)
{
	if (!list || !obj)
		return (0);
	if (list->count >= list->capacity)
	{
		if (!object_list_grow(list))
			return (0);
	}
	list->items[list->count] = *obj;
	list->count++;
	return (1);
}
