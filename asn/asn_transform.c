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
	dest_items = ft_ntree_to_set(dest_tree);
	src_ref = src_iter;

	while (NULL != src_iter)
	{
		src_node = src_iter->content;

		if (NULL != (dest_item = ft_htbl_get(dest_items, src_node->key)))
		{
			if (NULL != (src_item = src_node->content))
			{
				ft_memcpy(
					dest_item->content, src_item->content, src_item->size);
				dest_item->size = src_item->size;
				ft_strcpy(dest_item->description, src_item->description);
				dest_item->tag = src_item->tag;
				dest_item->tagnum = src_item->tagnum;
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
