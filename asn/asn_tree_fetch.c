#include <ft_ssl.h>
#include <ssl_asn.h>

static int	__f_keycmp(t_node *node, const void *key)
{
	if (NULL == node)
	{
		return (0);
	}
	return (!ft_strcmp(node->key, (const char *)key));
}

void	*asn_tree_fetch(t_node *asn_tree, const char *key)
{
	t_node	*node;

	ft_ntree_bfs(&node, asn_tree, key, __f_keycmp);

	if (NULL == node)
		return (NULL);
	
	return (node->content);
}
