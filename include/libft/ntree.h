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

#ifndef LIBFT_NTREE_H
# define LIBFT_NTREE_H

# include <libft/common.h>
# include <libft/node.h>
# include <libft/htable.h>

t_node	*ft_ntree_construct(const char *);
int 	ft_ntree_dfs(t_node **res, t_node *, const void *, int (*f)(t_node *, const void *));
int		ft_ntree_dfs_cur_depth(void);
int		ft_ntree_bfs(t_node **res, t_node *, const void *, int (*f)(t_node *, const void *));
void	ft_ntree_del(t_node *, FUNC_CONTENT_DEL);
void	ft_ntree_print(t_node *, void (*f_print)(t_node *, int));
int		ft_ntree_size(t_node *);
t_node	*ft_ntree_iter(t_node *);
t_htbl	*ft_ntree_to_htable(t_node *);

#endif
