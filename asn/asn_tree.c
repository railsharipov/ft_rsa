#include <common.h>
#include <asn.h>
#include <der.h>
#include <json.h>
#include <io.h>
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
	t_node		*type, *desc, *data, *encapsulates, *is_construct;
	t_node		*asn_node, *child_asn_node;
	t_iasn		*asn_item;

	ASN_LOG(TRACE, "creating asn tree from json schema node of type: %s", json_get_type_name(json_schema->type));
	ASN_LOG(TRACE, "json schema: %s", asn_tree_dump(json_schema));

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
	data = ft_htbl_get(htbl, "data");
	encapsulates = ft_htbl_get(htbl, "encapsulates");
	is_construct = ft_htbl_get(htbl, "is_construct");

	if (NULL == desc || desc->type != JSON_TYPE_STRING) {
		ASN_LOG(ERROR, "invalid json schema: expected description key");
		goto label_error;
	}
	if (NULL == type || type->type != JSON_TYPE_STRING) {
		ASN_LOG(ERROR, "invalid json schema: expected type key");
		goto label_error;
	}
	ASN_LOG(TRACE, "setting asn item type to '%s'", (char *)type->content);

	if (SSL_OK != asn_item_set_type(asn_item, (char *)type->content)) {
		ASN_LOG(ERROR, "failed to set asn node type");
		goto label_error;
	}

	ASN_LOG(TRACE, "setting asn item description to '%s'", (char *)desc->content);
	asn_item->description = ft_strdup((char *)desc->content);

	if (encapsulates != NULL) {
		if (encapsulates->type != JSON_TYPE_BOOL_TRUE && encapsulates->type != JSON_TYPE_BOOL_FALSE) {
			ASN_LOG(ERROR, "invalid json schema: expected encapsulates key to be a boolean");
			goto label_error;
		}
		ASN_LOG(TRACE, "setting asn item encapsulates to '%s'", json_get_type_name(encapsulates->type));
		asn_item->encapsulates = (encapsulates->type == JSON_TYPE_BOOL_TRUE) ? SSL_TRUE : SSL_FALSE;
	}

	ASN_LOG(TRACE, "setting encode tag for asn item with tagnum: %#x", asn_item->tagnum);
	if (is_construct != NULL) {
		if (is_construct->type != JSON_TYPE_BOOL_TRUE && is_construct->type != JSON_TYPE_BOOL_FALSE) {
			ASN_LOG(ERROR, "invalid json schema: expected is_construct key to be a boolean");
			goto label_error;
		}
		if (is_construct->type == JSON_TYPE_BOOL_TRUE) {
			switch (asn_item->tagnum) {
				case ASN_TAGNUM_INT:
				case ASN_TAGNUM_NULL:
				case ASN_TAGNUM_OBJECT_ID:
				case ASN_TAGNUM_OBJECT_DESCR:
				case ASN_TAGNUM_BOOLEAN:
					ASN_LOG(ERROR, "bad schema: type '%s' must be primitive, but got construct", (char *)type->content);
					goto label_error;
			}
			asn_item->tag |= ASN_ENCODE_CONSTRUCT;
		} else {
			if (asn_item->tagnum == ASN_TAGNUM_SEQUENCE) {
				ASN_LOG(ERROR, "bad schema: type 'sequence' must be construct, but got primitive");
				goto label_error;
			}
			asn_item->tag |= ASN_ENCODE_PRIMITIVE;
		}
	} else {
		switch (asn_item->tagnum) {
			case ASN_TAGNUM_INT:
			case ASN_TAGNUM_NULL:
			case ASN_TAGNUM_OBJECT_ID:
			case ASN_TAGNUM_OBJECT_DESCR:
			case ASN_TAGNUM_BOOLEAN:
			case ASN_TAGNUM_OCTET_STRING:
			case ASN_TAGNUM_BIT_STRING:
				asn_item->tag |= ASN_ENCODE_PRIMITIVE;
				break;
			case ASN_TAGNUM_SEQUENCE:
				asn_item->tag |= ASN_ENCODE_CONSTRUCT;
				break;
			default:
				asn_item->tag |= ASN_ENCODE_PRIMITIVE;
				break;
		}
	}
	ASN_LOG(TRACE, "using asn_item encode tag: %s", SSL_FLAG(ASN_ENCODE_CONSTRUCT, asn_item->tag) ? "construct" : "primitive");

	if (data != NULL) {
		if (data->type == JSON_TYPE_ARRAY) {
			ASN_LOG(TRACE, "data type is json array, processing array nodes");

			for (t_node *child = data->content; child != NULL; child = child->next) {
				ASN_LOG(TRACE, "creating asn node from json array item: %p", child);
				child_asn_node = asn_tree_create(child);
				
				if (NULL == child_asn_node) {
					ASN_LOG(ERROR, "failed to create asn node for asn sequence");
					goto label_error;
				}
				ft_lst_append((t_node **)&asn_item->content, child_asn_node);
			}
		} else if (data->type == JSON_TYPE_OBJECT) {
			ASN_LOG(TRACE, "data type is json object, processing object node");
			ASN_LOG(TRACE, "creating asn node from json object: %p", data);

			child_asn_node = asn_tree_create(data);
			asn_item->content = child_asn_node;
			asn_item->size = 0;

		} else {
			ASN_LOG(TRACE, "data type is json %s", json_get_type_name(data->type));
			
			switch (asn_item->tagnum) {
				case ASN_TAGNUM_INT:
					ASN_LOG(TRACE, "creating asn node from json number: %p, size: %zu", data->content, data->size);
					asn_item->content = bnum_clone((t_num *)data->content);
					asn_item->size = data->size;
					break;
				case ASN_TAGNUM_NULL:
					ASN_LOG(TRACE, "creating asn node from json null: %p, size: %zu", data->content, data->size);
					asn_item->content = NULL;
					asn_item->size = 0;
					break;
				case ASN_TAGNUM_OBJECT_ID:
					ASN_LOG(TRACE, "creating asn node from json object id: %p, size: %zu", data->content, data->size);
					asn_item->content = ft_strdup((char *)data->content);
					asn_item->size = data->size;
					break;
				case ASN_TAGNUM_OBJECT_DESCR:
					ASN_LOG(TRACE, "creating asn node from json object description: %p, size: %zu", data->content, data->size);
					asn_item->content = ft_strdup((char *)data->content);
					asn_item->size = data->size;
					break;
				case ASN_TAGNUM_BOOLEAN:
					ASN_LOG(TRACE, "creating asn node from json boolean: %p, size: %zu", data->content, data->size);
					SSL_ALLOC(asn_item->content, sizeof(uint8_t));
					*(uint8_t *)asn_item->content = (data->type == JSON_TYPE_BOOL_TRUE) ? (uint8_t)1u : (uint8_t)0u;
					asn_item->size = 0;
					break;
				case ASN_TAGNUM_OCTET_STRING:
					ASN_LOG(TRACE, "creating asn node from json octet string: %p, size: %zu", data->content, data->size);
					asn_item->content = ft_memdup((char *)data->content, data->size);
					asn_item->size = data->size;
					break;
				case ASN_TAGNUM_BIT_STRING:
					ASN_LOG(TRACE, "creating asn node from json bit string: %p, size: %zu", data->content, data->size);
					asn_item->content = ft_memdup((char *)data->content, data->size);
					asn_item->size = data->size;
					break;
				case ASN_TAGNUM_SEQUENCE:
					ASN_LOG(TRACE, "creating asn node from json sequence: %p, size: %zu", data->content, data->size);
					asn_item->content = data->content;
					asn_item->size = ft_lst_size((t_node *)data->content);
					break;
				default:
					ASN_LOG(ERROR, "invalid json schema: unknown type: %s", (char *)type->content);
					goto label_error;
			}
		}
	}

	if (asn_item->encapsulates == SSL_TRUE) {
		if (asn_item->tagnum != ASN_TAGNUM_OCTET_STRING && asn_item->tagnum != ASN_TAGNUM_BIT_STRING) {
			ASN_LOG(ERROR, "bad schema: expected data to be encapsulated in octet or bit string but got %s", asn_item_get_type_name(asn_item));
			goto label_error;
		}
	}

	asn_node = asn_node_create(asn_item);
	ASN_LOG(TRACE, "successfully created asn node: %s", asn_tree_dump(asn_node));

	return (asn_node);

label_error:
	asn_item_del(asn_item);
	return (NULL);
}
