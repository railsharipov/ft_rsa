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

static void	__dfs_recur(
	t_node *node, void *farg, int (*f)(t_node *, void *), void **ptr)
{
	if (NULL == node)
	{
		return ;
	}
	if (NULL == *ptr)
	{
		if (f(node, farg) != 0)
		{
			*ptr = node;
			return ;
		}
		__dfs_recur(node->nodes, farg, f, ptr);
		__dfs_recur(node->next, farg, f, ptr);
	}
}

void	*ft_ntree_dfs(t_node *node, void *farg, int (*f)(t_node *, void *))
{
	void	*ptr;

	if ((NULL == node) || (NULL == f))
	{
		return (NULL);
	}
	ptr = NULL;
	__dfs_recur(node, farg, f, &ptr);
	return (ptr);
}
