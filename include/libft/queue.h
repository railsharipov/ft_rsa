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

t_queue		*ft_queue_create(void);
void		ft_queue_enqueue(t_queue *queue, const char *key, void *content, size_t size);
void		ft_queue_enqueue_node(t_queue *queue, t_node *node);
void		*ft_queue_dequeue(t_queue *queue);
t_node		*ft_queue_dequeue_node(t_queue *queue);
t_node		*ft_queue_first(t_queue *queue);
t_node		*ft_queue_last(t_queue *queue);
int			ft_queue_is_empty(t_queue *queue);
int			ft_queue_size(t_queue *queue);
void		ft_queue_del(t_queue *queue);
void		ft_queue_del_with_f_del(t_queue *queue, t_func_content_del f_del);
void		ft_queue_del_node(t_queue *queue, t_node *node);
void		ft_queue_del_node_with_f_del(t_queue *queue, t_node *node, t_func_content_del f_del);
t_htbl		*ft_queue_to_htable(t_queue *queue);
t_node		*ft_queue_pop_list(t_queue *queue);

#endif
