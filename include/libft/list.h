/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/09 13:36:51 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 09:00:28 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_LIST_H
# define LIBFT_LIST_H

# include <libft/common.h>
# include <libft/node.h>
# include <libft/htable.h>

t_node		*ft_lst_append(t_node **lst, t_node *node);
t_node		*ft_lst_prepend(t_node **lst, t_node *node);
t_node		*ft_lst_concat(t_node *lst_a, t_node *lst_b);
void		ft_lst_del(t_node *lst);
void		ft_lst_del_with_f_del(t_node *lst, t_func_content_del f_del);
void		ft_lst_del_first(t_node **lst);
void		ft_lst_del_first_with_f_del(t_node **lst, t_func_content_del f_del);
void		ft_lst_del_last(t_node **lst);
void		ft_lst_del_last_with_f_del(t_node **lst, t_func_content_del f_del);
void		ft_lst_del_one(t_node **lst, t_node *node);
void		ft_lst_del_one_with_f_del(t_node **lst, t_node *node, t_func_content_del f_del);
size_t		ft_lst_size(t_node *lst);
void		ft_lst_rev(t_node **lst);
int			ft_lst_map(t_node *lst, void *farg, int (*f)(t_node *, void *));
t_htbl		*ft_lst_to_htable(t_node *lst);

#endif
