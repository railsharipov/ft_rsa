#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>

static void	__del_node(t_node *node)
{
	SSL_FREE(node);
}

int	asn_transform(t_node *src_tree, t_node *dest_tree)
{
	t_htbl	*dest_items;
	t_node	*src_iter;
	t_node	*src_ref;
	t_node	*src_node;
	t_iasn	*src_item;
	t_iasn	*dest_item;

	if (NULL == src_tree || NULL == dest_tree)
		return (ASN_ERROR(INVALID_INPUT));

	src_iter = ft_ntree_iter(src_tree);
	dest_items = ft_ntree_htable(dest_tree);
	src_ref = src_iter;

	while (NULL != src_iter)
	{
		src_node = src_iter->content;

		if (NULL != (dest_item = ft_htbl_get(dest_items, src_node->key)))
		{
			if (NULL != (src_item = src_node->content))
			{
				ft_memcpy(
					dest_item->content, src_item->content, src_item->__size);
				dest_item->__size = src_item->__size;
				ft_strcpy(dest_item->__type, src_item->__type);
			}
			else
			{
				return (SSL_ERROR(INVALID_ASN_TREE));
			}
		}
		src_iter = src_iter->next;
	}
	ft_lst_del(src_ref, __del_node);

	return (SSL_OK);
}
