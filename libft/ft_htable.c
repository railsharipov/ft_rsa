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

static void		__add_node(t_htbl *htbl, t_node *node, uint32_t hash);
static t_node	*__get_node(t_htbl *htbl, uint32_t hash);
static void		__del_node_array(t_htbl *htbl);

void	(ft_htbl_add)(t_htbl *htbl, void *content, const char *key)
{
	t_node		*node;

	if (NULL == htbl)
		return ;
	
	node = ft_node_new(key, content, 0);

	if (NULL == node)
		return ;

	node->hash = ft_hash((unsigned char *)key, ft_strlen(key));

	__add_node(htbl, node, node->hash);
}

void	*(ft_htbl_get)(t_htbl *htbl, const char *key)
{
	t_node		*node;
	uint32_t	hash;

	if (NULL == htbl)
		return (NULL);

	hash = ft_hash((unsigned char *)key, ft_strlen(key));
	node = __get_node(htbl, hash);

	if (NULL == node)
		return (NULL);
	
	return (node->content);
}

void	(ft_htbl_assign)(t_htbl *htbl, void *content, const char *key)
{
	t_node		*node;
	uint32_t	hash;

	if (NULL == htbl)
		return ;

	hash = ft_hash((unsigned char *)key, ft_strlen(key));

	node = __get_node(htbl, hash);

	if (NULL == node)
		return ;
	
	node->content = content;
}

void	ft_htbl_del(t_htbl *htbl)
{
	__del_node_array(htbl);
	LIBFT_FREE(htbl);
}

void ft_htbl_resize(t_htbl *htbl, int size)
{
	t_htbl new_htbl;
	t_node *item;
	int idx;

	new_htbl.size = MAX(LIBFT_HT_SIZE, LIBFT_HT_FACTOR * size);

	if (new_htbl.size <= htbl->size)
		return;

	LIBFT_ALLOC(new_htbl.arr, new_htbl.size * sizeof(void *));

	for (item = ft_htbl_iter(htbl); item != NULL; item = item->next)
		__add_node(&new_htbl, item, item->hash);

	__del_node_array(htbl);
	htbl->arr = new_htbl.arr;
	htbl->size = new_htbl.size;
}

static void	__add_node(t_htbl *htbl, t_node *node, uint32_t hash)
{
	int idx;

	idx = (int)(hash % (uint32_t)htbl->size);

	if (NULL != htbl->arr[idx])
		ft_lst_prepend((t_node **)(htbl->arr + idx), node);
	else
		htbl->arr[idx] = node;
}

static t_node	*__get_node(t_htbl *htbl, uint32_t hash)
{
	t_node *node;
	int idx;

	idx = (int)(hash % (uint32_t)htbl->size);
	node = htbl->arr[idx];

	if (NULL != node)
	{
		if (NULL == node->next)
			return (node);

		while (NULL != node)
		{
			if (node->hash == hash)
				return (node);

			node = node->next;
		}
	}
	return (NULL);
}

static void	__del_node_array(t_htbl *htbl)
{
	t_node *node;
	t_node *tmp;
	int idx;

	if (NULL == htbl)
		return;

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
					LIBFT_FREE(tmp->key);

				LIBFT_FREE(tmp);
			}
		}
		idx++;
	}
	LIBFT_FREE(htbl->arr);
	htbl->arr = NULL;
	htbl->size = 0;
}