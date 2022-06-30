#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_map.h>

static t_node	*__oid_tree;

static void	__oid_tree_init(void);
static void	__oid_tree_del(void);
static int	__f_init(t_node *node, const void *farg);
static int	__f_find_name(t_node *node, const void *farg);
static int	__f_find_oid(t_node *node, const void *farg);

char	*asn_oid_tree_get_name(const char *oid)
{
	t_node	*node;
	char	*name;

	name = NULL;

	if (NULL == oid)
	{
		return (NULL);
	}
	__oid_tree_init();

	if (NULL != (node = ft_ntree_bfs(__oid_tree, oid, __f_find_oid)))
	{
		name = ft_strdup(node->content);
	}
	__oid_tree_del();

	return (name);
}

char	*asn_oid_tree_get_oid(const char *name)
{
	t_node	*node;
	char	*oid;

	oid = NULL;

	if (NULL == name)
	{
		return (NULL);
	}
	__oid_tree_init();

	if (NULL != (node = ft_ntree_bfs(__oid_tree, name, __f_find_name)))
	{
		oid = ft_strdup(node->key);
	}
	__oid_tree_del();

	return (oid);
}

static void	__oid_tree_del(void)
{
	ft_ntree_del(__oid_tree, NULL);
	__oid_tree = NULL;
}

static int	__f_init(t_node *node, const void *farg)
{
	char	**keys;

	(void)farg;

	if (NULL == node)
	{
		return (0);
	}
	keys = ft_strsplit(node->key, ':');
	SSL_FREE(node->key);
	SSL_CHECK(NULL != keys);
	node->key = keys[0];
	node->content = keys[1];
	SSL_FREE(keys);

	return (0);
}

static void	__oid_tree_init(void)
{
	__oid_tree = ft_ntree_construct(MAP_ASN_OID_TREE);
	ft_ntree_bfs(__oid_tree, NULL, __f_init);
}

static int	__f_find_oid(t_node *node, const void *farg)
{
	if (NULL == node)
	{
		return (0);
	}
	if (!ft_strcmp(node->key, farg))
	{
		return (1);
	}
	return (0);
}

static int	__f_find_name(t_node *node, const void *farg)
{
	if (NULL == node)
	{
		return (0);
	}
	if (!ft_strcmp(node->content, farg))
	{
		return (1);
	}
	return (0);
}
