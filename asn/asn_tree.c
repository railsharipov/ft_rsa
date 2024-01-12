#include <ft_ssl.h>
#include <ssl_asn.h>

static int	__init_func(t_node *node, const void *p);
static void	__f_del_content(void *content);

static int	__error_status;

t_node	*asn_tree(const char *map)
{
	t_node	*tree;

	__error_status = SSL_OK;

	tree = ft_ntree_construct(map);

	if (ft_ntree_bfs(NULL, tree, NULL, __init_func) < 0)
	{
		ft_ntree_del(tree, __f_del_content);
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
		return (-1);

	ft_strncpy(type_key, strings[0], sizeof(type_key)-1);
	ft_strncpy(description, strings[1], sizeof(description)-1);

	ft_2darray_del_null_terminated((void **)strings);

	item = asn_item_init();

	if (SSL_OK != asn_item_set_type(item, type_key))
		return (-1);

	item->description = ft_strdup(description);
	node->content = item;

	return (0);
}

static void	__f_del_content(void *content)
{
	asn_item_del((t_iasn *)content);
}
