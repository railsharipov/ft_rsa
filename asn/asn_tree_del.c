#include <ft_ssl.h>
#include <ssl_asn.h>

static int	__f_del(t_node *node)
{
	if (NULL == node)
		return (0);
	
	asn_item_del(node->content);
	SSL_FREE(node->key);
	SSL_FREE(node);

	return (0);
}

int	asn_tree_del(t_node *tree)
{
	if (NULL == tree)
		return (SSL_ERROR(INVALID_INPUT));
	
	ft_ntree_del(tree, __f_del);

	return (SSL_OK);
}
