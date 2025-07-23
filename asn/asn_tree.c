#include <common.h>
#include <asn.h>
#include <json.h>
#include <libft/node.h>
#include <libft/ntree.h>
#include <libft/2darray.h>
#include <libft/string.h>
#include <libft/list.h>
#include <bnum.h>

// temporary

t_node	*asn_tree(const char *map) {
	return (NULL);
}

void	asn_tree_del(t_node *tree)
{
	return ;
}

void	*asn_tree_get(t_node *tree, const char *key)
{
	return (NULL);
}

t_htbl	*asn_tree_items(t_node *tree)
{
	return (NULL);
}

void	asn_tree_items_del(t_htbl *items)
{
	return ;
}

// 

t_node	*asn_tree_create(t_node *json_schema)
{
	t_htbl		*htbl;
	t_node		*type, *desc, *nodes;
	t_node		*asn_node, *child_asn_node;
	t_iasn		*asn_item;

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
	nodes = ft_htbl_get(htbl, "nodes");

	if (NULL == desc || desc->type != JSON_TYPE_STRING) {
		ASN_LOG(ERROR, "invalid json schema: expected description key");
		goto label_error;
	}
	if (NULL == type || type->type != JSON_TYPE_STRING) {
		ASN_LOG(ERROR, "invalid json schema: expected type key");
		goto label_error;
	}

	if (SSL_OK != asn_item_set_type(asn_item, (char *)type->content)) {
		ASN_LOG(ERROR, "failed to set asn node type");
		goto label_error;
	}
	asn_item->description = ft_strdup((char *)desc->content);

	if (nodes != NULL) {
		for (t_node *child = nodes->content; child != NULL; child = child->next) {
			child_asn_node = asn_tree_create(child);

			if (NULL == child_asn_node) {
				ASN_LOG(ERROR, "failed to create a child asn node");
				goto label_error;
			}
			ft_lst_append((t_node **)&asn_item->content, child_asn_node);
		}
	}

	return (asn_node_create(asn_item));

label_error:
	asn_item_del(asn_item);
	return (NULL);
}
