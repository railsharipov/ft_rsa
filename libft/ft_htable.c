/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_hash.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/13 09:35:41 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:18:24 by rsharipo         ###   ########.fr       */
/*                                                                       */
/* ************************************************************************** */

#include <stddef.h>
#include <libft.h>

static uint32_t	__calculate_hash(const char *key);
static t_node	*__create_htable_node(const char *key, void *content, uint32_t hash, FUNC_CONTENT_DEL f_del);
static void 	__add_node_to_htable(t_htbl *htbl, const char *key, void *content, FUNC_CONTENT_DEL f_del);
static void 	__assign_content_to_node(t_node *node, void *content, FUNC_CONTENT_DEL f_del);
static t_node 	*__get_list_from_htable(t_htbl *htbl, uint32_t hash);
static t_node	*__get_node_from_htable(t_htbl *htbl, const char *key);
static t_node	*__get_node_from_list(t_node *list, const char *key);
static int		__get_htable_array_idx(t_htbl *htbl, uint32_t hash);
static void		__del_htable_array(t_htbl *htbl, FUNC_CONTENT_DEL f_del);

void	(ft_htbl_add)(t_htbl *htbl, void *content, const char *key)
{
	t_node	*node;

	if (NULL == htbl) {
		return ;
	}
	__add_node_to_htable(htbl, key, content, NULL);
}

void	ft_htbl_add_with_f_del(t_htbl *htbl, void *content, const char *key, FUNC_CONTENT_DEL f_del)
{
	t_node	*node;

	if (NULL == htbl) {
		return ;
	}
	__add_node_to_htable(htbl, key, content, f_del);
}

void	*ft_htbl_get(t_htbl *htbl, const char *key)
{
	t_node	*node;

	if (NULL == htbl) {
		return (NULL);
	}
	node = __get_node_from_htable(htbl, key);

	if (NULL == node) {
		return (NULL);
	}
	return (node->content);
}

void	(ft_htbl_assign)(t_htbl *htbl, void *content, const char *key)
{
	t_node	*node;

	if (NULL == htbl) {
		return ;
	}
	node = __get_node_from_htable(htbl, key);

	if (NULL == node) {
		return ;
	}
	__assign_content_to_node(node, content, NULL);
}

void	ft_htbl_assign_with_f_del(t_htbl *htbl, void *content, const char *key, FUNC_CONTENT_DEL f_del)
{
	t_node	*node;

	if (NULL == htbl) {
		return ;
	}
	node = __get_node_from_htable(htbl, key);

	if (NULL == node) {
		return ;
	}
	__assign_content_to_node(node, content, f_del);
}

void	(ft_htbl_del_key)(t_htbl *htbl, const char *key)
{
	t_node		*node;
	t_node		*list;
	uint32_t	hash;
	int 		idx;

	if (NULL == htbl) {
		return;
	}
	hash = __calculate_hash(key);
	idx = __get_htable_array_idx(htbl, hash);

	list = htbl->arr[idx];
	node = __get_node_from_list(list, key);

	if (node != NULL) {
		ft_lst_del_one(&list, node, NULL);
	}
	htbl->arr[idx] = list;
}

void	ft_htbl_del_key_with_f_del(t_htbl *htbl, const char *key, FUNC_CONTENT_DEL f_del)
{
	t_node		*node;
	t_node		*list;
	uint32_t	hash;
	int 		idx;

	if (NULL == htbl) {
		return;
	}
	hash = __calculate_hash(key);
	idx = __get_htable_array_idx(htbl, hash);

	list = htbl->arr[idx];
	node = __get_node_from_list(list, key);

	if (node != NULL) {
		ft_lst_del_one(&list, node, f_del);
	}
	htbl->arr[idx] = list;
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
		__add_node_to_htable(&new_htbl, item->key, item->content, item->f_del);
		node = node->next;
		idx++;
	}

	ft_lst_del(iter, NULL);
	__del_htable_array(htbl, NULL);

	htbl->arr = new_htbl.arr;
	htbl->size = new_htbl.size;
}

static uint32_t	__calculate_hash(const char *key)
{
	return (ft_hash((unsigned char *)key, ft_strlen(key)));
}

static int	__get_htable_array_idx(t_htbl *htbl, uint32_t hash)
{
	return ((int)(hash % (uint32_t)htbl->size));
}

static void	__add_node_to_htable(t_htbl *htbl, const char *key, void *content, FUNC_CONTENT_DEL f_del)
{
	t_node		*list;
	t_node		*node;
	uint32_t	hash;
	int 		idx;

	hash = __calculate_hash(key);
	idx = __get_htable_array_idx(htbl, hash);
	list = htbl->arr[idx];

	if (NULL != list) {
		node = __get_node_from_list(list, key);

		if (NULL != node) {
			__assign_content_to_node(node, content, f_del);
		} else {
			ft_lst_prepend(&list, __create_htable_node(key, content, hash, f_del));
			htbl->arr[idx] = list;
		}
	} else {
		htbl->arr[idx] = __create_htable_node(key, content, hash, f_del);
	}
}

static t_node	*__create_htable_node(const char *key, void *content, uint32_t hash, FUNC_CONTENT_DEL f_del)
{
	t_node	*node;

	node = ft_node_new_with_f_del(key, content, sizeof(t_node), f_del);
	node->hash = hash;

	return (node);
}

static void 	__assign_content_to_node(t_node *node, void *content, FUNC_CONTENT_DEL f_del)
{
	if (node->f_del) {
		node->f_del(node->content);
	}
	node->content = content;
	node->f_del = f_del;
}

static t_node	*__get_list_from_htable(t_htbl *htbl, uint32_t hash)
{
	int idx;

	idx = (int)(hash % (uint32_t)htbl->size);

	return (htbl->arr[idx]);
}

static t_node	*__get_node_from_list(t_node *list, const char *key)
{
	t_node		*node;

	node = list;

	while (NULL != node) {
		if (ft_strcmp(node->key, key) == 0) {
			return (node);
		}
		node = node->next;
	}
	return (NULL);
}

static t_node	*__get_node_from_htable(t_htbl *htbl, const char *key)
{
	t_node		*list;
	uint32_t	hash;

	hash = __calculate_hash(key);
	list = __get_list_from_htable(htbl, hash);

	if (NULL == list) {
		return (NULL);
	}
	return (__get_node_from_list(list, key));
}

void	(ft_htbl_del)(t_htbl *htbl)
{
	__del_htable_array(htbl, NULL);
	LIBFT_FREE(htbl);
}

void	ft_htbl_del_with_f_del(t_htbl *htbl, FUNC_CONTENT_DEL f_del)
{
	__del_htable_array(htbl, f_del);
	LIBFT_FREE(htbl);
}

static void	__del_htable_array(t_htbl *htbl, FUNC_CONTENT_DEL f_del)
{
	t_node *node;
	t_node *tmp;
	int idx;

	if (NULL == htbl) {
		return;
	}
	idx = 0;
	while (idx < htbl->size) {
		node = htbl->arr[idx];

		while (NULL != node) {
			tmp = node;
			node = node->next;

			if (f_del) {
				ft_node_del_with_f_del(tmp, f_del);
			} else {
				(ft_node_del)(tmp);
			}
		}
		idx++;
	}
	LIBFT_FREE(htbl->arr);
	htbl->arr = NULL;
	htbl->size = 0;
}