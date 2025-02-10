#ifndef LIBFT_TUPLE_H
# define LIBFT_TUPLE_H

# include <libft/common.h>
# include <libft/node.h>

typedef struct	s_tuple
{
	t_node		*__head;
	t_node		*__tail;
}				t_tuple;

t_tuple		*ft_tuple_new(void *head, size_t head_size, void *tail, size_t tail_size);
t_tuple		*ft_tuple_new_with_f_del(void *head, size_t head_size, void *tail, size_t tail_size, FUNC_CONTENT_DEL f_del);
void		ft_tuple_del(t_tuple *tuple);
void		ft_tuple_del_with_f_del(t_tuple *tuple, FUNC_CONTENT_DEL f_del);
void		*ft_tuple_get(t_tuple *tuple, int index);

#endif
