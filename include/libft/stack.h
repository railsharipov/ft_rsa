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

#ifndef LIBFT_STACK_H
# define LIBFT_STACK_H

# include <libft/common.h>
# include <libft/node.h>
# include <libft/htable.h>

typedef struct	s_stack
{
	t_node		*top;
}				t_stack;

t_stack		*ft_stack_create(void);
void		*ft_stack_pop(t_stack *stack);
t_node		*ft_stack_pop_node(t_stack *stack);
void		ft_stack_push(t_stack *stack, const char *key, void *content, size_t size);
void		ft_stack_push_node(t_stack *stack, t_node *node);
t_node		*ft_stack_peek(t_stack *stack);
int			ft_stack_is_empty(t_stack *stack);
int			ft_stack_size(t_stack *stack);
void		ft_stack_del(t_stack *stack);
void		ft_stack_del_with_f_del(t_stack *stack, FUNC_CONTENT_DEL f_del);
t_htbl		*ft_stack_to_htable(t_stack *stack);
t_node		*ft_stack_pop_list(t_stack *stack);

#endif
