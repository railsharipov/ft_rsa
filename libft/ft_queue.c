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

#include <libft/node.h>
#include <libft/queue.h>
#include <libft/list.h>
#include <libft/alloc.h>

t_queue	*ft_queue_create(void)
{
	t_queue	*new;

	LIBFT_ALLOC(new, sizeof(t_queue));

	return (new);
}

void	ft_queue_enqueue(t_queue *queue, const char *key, void *content, size_t size)
{
	t_node	*node;

	if (NULL == queue) {
		return ;
	}
	node = ft_node_new(key, content, size);

	if (NULL == queue->last) {
		queue->last = node;
		queue->first = node;
	} else {
		queue->last->next = node;
		queue->last = node;
	}
}

void	ft_queue_enqueue_node(t_queue *queue, t_node *node)
{
	if (NULL == queue) {
		return ;
	}
	if (NULL == queue->last) {
		queue->last = node;
		queue->first = node;
	} else {
		queue->last->next = node;
		queue->last = node;
	}
}

void	*ft_queue_dequeue(t_queue *queue)
{
	t_node	*node;
	void	*content;

	if ((NULL == queue) || (NULL == queue->first)) {
		return (NULL);
	}
	node = queue->first;

	if (queue->first == queue->last) {
		queue->first = NULL;
		queue->last = NULL;
	} else {
		queue->first = queue->first->next;
	}
	content = node->content;
	LIBFT_FREE(node->key);
	LIBFT_FREE(node);

	return (content);
}

t_node	*ft_queue_dequeue_node(t_queue *queue)
{
	t_node	*node;

	if ((NULL == queue) || (NULL == queue->first)) {
		return (NULL);
	}
	node = queue->first;

	if (queue->first == queue->last) {
		queue->first = NULL;
		queue->last = NULL;
	} else {
		queue->first = queue->first->next;
	}

	return (node);
}

t_node	*ft_queue_first(t_queue *queue)
{
	if (NULL == queue) {
		return (NULL);
	}
	return (queue->first);
}

t_node	*ft_queue_last(t_queue *queue)
{
	if (NULL == queue) {
		return (NULL);
	}
	return (queue->last);
}

int		ft_queue_is_empty(t_queue *queue)
{
	if ((NULL == queue) || (NULL == queue->first)) {
		return (1);
	}
	return (0);
}

int		ft_queue_size(t_queue *queue)
{
	if ((NULL == queue) || (NULL == queue->first)) {
		return (0);
	}
	return (ft_lst_size(queue->first));
}

void	ft_queue_del(t_queue *queue)
{
	if (NULL == queue) {
		return ;
	}
	ft_lst_del(ft_queue_first(queue));
	LIBFT_FREE(queue);
}

void	ft_queue_del_with_f_del(t_queue *queue, FUNC_CONTENT_DEL f_del)
{
	if (NULL == queue) {
		return ;
	}
	ft_lst_del_with_f_del(ft_queue_first(queue), f_del);
	LIBFT_FREE(queue);
}

void	ft_queue_del_node(t_queue *queue, t_node *node)
{
	t_node *cur;
	t_node *prev;

	if (NULL == queue || NULL == queue->first || NULL == node) {
		return ;
	}
	prev = NULL;
	cur = queue->first;

	while ((NULL != cur) && (node != cur)) {
		prev = cur;
		cur = cur->next;
	}
	if (NULL == cur) {
		return ;
	}
	if (NULL == prev) {
		queue->first = cur->next;
	} else {
		prev->next = cur->next;
	}
	if (cur == queue->last) {
		queue->last = prev;
	}
	ft_node_del(cur);
}

void	ft_queue_del_node_with_f_del(t_queue *queue, t_node *node, FUNC_CONTENT_DEL f_del)
{
	t_node *cur;
	t_node *prev;

	if (NULL == queue || NULL == queue->first || NULL == node) {
		return ;
	}
	prev = NULL;
	cur = queue->first;

	while ((NULL != cur) && (node != cur)) {
		prev = cur;
		cur = cur->next;
	}
	if (NULL == cur) {
		return ;
	}
	if (NULL == prev) {
		queue->first = cur->next;
	} else {
		prev->next = cur->next;
	}
	if (cur == queue->last) {
		queue->last = prev;
	}
	ft_node_del_with_f_del(cur, f_del);
}

t_htbl	*ft_queue_to_htable(t_queue *queue)
{
	if (NULL == queue) {
		return (NULL);
	}
	return (ft_lst_to_htable(ft_queue_first(queue)));
}

t_node	*ft_queue_pop_list(t_queue *queue)
{
	t_node	*node;

	node = ft_queue_first(queue);
	queue->first = NULL;
	queue->last = NULL;

	return (node);
}
