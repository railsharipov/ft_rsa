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

#include <libft.h>

void	*ft_htbl_init(int size)
{
	t_htbl	*htbl;

	LIBFT_ALLOC(htbl, sizeof(t_htbl));
	htbl->size = MAX(LIBFT_HT_SIZE, LIBFT_HT_FACTOR*size);
	LIBFT_ALLOC(htbl->arr, htbl->size * sizeof(void *));
	return (htbl);
}

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
		if (NULL != item)
		{
			while (item)
			{
				ft_queue_enqueue(
					queue, ft_node_new(item->key, item->content, 0));
				item = item->next;
			}
		}
	}
	iter = ft_queue_peek(queue);
	ft_queue_del(queue, NULL);
	return (iter);
}

void	ft_htbl_add(t_htbl *htbl, const char *key, void *content)
{
	t_node		*node;
	uint32_t	hash;

	if (NULL == htbl)
		return ;
	hash = ft_hash(key, ft_strlen(key)) % htbl->size;
	node = ft_node_new(key, content, 0);
	if (NULL == node)
		return ;
	if (NULL != htbl->arr[hash])
	{
		ft_lst_prepend((t_node **)(htbl->arr+hash), node);
	}
	else
	{
		htbl->arr[hash] = node;
	}
}

static t_node	*__htbl_node(t_htbl *htbl, const char *key)
{
	t_node		*node;
	uint32_t	hash;

	hash = ft_hash(key, ft_strlen(key)) % htbl->size;
	node = htbl->arr[hash];
	if (NULL != node)
	{
		if (NULL == node->next)
		{
			return (node);
		}
		while (NULL != node)
		{
			if (!ft_strcmp(node->key, key))
				return (node);
			node = node->next;
		}
	}
	return (NULL);
}

void	*ft_htbl_get(t_htbl *htbl, const char *key)
{
	t_node	*node;

	if (NULL == htbl)
		return (NULL);
	node = __htbl_node(htbl, key);
	if (NULL == node)
		return (NULL);
	return (node->content);
}

void	ft_htbl_assign(t_htbl *htbl, const char *key, void *content)
{
	t_node	*node;

	if (NULL == htbl)
		return ;
	node = __htbl_node(htbl, key);
	if (NULL == node)
		return ;
	node->content = content;
}

void	ft_htbl_del(t_htbl *htbl)
{
	t_node	*node;
	t_node	*tmp;
	int		idx;

	if (NULL == htbl)
		return ;
	idx = 0;
	while (idx < htbl->size)
	{
		node = htbl->arr[idx];
		if (NULL != node)
		{
			while (NULL != node)
			{
				tmp = node;
				node = node->next;
				if (NULL != tmp->key)
				{
					LIBFT_FREE(tmp->key);
				}
				LIBFT_FREE(tmp);
			}
		}
		idx++;
	}
	LIBFT_FREE(htbl->arr);
	LIBFT_FREE(htbl);
}
