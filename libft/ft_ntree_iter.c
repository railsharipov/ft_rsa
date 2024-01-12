/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ntree.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/13 09:35:41 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:18:24 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

static int	__iter_func(t_node *node, const void *ptr)
{
	t_queue	*queue;

	queue = (t_queue *)(ptr);
	ft_queue_enqueue(queue, NULL, node, 0);

	return (0);
}

t_node	*ft_ntree_iter(t_node *ntree)
{
	t_queue	*queue;
	t_node	*iter;

	if (NULL == ntree || NULL == (queue = ft_queue_init())) {
		return (NULL);
	}
	ft_ntree_bfs(NULL, ntree, queue, __iter_func);

	iter = ft_queue_peek(queue);
	LIBFT_FREE(queue);

	return (iter);
}
