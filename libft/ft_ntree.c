#include <libft/node.h>
#include <libft/ntree.h>
#include <libft/queue.h>
#include <libft/alloc.h>
#include <libft/htable.h>
#include <libft/std.h>
#include <libft/string.h>
#include <libft/2darray.h>

static int	__calc_size_func(t_node *node, const void *farg);
static void	__del_node_recur(t_node *node, FUNC_CONTENT_DEL f_del);
static int	__iter_func(t_node *node, const void *ptr);
static int	__htable_func(t_node *node, const void *farg);
static void	__f_print_default(t_node *node, int space);
static int __dfs_recur(t_node *node, const void *farg, int (*f)(t_node *, const void *), t_node **res);
static void	__print_recur(t_node *node, int *space, void (*f_print)(t_node *, int));
static t_node *__create_recur(char **key, int *idx);
static void __del_node_func(t_node *node);
static int __dfs_recur(t_node *node, const void *farg, int (*f)(t_node *, const void *), t_node **res);

static int __depth;

int	ft_ntree_size(t_node *node)
{
	int	size;

	size = 0;
	ft_ntree_bfs(NULL, node, &size, __calc_size_func);

	return (size);
}

void	ft_ntree_del(t_node *ntree, FUNC_CONTENT_DEL f_del)
{
	if (NULL == ntree) {
		return ;
	}
	__del_node_recur(ntree, f_del);
}

t_node	*ft_ntree_iter(t_node *ntree)
{
	t_queue	*queue;
	t_node	*iter;

	if (NULL == ntree || NULL == (queue = ft_queue_create())) {
		return (NULL);
	}
	ft_ntree_bfs(NULL, ntree, queue, __iter_func);

	iter = ft_queue_first(queue);
	LIBFT_FREE(queue);

	return (iter);
}

t_htbl	*ft_ntree_to_htable(t_node *node)
{
	t_htbl	*htbl;

	if (NULL == node) {
		return (NULL);
	}

	htbl = ft_htbl_create(ft_ntree_size(node));
	ft_ntree_bfs(NULL, node, htbl, __htable_func);

	return (htbl);
}

void	ft_ntree_print(t_node *ntree, FUNC_NTREE_PRINT f_print)
{
	int	space;

	space = 0;

	if (NULL == f_print) {
		f_print = __f_print_default;
	}

	__print_recur(ntree, &space, f_print);
}

t_node *ft_ntree_construct(const char *map)
{
	// Construct ntree from a map.
	//
	// map: 	"colors{ RGB{ 25 27 30 } RGBA{ 32 35 40 0.5 } } sizes{ S M L }"
	//
	// result:
	//							colors         ------>       sizes
	//						/			\					/  \   \
	//					RGB	  			RGBA				S   M   L
	//				/	|	\		/	|	\	\
	//			   25	27	30		32	35	40	0.5

	t_node *ntree;
	char **keys;
	int idx;

	if (NULL == map) {
		return (NULL);
	}

	idx = 0;
	keys = ft_strsplit(map, ' ');

	if (NULL == keys) {
		return (NULL);
	}

	ntree = __create_recur(keys, &idx);
	ft_2darray_del_null_terminated((void **)keys);

	return (ntree);
}

int	ft_ntree_bfs(t_node **res, t_node *node, const void *farg, FUNC_NTREE_MAP f)
{
	t_queue	*queue;
	t_node	*child_node;
	t_node	*result_node;
	int		ret;

	if (NULL == node || NULL == f) {
		return (-1);
	}
	queue = ft_queue_create();
	result_node = NULL;

	while (node) {
		ft_queue_enqueue(queue, NULL, node, 0);
		node = node->next;
	}
	while (!ft_queue_is_empty(queue)) {
		node = ft_queue_dequeue(queue);

		if (ft_node_is_parent(node)) {
			child_node = node->nodes;

			while (NULL != child_node) {
				ft_queue_enqueue(queue, NULL, child_node, 0);
				child_node = child_node->next;
			}
		}
		ret = f(node, farg);

		if (ret < 0) {
			break ;
		} else if (ret == 1) {
			result_node = node;
			break ;
		}
	}
	if (res != NULL) {
		*res = result_node;
	}
	ft_queue_del(queue);

	return (ret);
}

int	ft_ntree_dfs(t_node **res, t_node *ntree, const void *farg, FUNC_NTREE_MAP f)
{
	t_node	*node;
	int		ret;

	__depth = 0;

	if ((NULL == ntree) || (NULL == f)) {
		return (-1);
	}

	ret = __dfs_recur(ntree, farg, f, &node);

	if (res != NULL) {
		*res = node;
	}

	return (ret);
}

int ft_ntree_dfs_cur_depth(void)
{
	return (__depth);
}

static int __dfs_recur(t_node *node, const void *farg, FUNC_NTREE_MAP f, t_node **res)
{
	int ret;

	if (NULL == node) {
		*res = NULL;
		return (0);
	}

	if ((ret = f(node, farg)) != 0) {
		*res = (ret == 1) ? node : NULL;
		return (ret);
	}

	__depth++;

	ret = __dfs_recur(node->nodes, farg, f, res);

	__depth--;

	if (ret == 0) {
		ret = __dfs_recur(node->next, farg, f, res);
	}

	return (ret);
}

static int	__calc_size_func(t_node *node, const void *farg)
{
	int	*size;

	(void)node;
	size = (int *)farg;
	*size = *size + 1;

	return (0);
}

static void	__del_node_recur(t_node *node, FUNC_CONTENT_DEL f_del)
{
	if (NULL == node) {
		return ;
	}
	__del_node_recur(node->nodes, f_del);
	__del_node_recur(node->next, f_del);
	ft_node_del_with_f_del(node, f_del);
}

static int	__iter_func(t_node *node, const void *ptr)
{
	t_queue	*queue;

	queue = (t_queue *)(ptr);
	ft_queue_enqueue(queue, NULL, node, 0);

	return (0);
}

static int	__htable_func(t_node *node, const void *farg)
{
	ft_htbl_add((t_htbl *)farg, node->content, node->key);

	return (0);
}

static void	__f_print_default(t_node *node, int space)
{
	(void)space;

	if (NULL != node) {
		if (ft_node_is_parent(node)) {
			ft_printf("%s\n", node->key, node->content);
		}
		else
			ft_printf("%-25.25s %p\n", node->key, node->content);
	}
}

static void	__print_recur(t_node *node, int *space, void (*f_print)(t_node *, int))
{
	if (NULL == node) {
		return ;
	}

	f_print(node, *space);

	(*space)++;

	__print_recur(node->nodes, space, f_print);

	(*space)--;

	__print_recur(node->next, space, f_print);
}

static t_node *__create_recur(char **key, int *idx)
{
	t_node	*node;
	char	**childkey;

	if (NULL == key[*idx]) {
		return (NULL);
	}

	if (!ft_strcmp(key[*idx], "}")) {
		(*idx)++;
		return (NULL);
	}

	node = ft_node_new(key[(*idx)++], NULL, 0);

	if (ft_strchr(node->key, '{')) {
		childkey = ft_strsplit(node->key, '{');
		LIBFT_FREE(node->key);

		if (NULL != childkey) {
			node->key = ft_strdup(childkey[0]);
		}
		else
			node->key = NULL;

		ft_2darray_del_null_terminated((void **)childkey);

		node->nodes = __create_recur(key, idx);
	}

	node->next = __create_recur(key, idx);

	return (node);
}