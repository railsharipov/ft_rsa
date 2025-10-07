/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: exam <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/04 09:12:22 by exam              #+#    #+#             */
/*   Updated: 2018/12/04 09:50:21 by exam             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft/std.h>
#include <libft/list.h>
#include <unistd.h>

void	ft_lst_append(t_node **lst, t_node *node)
{
	t_node	*tmp;

	if (NULL == lst || NULL == node) {
		return ;
	}
	if (NULL == *lst) {
		*lst = node;
		return ;
	}
	tmp = *lst;

	while (tmp->next) {
		tmp = tmp->next;
	}
	tmp->next = node;
}

void	ft_lst_prepend(t_node **lst, t_node *node)
{
	if (NULL == node || NULL == lst) {
		return ;
	}
	node->next = *lst;
	*lst = node;
}

void	ft_lst_del_first(t_node **lst)
{
	t_node	*tmp;

	if ((NULL == lst) || (NULL == *lst)) {
		return ;
	}
	tmp = *lst;
	*lst = (*lst)->next;
	ft_node_del(tmp);
}

void	ft_lst_del_first_with_f_del(t_node **lst, t_func_content_del f_del)
{
	t_node	*tmp;

	if ((NULL == lst) || (NULL == *lst)) {
		return ;
	}
	tmp = *lst;
	*lst = (*lst)->next;
	ft_node_del_with_f_del(tmp, f_del);
}

void	ft_lst_del_last(t_node **lst)
{
	t_node	*cur;
	t_node	*prev;

	if ((NULL == lst) || (NULL == *lst)) {
		return ;
	}
	prev = NULL;
	cur = *lst;

	while (cur->next) {
		prev = cur;
		cur = cur->next;
	}
	if (NULL == prev) {
		*lst = NULL;
	} else {
		prev->next = NULL;
	}
	ft_node_del(cur);
}

void	ft_lst_del_last_with_f_del(t_node **lst, t_func_content_del f_del)
{
	t_node	*cur;
	t_node	*prev;

	if ((NULL == lst) || (NULL == *lst)) {
		return ;
	}
	prev = NULL;
	cur = *lst;

	while (cur->next) {
		prev = cur;
		cur = cur->next;
	}
	if (NULL == prev) {
		*lst = NULL;
	} else {
		prev->next = NULL;
	}
	ft_node_del_with_f_del(cur, f_del);
}

void	ft_lst_del_one(t_node **lst, t_node *node)
{
	t_node	*cur;
	t_node	*prev;

	if (NULL == lst || NULL == *lst || NULL == node) {
		return ;
	}
	prev = NULL;
	cur = *lst;

	while ((NULL != cur) && (node != cur)) {
		prev = cur;
		cur = cur->next;
	}
	if (NULL == cur) {
		return ;
	}
	if (NULL == prev) {
		*lst = cur->next;
	} else {
		prev->next = cur->next;
	}
	ft_node_del(cur);
}

void	ft_lst_del_one_with_f_del(t_node **lst, t_node *node, t_func_content_del f_del)
{
	t_node	*cur;
	t_node	*prev;

	if (NULL == lst || NULL == *lst || NULL == node) {
		return ;
	}
	prev = NULL;
	cur = *lst;

	while ((NULL != cur) && (node != cur)) {
		prev = cur;
		cur = cur->next;
	}
	if (NULL == cur) {
		return ;
	}
	if (NULL == prev) {
		*lst = cur->next;
	} else {
		prev->next = cur->next;
	}
	ft_node_del_with_f_del(cur, f_del);
}

size_t	ft_lst_size(t_node *lst)
{
	size_t	size;

	size = 0;
	while (lst) {
		size++;
		lst = lst->next;
	}

	return (size);
}

void	ft_lst_rev(t_node **lst)
{
	t_node	*prev;
	t_node	*cur;
	t_node	*next;

	if (NULL == lst || NULL == *lst) {
		return ;
	}

	prev = NULL;
	cur = *lst;

	while (cur) {
		next = cur->next;
		cur->next = prev;
		prev = cur;
		cur = next;
	}
	*lst = prev;
}

void	ft_lst_del(t_node *lst)
{
	t_node	*tmp;

	while (lst) {
		tmp = lst;
		lst = lst->next;
		ft_node_del(tmp);
	}
}

void	ft_lst_del_with_f_del(t_node *lst, t_func_content_del f_del)
{
	t_node	*tmp;

	while (lst) {
		tmp = lst;
		lst = lst->next;
		ft_node_del_with_f_del(tmp, f_del);
	}
}

int	ft_lst_map(t_node *lst, void *farg, int (*f)(t_node *, void *))
{
	if (NULL == f) {
		return (-1);
	}
	while (lst) {
		if (f(lst, farg) < 0) {
			return (-1);
		}
		lst = lst->next;
	}
	return (0);
}

t_htbl	*ft_lst_to_htable(t_node *lst)
{
	t_htbl	*htbl;

	if (NULL == lst) {
		return (NULL);
	}
	htbl = ft_htbl_create(ft_lst_size(lst));

	while (lst) {
		ft_htbl_add(htbl, lst->content, lst->key);
		lst = lst->next;
	}

	return (htbl);
}
