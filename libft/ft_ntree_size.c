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

static int	__calc_size_func(t_node *node, const void *farg)
{
	int	*size;

	(void)node;
	size = (int *)farg;
	*size = *size + 1;
	
	return (0);
}

int	ft_ntree_size(t_node *node)
{
	int	size;

	size = 0;
	ft_ntree_bfs(NULL, node, &size, __calc_size_func);

	return (size);
}
