/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: exam <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/04 09:12:22 by exam              #+#    #+#             */
/*   Updated: 2018/12/04 09:50:21 by exam             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

t_queue	*ft_queue_init(void)
{
	t_queue	*new;

	LIBFT_ALLOC(new, sizeof(t_queue));

	return (new);
}

void	ft_queue_enqueue(t_queue *queue, const char *key, void *content, size_t size)
{
	t_node	*node;
	t_node	*tmp;

	if (NULL == queue)
		return ;
	
	node = ft_node_new(key, content, size);

	if (NULL == queue->last)
	{
		queue->last = node;
		queue->first = node;
	}
	else
	{
		tmp = queue->last;
		queue->last = node;
		tmp->next = node;
	}
}

void	*ft_queue_dequeue(t_queue *queue)
{
	t_node	*node;
	void	*content;

	if ((NULL == queue) || (NULL == queue->first))
		return (NULL);
	
	node = queue->first;

	if (queue->first == queue->last)
	{
		queue->first = NULL;
		queue->last = NULL;
	}
	else
	{
		queue->first = queue->first->next;
	}

	content = node->content;
	LIBFT_FREE(node->key);
	LIBFT_FREE(node);

	return (content);
}

t_node	*ft_queue_peek(t_queue *queue)
{
	if (NULL == queue)
		return (NULL);
	
	return (queue->first);
}

int		ft_queue_is_empty(t_queue *queue)
{
	if ((NULL == queue) || (NULL == queue->first))
		return (1);
	
	return (0);
}

int		ft_queue_size(t_queue *queue)
{
	if ((NULL == queue) || (NULL == queue->first))
		return (0);
	
	return (ft_lst_size(queue->first));
}

int	ft_queue_del(t_queue *queue, int (*f_del)(t_node *))
{
	if (NULL == queue)
		return (LIBFT_ERR);
	
	if (LIBFT_OK != ft_lst_del(ft_queue_peek(queue), f_del))
		return (LIBFT_ERR);

	LIBFT_FREE(queue);

	return (LIBFT_OK);
}

int	ft_queue_del_node(t_queue *queue, t_node *node, int (*f_del)(t_node *))
{
	t_node *cur;
	t_node *prev;

	if ((NULL == queue) || (NULL == queue->first))
		return (LIBFT_ERR);

	if (NULL == node)
		return (LIBFT_ERR);

	prev = NULL;
	cur = queue->first;

	while ((NULL != cur) && (node != cur))
	{
		prev = cur;
		cur = cur->next;
	}

	if (NULL == cur)
		return (LIBFT_OK);

	if (NULL == prev)
		queue->first = cur->next;
	else
		prev->next = cur->next;
	
	if (cur == queue->last)
		queue->last = prev;

	return (ft_node_del(cur, f_del));
}

t_htbl	*ft_queue_htable(t_queue *queue)
{
	if (NULL == queue)
		return (NULL);
	
	return (ft_lst_htable(ft_queue_peek(queue)));
}
