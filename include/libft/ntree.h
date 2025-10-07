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

typedef int 	(*t_func_ntree_map)(t_node *node, const void *farg);
typedef void	(*t_func_ntree_print)(t_node *node, int space);

t_node	*ft_ntree_construct(const char *map);
int 	ft_ntree_dfs(t_node **res, t_node *ntree, const void *farg, t_func_ntree_map f);
int		ft_ntree_dfs_cur_depth(void);
int		ft_ntree_bfs(t_node **res, t_node *node, const void *farg, t_func_ntree_map f);
void	ft_ntree_del(t_node *ntree, t_func_content_del f_del);
void	ft_ntree_print(t_node *ntree, t_func_ntree_print f_print);
int		ft_ntree_size(t_node *node);
t_node	*ft_ntree_iter(t_node *ntree);
t_htbl	*ft_ntree_to_htable(t_node *node);

#endif
