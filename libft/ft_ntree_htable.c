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

static int	__htable_func(t_node *node, void *farg)
{
	ft_htbl_add(farg, node->key, node->content);
	return (0);
}

t_htbl	*ft_ntree_htable(t_node *node)
{
	t_htbl	*htbl;

	if (NULL == node)
	{
		return (NULL);
	}
	htbl = ft_htbl_init(ft_ntree_size(node));
	ft_ntree_bfs(node, htbl, __htable_func);
	return (htbl);
}
