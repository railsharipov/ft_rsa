/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/13 09:35:41 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:18:24 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft/node.h>
#include <libft/alloc.h>
#include <libft/std.h>
#include <libft/string.h>

void	(ft_node_init)(t_node *node)
{
	ft_bzero(node, sizeof(t_node));
}

void	ft_node_init_with_f_del(t_node *node, FUNC_CONTENT_DEL f_del)
{
	ft_bzero(node, sizeof(t_node));
	node->f_del = f_del;
}

t_node *(ft_node_create)(void)
{
	t_node	*node;

	LIBFT_ALLOC(node, sizeof(t_node));

	return (node);
}

t_node *ft_node_create_with_f_del(FUNC_CONTENT_DEL f_del)
{
	t_node	*node;

	LIBFT_ALLOC(node, sizeof(t_node));
	node->f_del = f_del;

	return (node);
}

t_node	*(ft_node_new)(const char *key, void *content, size_t size)
{
	t_node	*node;

	node = ft_node_create();

	if (NULL == node)
		return (NULL);

	node->key = ft_strdup(key);
	node->content = content;
	node->size = size;

	return (node);
}

t_node	*ft_node_new_with_f_del(const char *key, void *content, size_t size, FUNC_CONTENT_DEL f_del)
{
	t_node	*node;

	node = ft_node_create_with_f_del(f_del);

	if (NULL == node)
		return (NULL);

	node->key = ft_strdup(key);
	node->content = content;
	node->size = size;

	return (node);
}

void	(ft_node_del)(t_node *node)
{
	if (NULL != node) {
		if (NULL != node->f_del) {
			node->f_del(node->content);
		}
		LIBFT_FREE(node->key);
		LIBFT_FREE(node);
	}
}

void ft_node_del_with_f_del(t_node *node, FUNC_CONTENT_DEL f_del)
{
	if (NULL != node) {
		if (NULL != f_del) {
			f_del(node->content);
		}
		LIBFT_FREE(node->key);
		LIBFT_FREE(node);
	}
}
