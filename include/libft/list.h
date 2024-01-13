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

void		ft_lst_append(t_node **, t_node *);
void		ft_lst_prepend(t_node **, t_node *);
void		ft_lst_del(t_node *, FUNC_CONTENT_DEL);
void		ft_lst_del_first(t_node **, FUNC_CONTENT_DEL);
void		ft_lst_del_last(t_node **, FUNC_CONTENT_DEL);
void		ft_lst_del_one(t_node **, t_node *, FUNC_CONTENT_DEL);
size_t		ft_lst_size(t_node *);
int			ft_lst_map(t_node *, void *farg, int (*f)(t_node *, void *));
t_htbl		*ft_lst_htable(t_node *);

#endif
