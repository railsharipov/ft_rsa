#include <common.h>
#include <logger.h>
#include <asn.h>
#include <json.h>
#include <libft.h>

t_node	*asn_node_create(t_iasn *asn_item) {
	t_node	*asn_node;

	asn_node = ft_node_create();
	asn_node->type = JSON_TYPE_BYTES;
	asn_node->f_del_content = (void (*)(void *))asn_item_del;
	asn_node->content = asn_item;
	asn_node->size = sizeof(asn_item);

	return (asn_node);
}

void	asn_node_clear(t_node *asn_node)
{
	if (NULL == asn_node) {
		return;
	}
	if (NULL != asn_node->content) {
		asn_item_del(asn_node->content);
		asn_node->content = NULL;
	}
	asn_node->type = JSON_TYPE_BYTES;
	asn_node->f_del_content = (void (*)(void *))asn_item_del;
	asn_node->content = NULL;
	asn_node->size = 0;
}

void	asn_node_del(t_node *asn_node)
{
	if (NULL == asn_node) {
		return;
	}
	ft_node_del(asn_node);
}
