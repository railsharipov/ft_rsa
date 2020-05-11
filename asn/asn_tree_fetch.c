#include <ft_ssl.h>
#include <ssl_asn.h>

static int	__f_keycmp(t_node *node, const char *key)
{
	if (NULL == node)
	{
		return (0);
	}
	return (!ft_strcmp(node->key, key));
}

void	*asn_tree_fetch(t_node *asn_tree, const char *key)
{
	t_node	*node;

	node = ft_ntree_bfs(asn_tree, key, __f_keycmp);

	if (NULL == node)
	{
		return (NULL);
	}
	return (node->content);
}
