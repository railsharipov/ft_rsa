#include <ft_ssl.h>
#include <ssl_asn.h>

static int	__init_func(t_node *node, const void *p);
static void	__del_func(t_node *node);
static int	__abort_with_error();

static int	__error_status;

t_node	*asn_tree(const char *map)
{
	t_node	*tree;

	__error_status = SSL_OK;

	tree = ft_ntree_construct(map);
	ft_ntree_bfs(tree, NULL, __init_func);

	if (__error_status != SSL_OK)
	{
		ft_ntree_del(tree, __del_func);
		return (NULL);
	}

	return (tree);
}

static int	__init_func(t_node *node, const void *p)
{
	t_iasn	*item;
	char	**strings;
	size_t	num_strings;
	char	type_key[128] = {0};
	char	description[256] = {0};

	if (NULL == node)
		return (0);

	strings = ft_strsplit(node->key, ':');
	num_strings = ft_2darray_len_null_terminated((void **)strings);

	if (NULL == strings || num_strings != 2)
		return (__abort_with_error());

	ft_strncpy(type_key, strings[0], sizeof(type_key)-1);
	ft_strncpy(description, strings[1], sizeof(description)-1);

	ft_2darray_del_null_terminated((void **)strings);

	item = asn_item_init();

	if (SSL_OK != asn_item_set_type(item, type_key))
		return (__abort_with_error());

	item->description = ft_strdup(description);
	node->content = item;

	return (0);
}

static void	__del_func(t_node *node)
{
	t_iasn	*item;

	if (NULL != node->content)
		asn_item_del((t_iasn *)node->content);
}

static int	__abort_with_error()
{
	__error_status = SSL_FAIL;
	return (-1);
}
