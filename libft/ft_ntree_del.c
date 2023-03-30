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

static int	__del_node_recur(t_node *node, int (*f_del)(t_node *))
{
	if (NULL == node)
		return (LIBFT_OK);
	
	if (LIBFT_OK != __del_node_recur(node->nodes, f_del))
		return (LIBFT_ERR);
	
	if (LIBFT_OK != __del_node_recur(node->next, f_del))
		return (LIBFT_ERR);
	
	return (ft_node_del(node, f_del));
}

int	ft_ntree_del(t_node *ntree, int (*f_del)(t_node *))
{
	if (NULL == ntree)
		return (LIBFT_ERR);
	
	return (__del_node_recur(ntree, f_del));
}
