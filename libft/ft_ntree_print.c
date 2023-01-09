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

static void	__f_default(t_node *node, int space)
{
	(void)space;

	if (NULL != node)
	{
		if (ft_node_is_parent(node))
			ft_printf("%s\n", node->key, node->content);
		else
			ft_printf("%-25.25s %p\n", node->key, node->content);
		
	}
}

static void	__print_recur(
	t_node *node, int *space, void (*f_print)(t_node *, int))
{
	if (NULL == node)
		return ;
	
	f_print(node, *space);

	(*space)++;

	__print_recur(node->nodes, space, f_print);

	(*space)--;

	__print_recur(node->next, space, f_print);
}

void	ft_ntree_print(t_node *ntree, void (*f_print)(t_node *, int))
{
	int	space;

	space = 0;

	if (NULL == f_print)
		f_print = __f_default;
	
	__print_recur(ntree, &space, f_print);
}
