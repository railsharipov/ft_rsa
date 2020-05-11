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

static void	__f_default(t_node *node)
{
	LIBFT_FREE(node->key);
	LIBFT_FREE(node->content);
	LIBFT_FREE(node);
}

static void	__del_node_recur(t_node *node, void (*f_del)(t_node *))
{
	if (NULL == node)
	{
		return ;
	}
	__del_node_recur(node->nodes, f_del);
	__del_node_recur(node->next, f_del);
	f_del(node);
}

void	ft_ntree_del(t_node *ntree, void (*f_del)(t_node *))
{
	if (NULL == ntree)
	{
		return ;
	}
	if (NULL == f_del)
	{
		f_del = __f_default;
	}
	__del_node_recur(ntree, f_del);
}
