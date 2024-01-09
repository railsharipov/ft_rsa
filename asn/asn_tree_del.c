#include <ft_ssl.h>
#include <ssl_asn.h>

static void	__f_del(t_node *node)
{
	asn_item_del(node->content);
	SSL_FREE(node->key);
	SSL_FREE(node);
}

void	asn_tree_del(t_node *tree)
{
	if (NULL == tree) {
		return ;
	}
	ft_ntree_del(tree, __f_del);
}
