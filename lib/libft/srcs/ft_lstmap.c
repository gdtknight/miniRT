/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 10:53:25 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/20 21:24:25 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*new_lst_head;
	t_list	*new_node;
	t_list	*new_content;

	if (!lst || !f || !del)
		return (NULL);
	new_lst_head = NULL;
	new_node = NULL;
	while (lst)
	{
		new_content = f(lst->content);
		new_node = ft_lstnew(new_content);
		if (!new_node)
		{
			if (new_content)
				del(new_content);
			ft_lstclear(&new_lst_head, del);
			return (NULL);
		}
		ft_lstadd_back(&new_lst_head, new_node);
		lst = lst->next;
	}
	return (new_lst_head);
}
