#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <json.h>
#include <libft.h>

t_node	*asn1_node_create(t_iasn *asn1_item) {
	t_node	*asn1_node;

	asn1_node = ft_node_create();
	asn1_node->type = JSON_TYPE_BYTES;
	asn1_node->f_del_content = (void (*)(void *))asn1_item_del;
	asn1_node->content = asn1_item;
	asn1_node->size = sizeof(asn1_item);

	return (asn1_node);
}

void	asn1_node_clear(t_node *asn1_node)
{
	if (NULL == asn1_node) {
		return;
	}
	if (NULL != asn1_node->content) {
		asn1_item_del(asn1_node->content);
		asn1_node->content = NULL;
	}
	asn1_node->type = JSON_TYPE_BYTES;
	asn1_node->f_del_content = (void (*)(void *))asn1_item_del;
	asn1_node->content = NULL;
	asn1_node->size = 0;
}

void	asn1_node_del(t_node *asn1_node)
{
	if (NULL == asn1_node) {
		return;
	}
	ft_node_del(asn1_node);
}
