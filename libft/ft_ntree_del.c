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

static void	__del_node_recur(t_node *node, FUNC_CONTENT_DEL f_del)
{
	if (NULL == node) {
		return ;
	}
	__del_node_recur(node->nodes, f_del);
	__del_node_recur(node->next, f_del);
	ft_node_del(node, f_del);
}

void	ft_ntree_del(t_node *ntree, FUNC_CONTENT_DEL f_del)
{
	if (NULL == ntree) {
		return ;
	}
	__del_node_recur(ntree, f_del);
}
