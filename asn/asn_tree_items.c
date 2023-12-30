#include <ft_ssl.h>

t_htbl	*asn_tree_items(t_node *tree)
{
	return (ft_ntree_to_htable(tree));
}

void	asn_tree_items_del(t_htbl *tree_items)
{
	ft_htbl_del(tree_items);
}
