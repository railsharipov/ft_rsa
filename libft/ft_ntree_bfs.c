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

static void __del_node_func(t_node *node);

int	ft_ntree_bfs(
	t_node **res, t_node *node, const void *farg, int (*f)(t_node *, const void *))
{
	t_queue	*queue;
	t_node	*child_node;
	int		ret;

	if ((NULL == node) || (NULL == f))
		return (-1);

	queue = ft_queue_init();

	while (node)
	{
		ft_queue_enqueue(queue, ft_node_new(NULL, node, 0));
		node = node->next;
	}

	while (!ft_queue_is_empty(queue))
	{
		node = ft_queue_dequeue(queue);

		if (ft_node_is_parent(node))
		{
			child_node = node->nodes;

			while (NULL != child_node)
			{
				ft_queue_enqueue(queue, ft_node_new(NULL, child_node, 0));
				child_node = child_node->next;
			}
		}

		ret = f(node, farg);

		if (ret != 0)
			break;
	}

	ft_queue_del(queue, __del_node_func);

	if (res != NULL)
		*res = (ret == 1) ? node : NULL;

	return (ret);
}

static void __del_node_func(t_node *node)
{
	if (NULL == node)
		return;

	LIBFT_FREE(node->key);
	LIBFT_FREE(node);
}