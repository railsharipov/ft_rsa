#include <ft_ssl.h>
#include <ssl_asn.h>

static void	__f_del_content(void *content)
{
	asn_item_del((t_iasn *)content);
}

void	asn_tree_del(t_node *tree)
{
	if (NULL == tree) {
		return ;
	}
	ft_ntree_del(tree, __f_del_content);
}
