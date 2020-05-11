#include <ft_ssl.h>
#include <ssl_asn.h>

static const struct {
	char	*type;
	t_iasn	*(*func)(char *, char *);
} T[] = {
	{ "sequence",		asn_construct_sequence	},
	{ "boolean",		asn_primitive_bool		},
	{ "int",			asn_primitive_int		},
	{ "ostring",		asn_primitive_ostring	},
	{ "bitstring",		asn_primitive_bitstring	},
	{ "objectId",		asn_primitive_oid		},
	{ "null",			asn_primitive_null		}
};

static t_htbl	*__init_func_htable(void)
{
	t_htbl	*func_htable;
	int		ntypes;
	int		idx;

	ntypes = sizeof(T)/sizeof(*T);
	func_htable = ft_htbl_init(ntypes);

	for (idx = 0; idx < ntypes; idx++)
	{
		ft_htbl_add(func_htable, T[idx].type, T[idx].func);
	}
	return (func_htable);
}

static int	__init_func(t_node *node, void *ptr)
{
	t_htbl	*func_htable;
	t_iasn	*(*f)(char *, char *);
	char	**keys;

	if (NULL == node)
	{
		return (0);
	}
	func_htable = (t_htbl *)(ptr);
	keys = ft_strsplit(node->key, ':');
	SSL_FREE(node->key);
	SSL_CHECK(NULL != keys);
	f = ft_htbl_get(func_htable, keys[0]);
	SSL_CHECK(NULL != f);
	node->content = f(keys[0], keys[1]);
	node->key = ft_strdup(keys[1]);
	ft_2darray_del(keys, -1);

	return (0);
}

t_node	*asn_tree(char *map)
{
	t_node	*tree;
	t_htbl	*func_htable;

	func_htable = __init_func_htable();
	tree = ft_ntree_construct(map);
	ft_ntree_bfs(tree, func_htable, __init_func);
	ft_htbl_del(func_htable);

	return (tree);
}
