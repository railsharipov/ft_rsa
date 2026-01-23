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
#include <libft/std.h>
#include <libft/htable.h>
#include <libft/list.h>
#include <libft/string.h>
#include <libft/alloc.h>
#include <libft/queue.h>

static uint32_t	__calculate_hash(const char *key);
static t_node	*__create_htable_node(const char *key, void *content, uint32_t hash, t_func_content_del f_del);
static void 	__add_node_to_htable(t_htbl *htbl, const char *key, void *content, t_func_content_del f_del);
static void 	__assign_content_to_node(t_node *node, void *content, t_func_content_del f_del);
static t_node	*__get_node_from_htable(t_htbl *htbl, const char *key);
static t_node	*__get_node_from_list(t_node *list, const char *key);
static int		__get_htable_array_idx(t_htbl *htbl, uint32_t hash);
static void		__del_htable_array(t_htbl *htbl, t_func_content_del f_del);
static void		__copy_node_to_htable(t_htbl *htbl, t_node *node, t_func_content_copy f_copy);

void *ft_htbl_create(int size)
{
	t_htbl	*htbl;

	LIBFT_ALLOC(htbl, sizeof(t_htbl));

	htbl->size = MAX(LIBFT_HT_SIZE, CEIL_TO_MULTIPLE(size, LIBFT_HT_SIZE));

	LIBFT_ALLOC(htbl->arr, htbl->size * sizeof(void *));

	return (htbl);
}

void	(ft_htbl_add)(t_htbl *htbl, void *content, const char *key)
{
	if (NULL == htbl) {
		return ;
	}
	__add_node_to_htable(htbl, key, content, NULL);
}

void	ft_htbl_add_with_f_del(t_htbl *htbl, void *content, const char *key, t_func_content_del f_del)
{
	if (NULL == htbl) {
		return ;
	}
	__add_node_to_htable(htbl, key, content, f_del);
}

int		ft_htbl_has(t_htbl *htbl, const char *key)
{
	if (NULL == htbl) {
		return (0);
	}
	return (__get_node_from_htable(htbl, key) != NULL);
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

void	ft_htbl_assign_with_f_del(t_htbl *htbl, void *content, const char *key, t_func_content_del f_del)
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
		ft_lst_del_one(&list, node);
	}
	htbl->arr[idx] = list;
}

void	ft_htbl_del_key_with_f_del(t_htbl *htbl, const char *key, t_func_content_del f_del)
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
		ft_lst_del_one_with_f_del(&list, node, f_del);
	}
	htbl->arr[idx] = list;
}

void ft_htbl_resize(t_htbl *htbl, int size)
{
	t_htbl new_htbl;
	t_node *node;

	if (NULL == htbl || size <= 0) {
		return;
	}

	new_htbl.size = MAX(LIBFT_HT_SIZE, CEIL_TO_MULTIPLE(size, LIBFT_HT_SIZE));

	if (new_htbl.size <= htbl->size) {
		return;
	}

	LIBFT_ALLOC(new_htbl.arr, new_htbl.size * sizeof(void *));

	node = ft_htbl_node_next(htbl, NULL);

	while (node != NULL) {
		__add_node_to_htable(&new_htbl, node->key, node->content, node->f_del_content);
		node = ft_htbl_node_next(htbl, node);
	}

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

static void	__add_node_to_htable(t_htbl *htbl, const char *key, void *content, t_func_content_del f_del)
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

static t_node	*__create_htable_node(const char *key, void *content, uint32_t hash, t_func_content_del f_del)
{
	t_node	*node;

	node = ft_node_new_with_f_del(key, content, sizeof(t_node), f_del);
	node->hash = hash;

	return (node);
}

static void 	__assign_content_to_node(t_node *node, void *content, t_func_content_del f_del)
{
	if (node->f_del_content) {
		node->f_del_content(node->content);
	}
	node->content = content;
	node->f_del_content = f_del;
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

	list = htbl->arr[hash % (uint32_t)htbl->size];

	if (NULL == list) {
		return (NULL);
	}
	return (__get_node_from_list(list, key));
}

void	(ft_htbl_del)(t_htbl *htbl)
{
	if (NULL == htbl) {
		return;
	}
	__del_htable_array(htbl, NULL);
	LIBFT_FREE(htbl);
}

void	ft_htbl_del_with_f_del(t_htbl *htbl, t_func_content_del f_del)
{
	if (NULL == htbl) {
		return;
	}
	__del_htable_array(htbl, f_del);
	LIBFT_FREE(htbl);
}

static void	__del_htable_array(t_htbl *htbl, t_func_content_del f_del)
{
	t_node *node;
	t_node *tmp;
	int idx;

	if (htbl->arr == NULL) {
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

t_node	*ft_htbl_node_next(t_htbl *htbl, t_node *node)
{
	t_node	*item;
	int	idx;

	if (NULL == htbl) {
		return (NULL);
	}
	if (node != NULL && node->next != NULL) {
		return (node->next);
	}

	if (node != NULL) {
		idx = (int)(node->hash % (uint32_t)htbl->size) + 1;
	} else {
		idx = 0;
	}

	while (idx < htbl->size) {
		item = htbl->arr[idx];

		if (NULL != item) {
			return (item);
		}
		idx++;
	}
	return (NULL);
}

void	ft_htbl_dump(t_htbl *htbl)
{
	t_node *node;
	int idx;

	if (NULL == htbl) {
		ft_printf("hash table: (null)\n");
		return ;
	}
	ft_printf("hash table: {size: %d, arr: %p}\n", htbl->size, htbl->arr);

	if (htbl->arr != NULL && htbl->size > 0) {
		idx = 0;
		while (idx < htbl->size) {
			node = htbl->arr[idx];
			while (node != NULL) {
				ft_printf("\tnode at idx: %d, addr: %p: {key: %s, hash: %llu}\n", idx, node, node->key, node->hash);
				node = node->next;
			}
			idx++;
		}
	}
}

t_htbl	*ft_htbl_copy(t_htbl *htbl)
{
	t_htbl	*copy;

	if (NULL == htbl) {
		return (NULL);
	}
	copy = ft_htbl_create(htbl->size);
	ft_htbl_merge(copy, htbl);
	return (copy);
}

t_htbl	*ft_htbl_copy_with_f_copy(t_htbl *htbl, t_func_content_copy f_copy)
{
	t_htbl	*copy;

	if (NULL == htbl) {
		return (NULL);
	}
	copy = ft_htbl_create(htbl->size);
	ft_htbl_merge_with_f_copy(copy, htbl, f_copy);
	return (copy);
}

void	ft_htbl_merge(t_htbl *htbl_to, t_htbl *htbl_from)
{
	t_node	*node;

	if (htbl_from == NULL || htbl_to == NULL) {
		return;
	}
	if (htbl_from->size > htbl_to->size) {
		ft_htbl_resize(htbl_to, htbl_from->size);
	}
	node = ft_htbl_node_next(htbl_from, NULL);

	while (node != NULL) {
		__copy_node_to_htable(htbl_to, node, NULL);
		node = ft_htbl_node_next(htbl_from, node);
	}
}

void	ft_htbl_merge_with_f_copy(t_htbl *htbl_to, t_htbl *htbl_from, t_func_content_copy f_copy)
{
	t_node	*node;

	if (htbl_from == NULL || htbl_to == NULL) {
		return;
	}
	if (htbl_from->size > htbl_to->size) {
		ft_htbl_resize(htbl_to, htbl_from->size);
	}
	node = ft_htbl_node_next(htbl_from, NULL);
	while (node != NULL) {
		__copy_node_to_htable(htbl_to, node, f_copy);
		node = ft_htbl_node_next(htbl_from, node);
	}
}


static void	__copy_node_to_htable(t_htbl *htbl, t_node *node, t_func_content_copy f_copy)
{
	if (f_copy != NULL) {
		__add_node_to_htable(htbl, node->key, f_copy(node->content), node->f_del_content);
	}
	else {
		__add_node_to_htable(htbl, node->key, node->content, node->f_del_content);
	}
}