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

static t_node *__create_recur(char **key, int *idx);

// Construct ntree from a map.
//
// MAP is a null-terminated string.
// MAP represents an ITEMS sequence:
//		ITEMS[0], ITEMS[1], ..., ITEMS[n]
//
// Each item in ITEMS is a word separated by a single space.
// Item is one of the following types:
//
// 1.	NODE = any word that is not NODE_SEQUENCE_START or NODE_SEQUENCE_END.
// 		A node with NODE as a node key.
//
// 2.	NODE_SEQUENCE_START = "NODE{".
// 		A parent node with NODE as node key. Every ITEM following
//		NODE_SEQUENCE_START, until NODE_SEQUENCE_END, is a child node.
//
// 3.	NODE_SEQUENCE_END = "}".
// 		NODE_SEQUENCE_END represents end of child node sequence.
//
// Curly brackets are NOT included in node keys.
//
// Example: "colors{ RGB{ 25 27 30 } RGBA{ 32 35 40 0.5 } } sizes{ S M L }"
//
//					colors         ------>       sizes
//				/			\					/  \   \
//			RGB				RGBA				S   M   L
//		/	|	\		/	|	\	\
//		25	27	30		32	35	40	0.5

t_node *ft_ntree_construct(const char *map)
{
	t_node *ntree;
	char **keys;
	int idx;

	if (NULL == map)
		return (NULL);

	idx = 0;
	keys = ft_strsplit(map, ' ');

	if (NULL == keys)
		return (NULL);

	ntree = __create_recur(keys, &idx);
	ft_2darray_del_null_terminated((void **)keys);

	return (ntree);
}

static t_node *__create_recur(char **key, int *idx)
{
	t_node	*node;
	char	**childkey;

	if (NULL == key[*idx])
		return (NULL);
	
	if (!ft_strcmp(key[*idx], "}"))
	{
		(*idx)++;
		return (NULL);
	}

	node = ft_node_new(key[(*idx)++], NULL, 0);

	if (ft_strchr(node->key, '{'))
	{
		childkey = ft_strsplit(node->key, '{');
		LIBFT_FREE(node->key);

		if (NULL != childkey)
			node->key = ft_strdup(childkey[0]);
		else
			node->key = NULL;

		ft_2darray_del_null_terminated((void **)childkey);

		node->nodes = __create_recur(key, idx);
	}

	node->next = __create_recur(key, idx);

	return (node);
}
