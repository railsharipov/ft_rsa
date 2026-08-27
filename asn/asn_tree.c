#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <der.h>
#include <json.h>
#include <io.h>
#include <libft.h>
#include <bnum.h>

// temporary

t_node	*asn1_tree(const char *map) {
	return (NULL);
}

void	asn1_tree_del(t_node *tree)
{
	return ;
}

void	*asn1_tree_get(t_node *tree, const char *key)
{
	return (NULL);
}

t_htbl	*asn1_tree_items(t_node *tree)
{
	return (NULL);
}

void	asn1_tree_items_del(t_htbl *items)
{
	return ;
}

//

t_node	*asn1_tree_create(t_node *json_schema)
{
	t_htbl		*htbl;
	t_node		*type, *desc, *data, *encapsulates, *is_construct;
	t_node		*asn1_node, *child_asn1_node;
	t_iasn		*asn1_item;

	SSL_LOG(TRACE, "creating asn tree from json schema node of type: %s", json_get_type_name(json_schema->type));
	SSL_LOG(TRACE, "json schema: %s", asn1_tree_dump(json_schema));

	asn1_item = asn1_item_create();

	if (NULL == json_schema) {
		SSL_LOG(ERROR, "invalid json schema: expected object");
		goto label_error;
	}
	if (json_schema->type != JSON_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid json schema: expected: %s, got: %s", json_get_type_name(JSON_TYPE_OBJECT), json_get_type_name(json_schema->type));
		goto label_error;
	}
	htbl = json_schema->content;

	desc = ft_htbl_get(htbl, "description");
	type = ft_htbl_get(htbl, "type");
	data = ft_htbl_get(htbl, "data");
	encapsulates = ft_htbl_get(htbl, "encapsulates");
	is_construct = ft_htbl_get(htbl, "is_construct");

	if (NULL == desc || desc->type != JSON_TYPE_STRING) {
		SSL_LOG(ERROR, "invalid json schema: expected description key");
		goto label_error;
	}
	if (NULL == type || type->type != JSON_TYPE_STRING) {
		SSL_LOG(ERROR, "invalid json schema: expected type key");
		goto label_error;
	}
	SSL_LOG(TRACE, "setting asn item type to '%s'", (char *)type->content);

	if (SSL_OK != asn1_item_set_type(asn1_item, (char *)type->content)) {
		SSL_LOG(ERROR, "failed to set asn node type");
		goto label_error;
	}

	SSL_LOG(TRACE, "setting asn item description to '%s'", (char *)desc->content);
	asn1_item->description = ft_strdup((char *)desc->content);

	if (encapsulates != NULL) {
		if (encapsulates->type != JSON_TYPE_BOOL_TRUE && encapsulates->type != JSON_TYPE_BOOL_FALSE) {
			SSL_LOG(ERROR, "invalid json schema: expected encapsulates key to be a boolean");
			goto label_error;
		}
		SSL_LOG(TRACE, "setting asn item encapsulates to '%s'", json_get_type_name(encapsulates->type));
		asn1_item->encapsulates = (encapsulates->type == JSON_TYPE_BOOL_TRUE) ? SSL_TRUE : SSL_FALSE;
	}

	SSL_LOG(TRACE, "setting encode tag for asn item with tagnum: %#x", asn1_item->tagnum);
	if (is_construct != NULL) {
		if (is_construct->type != JSON_TYPE_BOOL_TRUE && is_construct->type != JSON_TYPE_BOOL_FALSE) {
			SSL_LOG(ERROR, "invalid json schema: expected is_construct key to be a boolean");
			goto label_error;
		}
		if (is_construct->type == JSON_TYPE_BOOL_TRUE) {
			switch (asn1_item->tagnum) {
				case ASN_TAGNUM_INT:
				case ASN_TAGNUM_NULL:
				case ASN_TAGNUM_OBJECT_ID:
				case ASN_TAGNUM_OBJECT_DESCR:
				case ASN_TAGNUM_BOOLEAN:
					SSL_LOG(ERROR, "bad schema: type '%s' must be primitive, but got construct", (char *)type->content);
					goto label_error;
			}
			asn1_item->tag |= ASN_ENCODE_CONSTRUCT;
		} else {
			if (asn1_item->tagnum == ASN_TAGNUM_SEQUENCE) {
				SSL_LOG(ERROR, "bad schema: type 'sequence' must be construct, but got primitive");
				goto label_error;
			}
			asn1_item->tag |= ASN_ENCODE_PRIMITIVE;
		}
	} else {
		switch (asn1_item->tagnum) {
			case ASN_TAGNUM_INT:
			case ASN_TAGNUM_NULL:
			case ASN_TAGNUM_OBJECT_ID:
			case ASN_TAGNUM_OBJECT_DESCR:
			case ASN_TAGNUM_BOOLEAN:
			case ASN_TAGNUM_OCTET_STRING:
			case ASN_TAGNUM_BIT_STRING:
				asn1_item->tag |= ASN_ENCODE_PRIMITIVE;
				break;
			case ASN_TAGNUM_SEQUENCE:
				asn1_item->tag |= ASN_ENCODE_CONSTRUCT;
				break;
			default:
				asn1_item->tag |= ASN_ENCODE_PRIMITIVE;
				break;
		}
	}
	SSL_LOG(TRACE, "using asn1_item encode tag: %s", SSL_FLAG(ASN_ENCODE_CONSTRUCT, asn1_item->tag) ? "construct" : "primitive");

	if (data != NULL) {
		if (data->type == JSON_TYPE_ARRAY) {
			SSL_LOG(TRACE, "data type is json array, processing array nodes");

			for (t_node *child = data->content; child != NULL; child = child->next) {
				SSL_LOG(TRACE, "creating asn node from json array item: %p", child);
				child_asn1_node = asn1_tree_create(child);

				if (NULL == child_asn1_node) {
					SSL_LOG(ERROR, "failed to create asn node for asn sequence");
					goto label_error;
				}
				ft_lst_append((t_node **)&asn1_item->content, child_asn1_node);
			}
		} else if (data->type == JSON_TYPE_OBJECT) {
			SSL_LOG(TRACE, "data type is json object, processing object node");
			SSL_LOG(TRACE, "creating asn node from json object: %p", data);

			child_asn1_node = asn1_tree_create(data);
			asn1_item->content = child_asn1_node;
			asn1_item->size = 0;

		} else {
			SSL_LOG(TRACE, "data type is json %s", json_get_type_name(data->type));

			switch (asn1_item->tagnum) {
				case ASN_TAGNUM_INT:
					SSL_LOG(TRACE, "creating asn node from json number: %p, size: %zu", data->content, data->size);
					asn1_item->content = bnum_clone((t_num *)data->content);
					asn1_item->size = data->size;
					break;
				case ASN_TAGNUM_NULL:
					SSL_LOG(TRACE, "creating asn node from json null: %p, size: %zu", data->content, data->size);
					asn1_item->content = NULL;
					asn1_item->size = 0;
					break;
				case ASN_TAGNUM_OBJECT_ID:
					SSL_LOG(TRACE, "creating asn node from json object id: %p, size: %zu", data->content, data->size);
					asn1_item->content = ft_strdup((char *)data->content);
					asn1_item->size = data->size;
					break;
				case ASN_TAGNUM_OBJECT_DESCR:
					SSL_LOG(TRACE, "creating asn node from json object description: %p, size: %zu", data->content, data->size);
					asn1_item->content = ft_strdup((char *)data->content);
					asn1_item->size = data->size;
					break;
				case ASN_TAGNUM_BOOLEAN:
					SSL_LOG(TRACE, "creating asn node from json boolean: %p, size: %zu", data->content, data->size);
					SSL_ALLOC(asn1_item->content, sizeof(uint8_t));
					*(uint8_t *)asn1_item->content = (data->type == JSON_TYPE_BOOL_TRUE) ? (uint8_t)1u : (uint8_t)0u;
					asn1_item->size = 0;
					break;
				case ASN_TAGNUM_OCTET_STRING:
					SSL_LOG(TRACE, "creating asn node from json octet string: %p, size: %zu", data->content, data->size);
					asn1_item->content = ft_memdup((char *)data->content, data->size);
					asn1_item->size = data->size;
					break;
				case ASN_TAGNUM_BIT_STRING:
					SSL_LOG(TRACE, "creating asn node from json bit string: %p, size: %zu", data->content, data->size);
					asn1_item->content = ft_memdup((char *)data->content, data->size);
					asn1_item->size = data->size;
					break;
				case ASN_TAGNUM_SEQUENCE:
					SSL_LOG(TRACE, "creating asn node from json sequence: %p, size: %zu", data->content, data->size);
					asn1_item->content = data->content;
					asn1_item->size = ft_lst_size((t_node *)data->content);
					break;
				default:
					SSL_LOG(ERROR, "invalid json schema: unknown type: %s", (char *)type->content);
					goto label_error;
			}
		}
	}

	if (asn1_item->encapsulates == SSL_TRUE) {
		if (asn1_item->tagnum != ASN_TAGNUM_OCTET_STRING && asn1_item->tagnum != ASN_TAGNUM_BIT_STRING) {
			SSL_LOG(ERROR, "bad schema: expected data to be encapsulated in octet or bit string but got %s", asn1_item_get_type_name(asn1_item));
			goto label_error;
		}
	}

	asn1_node = asn1_node_create(asn1_item);
	SSL_LOG(TRACE, "successfully created asn node: %s", asn1_tree_dump(asn1_node));

	return (asn1_node);

label_error:
	asn1_item_del(asn1_item);
	return (NULL);
}
