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

#include <libft/node.h>

static int __dfs_recur(t_node *node, const void *farg, int (*f)(t_node *, const void *), t_node **res);

static int __depth;

int	ft_ntree_dfs(
	t_node **res, t_node *ntree, const void *farg, int (*f)(t_node *, const void *))
{
	t_node	*node;
	int		ret;

	__depth = 0;

	if ((NULL == ntree) || (NULL == f))
		return (-1);

	ret = __dfs_recur(ntree, farg, f, &node);

	if (res != NULL)
		*res = node;

	return (ret);
}

int ft_ntree_dfs_cur_depth(void)
{
	return (__depth);
}

static int __dfs_recur(
	t_node *node, const void *farg, int (*f)(t_node *, const void *), t_node **res)
{
	int ret;

	if (NULL == node)
	{
		*res = NULL;
		return (0);
	}

	if ((ret = f(node, farg)) != 0)
	{
		*res = (ret == 1) ? node : NULL;
		return (ret);
	}

	__depth++;

	ret = __dfs_recur(node->nodes, farg, f, res);

	__depth--;

	if (ret == 0)
		ret = __dfs_recur(node->next, farg, f, res);

	return (ret);
}