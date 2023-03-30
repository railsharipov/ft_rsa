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

static void 	__add_node(t_htbl *htbl, const char *key, void *content);
static t_node 	*__get_node(t_htbl *htbl, const char *key);
static void		__del_node_array(t_htbl *htbl);

void	(ft_htbl_add)(t_htbl *htbl, void *content, const char *key)
{
	t_node	*node;

	if (NULL == htbl)
		return ;
	
	__add_node(htbl, key, content);
}

void	*(ft_htbl_get)(t_htbl *htbl, const char *key)
{
	t_node	*node;

	if (NULL == htbl)
		return (NULL);

	node = __get_node(htbl, key);

	if (NULL == node)
		return (NULL);
	
	return (node->content);
}

void	(ft_htbl_assign)(t_htbl *htbl, void *content, const char *key)
{
	t_node	*node;

	if (NULL == htbl)
		return ;

	node = __get_node(htbl, key);

	if (NULL == node)
		return ;
	
	node->content = content;
}

void	(ft_htbl_erase)(t_htbl *htbl, const char *key)
{
	t_node		*node;
	t_node		*list;
	uint32_t	hash;
	int 		idx;

	if (NULL == htbl)
		return;

	hash = ft_hash((unsigned char *)key, ft_strlen(key));
	idx = (int)(hash % (uint32_t)htbl->size);

	list = htbl->arr[idx];
	node = list;

	while (NULL != node)
	{
		if (ft_strcmp(node->key, key) == 0)
			break ;

		node = node->next;
	}

	if (node != NULL)
		ft_lst_del_one(&list, node, NULL);

	htbl->arr[idx] = list;
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
	t_node *iter;
	t_node *node;
	int idx;

	new_htbl.size = MAX(LIBFT_HT_SIZE, size);
	new_htbl.size = CEIL(new_htbl.size, LIBFT_HT_SIZE);

	if (new_htbl.size <= htbl->size)
		return;

	LIBFT_ALLOC(new_htbl.arr, new_htbl.size * sizeof(void *));

	iter = ft_htbl_iter(htbl);
	node = iter;

	idx = 0;
	while (node != NULL)
	{
		item = node->content;
		__add_node(&new_htbl, item->key, item->content);
		node = node->next;
		idx++;
	}

	ft_lst_del(iter, NULL);
	__del_node_array(htbl);

	htbl->arr = new_htbl.arr;
	htbl->size = new_htbl.size;
}

static void	__add_node(t_htbl *htbl, const char *key, void *content)
{
	t_node		*node;
	t_node		*tmp;
	uint32_t	hash;
	int 		idx;

	hash = ft_hash((unsigned char *)key, ft_strlen(key));
	idx = (int)(hash % (uint32_t)htbl->size);

	node = ft_node_new(key, content, 0);
	node->hash = hash;

	if (NULL != htbl->arr[idx])
	{
		tmp = __get_node(htbl, key);

		if (NULL != tmp)
			tmp->content = content;
		else
			ft_lst_prepend((t_node **)(htbl->arr + idx), node);
	}
	else
	{
		htbl->arr[idx] = node;
	}
}

static t_node	*__get_node(t_htbl *htbl, const char *key)
{
	t_node		*node;
	uint32_t	hash;
	int 		idx;

	hash = ft_hash((unsigned char *)key, ft_strlen(key));
	idx = (int)(hash % (uint32_t)htbl->size);

	node = htbl->arr[idx];

	while (NULL != node)
	{
		if (ft_strcmp(node->key, key) == 0)
			return (node);

		node = node->next;
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

		while (NULL != node)
		{
			tmp = node;
			node = node->next;

			if (NULL != tmp->key)
				LIBFT_FREE(tmp->key);

			LIBFT_FREE(tmp);
		}

		idx++;
	}

	LIBFT_FREE(htbl->arr);
	htbl->arr = NULL;
	htbl->size = 0;
}