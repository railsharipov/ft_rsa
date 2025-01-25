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

typedef int 	(*FUNC_NTREE_MAP)(t_node *node, const void *farg);
typedef void	(*FUNC_NTREE_PRINT)(t_node *node, int space);

t_node	*ft_ntree_construct(const char *map);
int 	ft_ntree_dfs(t_node **res, t_node *ntree, const void *farg, FUNC_NTREE_MAP f);
int		ft_ntree_dfs_cur_depth(void);
int		ft_ntree_bfs(t_node **res, t_node *node, const void *farg, FUNC_NTREE_MAP f);
void	ft_ntree_del(t_node *ntree, FUNC_CONTENT_DEL f_del);
void	ft_ntree_print(t_node *ntree, FUNC_NTREE_PRINT f_print);
int		ft_ntree_size(t_node *node);
t_node	*ft_ntree_iter(t_node *ntree);
t_htbl	*ft_ntree_to_htable(t_node *node);

#endif
