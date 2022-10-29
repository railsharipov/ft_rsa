/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_hash.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/13 09:35:41 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:18:24 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <libft.h>

t_node	*ft_htbl_iter(t_htbl *htbl)
{
	t_queue	*queue;
	t_node	*iter;
	t_node	*item;
	int		idx;

	if ((NULL == htbl) || (NULL == htbl->arr))
		return (NULL);
	
	if (NULL == (queue = ft_queue_init()))
		return (NULL);
	
	idx = 0;
	while (idx < htbl->size)
	{
		item = htbl->arr[idx++];

		while (NULL != item)
		{
			ft_queue_enqueue(queue, ft_node_dup(item));
			item = item->next;
		}
	}
	iter = ft_queue_peek(queue);
	ft_queue_del(queue, NULL);

	return (iter);
}