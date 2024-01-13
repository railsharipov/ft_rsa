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

t_stack		*ft_stack_init(void);
void		*ft_stack_pop(t_stack *);
void		ft_stack_push(t_stack *, const char *, void *, size_t);
t_node		*ft_stack_peek(t_stack *);
int 		ft_stack_is_empty(t_stack *);
int 		ft_stack_size(t_stack *);
void		ft_stack_del(t_stack *, FUNC_CONTENT_DEL);
t_htbl		*ft_stack_htable(t_stack *);

#endif
