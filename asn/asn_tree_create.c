#include <common.h>
#include <asn.h>
#include <json.h>
#include <libft/node.h>
#include <libft/ntree.h>
#include <libft/2darray.h>
#include <libft/string.h>
#include <libft/list.h>
#include <bnum.h>

t_node	*asn_tree_create(t_node *json_schema)
{
	t_htbl	*htbl;
	t_node	*type, *desc, *nodes;
	t_node	*asn_node, *child_asn_node;
	t_iasn	*asn_item;

	asn_node = ft_node_create();
	asn_item = asn_item_create();

	if (NULL == json_schema) {
		ASN_LOG(ERROR, "invalid json schema: expected object");
		goto label_error;
	}
	if (json_schema->type != JSON_TYPE_OBJECT) {
		ASN_LOG(ERROR, "invalid json schema: expected: %s, got: %s", json_get_type_name(JSON_TYPE_OBJECT), json_get_type_name(json_schema->type));
		goto label_error;
	}
	htbl = json_schema->content;

	desc = ft_htbl_get(htbl, "description");
	type = ft_htbl_get(htbl, "type");

	if (NULL == desc) {
		ASN_LOG(ERROR, "invalid json schema: expected description key");
		goto label_error;
	}
	if (NULL == type) {
		ASN_LOG(ERROR, "invalid json schema: expected type key");
		goto label_error;
	}

	if (SSL_OK != asn_item_set_type(asn_item, (char *)type->content)) {
		ASN_LOG(ERROR, "failed to set asn node type");
		goto label_error;
	}
	asn_item->description = ft_strdup((char *)desc->content);

	if (asn_item->tagnum == ASN_TAGNUM_SEQUENCE) {
		asn_node->type = JSON_TYPE_ARRAY;
		asn_node->f_del_content = json_get_f_del(JSON_TYPE_ARRAY);

		nodes = ft_htbl_get(htbl, "nodes");
		if (NULL == nodes) {
			ASN_LOG(ERROR, "invalid json schema: expected: %s, got: %s", json_get_type_name(JSON_TYPE_ARRAY), json_get_type_name(nodes->type));
			goto label_error;
		}

		for (t_node *child = nodes->content; child != NULL; child = child->next) {
			child_asn_node = __create_asn_node(child);
			if (NULL == child_asn_node) {
				ASN_LOG(ERROR, "failed to create asn node");
				goto label_error;
			}
			ft_lst_append(&asn_node->content, child_asn_node);
		}
		asn_node->size = ft_lst_size(asn_node->content);

	} else {
		asn_node->type = JSON_TYPE_BYTES;
		asn_node->f_del_content = asn_item_del;
		asn_node->content = asn_item;
		asn_node->size = sizeof(asn_item);
	}
	return (asn_node);

label_error:
	ft_node_del(asn_node);
	asn_item_del(asn_item);
	return (NULL);
}
