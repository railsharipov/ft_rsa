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

#ifndef LIBFT_QUEUE_H
# define LIBFT_QUEUE_H

# include <libft/common.h>
# include <libft/node.h>
# include <libft/htable.h>

typedef struct		s_queue
{
	t_node			*first;
	t_node			*last;
}					t_queue;

t_queue		*ft_queue_init(void);
void		ft_queue_enqueue(t_queue *, const char *, void *, size_t);
void		*ft_queue_dequeue(t_queue *);
t_node		*ft_queue_peek(t_queue *);
int			ft_queue_is_empty(t_queue *);
int			ft_queue_size(t_queue *);
void		ft_queue_del(t_queue *, FUNC_CONTENT_DEL);
void		ft_queue_del_node(t_queue *, t_node *, FUNC_CONTENT_DEL);
t_htbl		*ft_queue_htable(t_queue *);

#endif
