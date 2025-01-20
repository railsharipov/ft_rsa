#include <libft/tuple.h>
#include <libft/alloc.h>

static t_tuple	*__create(void);

t_tuple *ft_tuple_new(void *head, size_t head_size, void *tail, size_t tail_size)
{
	t_tuple *tuple;

	tuple = __create();
	tuple->head = ft_node_new(NULL, head, head_size);
	tuple->tail = ft_node_new(NULL, tail, tail_size);
	return (tuple);
}

t_tuple *ft_tuple_new_with_f_del(void *head, size_t head_size, void *tail, size_t tail_size, FUNC_CONTENT_DEL f_del)
{
	t_tuple *tuple;

	tuple = __create();
	tuple->head = ft_node_new_with_f_del(NULL, head, head_size, f_del);
	tuple->tail = ft_node_new_with_f_del(NULL, tail, tail_size, f_del);
	return (tuple);
}

void	ft_tuple_del(t_tuple *tuple)
{
	if (NULL == tuple) {
		return ;
	}
	ft_node_del(tuple->head);
	ft_node_del(tuple->tail);
	LIBFT_FREE(tuple);
}

void 	ft_tuple_del_with_f_del(t_tuple *tuple, FUNC_CONTENT_DEL f_del)
{
	if (NULL == tuple) {
		return ;
	}
	ft_node_del_with_f_del(tuple->head, f_del);
	ft_node_del_with_f_del(tuple->tail, f_del);
	LIBFT_FREE(tuple);
}

void 	*ft_tuple_get(t_tuple *tuple, int index)
{
	if (NULL == tuple) {
		return (NULL);
	}
	if (index == 0) {
		if (NULL == tuple->head) {
			return (NULL);
		}
		return (tuple->head->content);
	}
	if (NULL == tuple->tail) {
		return (NULL);
	}
	return (tuple->tail->content);
}

static t_tuple	*__create(void)
{
	t_tuple *tuple;

	LIBFT_ALLOC(tuple, sizeof(t_tuple));
	ft_bzero(tuple, sizeof(t_tuple));
	return (tuple);
}