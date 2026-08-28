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
t_node	*asn1_node_create(t_iasn *asn1_item)
{
	t_node *asn1_node = ft_node_create();
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

t_node	*asn1_tree_create(t_node *json_schema)
{
	t_htbl		*htbl;
	t_node		*type, *desc, *value;
	t_node		*asn1_node, *child_asn1_node;
	t_iasn		*asn1_item;

	SSL_LOG(TRACE, "creating asn tree from json schema");

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
	value = ft_htbl_get(htbl, "value");

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

	SSL_LOG(TRACE, "setting encode tag for asn item with tagnum: %#x", asn1_item->tagnum);
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
		SSL_LOG(ERROR, "unknown asn1 tagnum: %#x", asn1_item->tagnum);
		goto label_error;
	}
	SSL_LOG(TRACE, "using asn1_item encode tag: %s", SSL_FLAG(ASN_ENCODE_CONSTRUCT, asn1_item->tag) ? "construct" : "primitive");

	if (value != NULL) {
		if (value->type == JSON_TYPE_ARRAY) {
			SSL_LOG(TRACE, "value type is json array, processing array nodes");

			for (t_node *child = value->content; child != NULL; child = child->next) {
				SSL_LOG(TRACE, "creating asn node from json array item: %p", child);
				child_asn1_node = asn1_tree_create(child);

				if (NULL == child_asn1_node) {
					SSL_LOG(ERROR, "failed to create asn node for asn sequence");
					goto label_error;
				}
				ft_lst_append((t_node **)&asn1_item->content, child_asn1_node);
			}
		} else if (value->type == JSON_TYPE_OBJECT) {
			SSL_LOG(TRACE, "value type is json object, processing object node");
			SSL_LOG(TRACE, "creating asn node from json object: %p", value);

			child_asn1_node = asn1_tree_create(value);
			asn1_item->content = child_asn1_node;
			asn1_item->size = 0;

		} else {
			SSL_LOG(TRACE, "value type is json %s", json_get_type_name(value->type));

			switch (asn1_item->tagnum) {
				case ASN_TAGNUM_INT:
					SSL_LOG(TRACE, "creating asn node from json number: %p, size: %zu", value->content, value->size);
					asn1_item->content = bnum_clone((t_num *)value->content);
					asn1_item->size = value->size;
					break;
				case ASN_TAGNUM_NULL:
					SSL_LOG(TRACE, "creating asn node from json null: %p, size: %zu", value->content, value->size);
					asn1_item->content = NULL;
					asn1_item->size = 0;
					break;
				case ASN_TAGNUM_OBJECT_ID:
					SSL_LOG(TRACE, "creating asn node from json object id: %p, size: %zu", value->content, value->size);
					asn1_item->content = ft_strdup((char *)value->content);
					asn1_item->size = value->size;
					break;
				case ASN_TAGNUM_OBJECT_DESCR:
					SSL_LOG(TRACE, "creating asn node from json object description: %p, size: %zu", value->content, value->size);
					asn1_item->content = ft_strdup((char *)value->content);
					asn1_item->size = value->size;
					break;
				case ASN_TAGNUM_BOOLEAN:
					SSL_LOG(TRACE, "creating asn node from json boolean: %p, size: %zu", value->content, value->size);
					SSL_ALLOC(asn1_item->content, sizeof(uint8_t));
					*(uint8_t *)asn1_item->content = (value->type == JSON_TYPE_BOOL_TRUE) ? (uint8_t)1u : (uint8_t)0u;
					asn1_item->size = 0;
					break;
				case ASN_TAGNUM_OCTET_STRING:
					SSL_LOG(TRACE, "creating asn node from json octet string: %p, size: %zu", value->content, value->size);
					asn1_item->content = ft_memdup((char *)value->content, value->size);
					asn1_item->size = value->size;
					break;
				case ASN_TAGNUM_BIT_STRING:
					SSL_LOG(TRACE, "creating asn node from json bit string: %p, size: %zu", value->content, value->size);
					asn1_item->content = ft_memdup((char *)value->content, value->size);
					asn1_item->size = value->size;
					break;
				case ASN_TAGNUM_SEQUENCE:
					SSL_LOG(TRACE, "creating asn node from json sequence: %p, size: %zu", value->content, value->size);
					asn1_item->content = value->content;
					asn1_item->size = ft_lst_size((t_node *)value->content);
					break;
				default:
					SSL_LOG(ERROR, "invalid json schema: unknown type: %s", (char *)type->content);
					goto label_error;
			}
		}
	}

	asn1_node = asn1_node_create(asn1_item);
	SSL_LOG(TRACE, "successfully created asn node: %p", asn1_node);

	return (asn1_node);

label_error:
	asn1_item_del(asn1_item);
	return (NULL);
}

static int 	__f_asn1_node_dumper(t_node *node, t_ostring *ostring);

char	*asn1_tree_dumps(t_node *asn1_tree)
{
	if (NULL == asn1_tree) {
		return (NULL);
	}
	return (json_dumps_with_f_dumper(asn1_tree, __f_asn1_node_dumper));
}

size_t	asn1_tree_dumpb(t_node *asn1_tree, char *buf, size_t size)
{
	return (json_dumpb_with_f_dumper(asn1_tree, buf, size, __f_asn1_node_dumper));
}

static int	__f_asn1_node_dumper(t_node *node, t_ostring *ostring)
{
	t_iasn *asn1_item = node->content;

	switch (node->type) {
	case JSON_TYPE_BYTES:
		if (asn1_item) {
			char *item_dumps = asn1_item_dumps(asn1_item);
			ft_ostr_append_cstr(ostring, item_dumps);
			SSL_FREE(item_dumps);
		} else {
			ft_ostr_append_cstr(ostring, "null");
		}
		return (SSL_OK);
	default:
		SSL_LOG(ERROR, "unknown asn1 node type: %#x", node->type);
		return (SSL_ERR);
	}
}

t_iasn *asn1_item_create(void)
{
	t_iasn *item;

	SSL_ALLOC(item, sizeof(t_iasn));
	ft_bzero(item, sizeof(t_iasn));
	return (item);
}

void	asn1_item_init(t_iasn *item)
{
	if (NULL == item) {
		return ;
	}

	ft_bzero(item, sizeof(t_iasn));
}

void asn1_item_clear(t_iasn *item)
{
	if (NULL == item) {
		return;
	}

	SSL_FREE(item->content);
	SSL_FREE(item->description);
	ft_bzero(item, sizeof(t_iasn));
}

void asn1_item_del(t_iasn *item)
{
	if (NULL == item) {
		return;
	}

	SSL_FREE(item->content);
	SSL_FREE(item->description);
	SSL_FREE(item);
}

t_iasn *asn1_item_dup(t_iasn *item)
{
	t_iasn *dup_item;

	if (NULL == item) {
		return (NULL);
	}

	dup_item = asn1_item_create();
	ft_memcpy(dup_item, item, sizeof(t_iasn));

	dup_item->content = ft_memdup(item->content, item->size);

	return (dup_item);
}

int	asn1_item_set_type(t_iasn *item, char *type)
{
	uint8_t	tagnum;

	if (NULL == type || NULL == item) {
		return (SSL_LOG(ERROR, INVALID_INPUT_ERROR));
	}

	if (ft_streq(type, ASN_TYPE_NAME_SEQUENCE)) {
		tagnum = ASN_TAGNUM_SEQUENCE;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_OSTRING)) {
		tagnum = ASN_TAGNUM_OCTET_STRING;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_BITSTRING)) {
		tagnum = ASN_TAGNUM_BIT_STRING;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_OBJECT_ID)) {
		tagnum = ASN_TAGNUM_OBJECT_ID;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_OBJECT_DESC)) {
		tagnum = ASN_TAGNUM_OBJECT_DESCR;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_INT)) {
		tagnum = ASN_TAGNUM_INT;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_NULL)) {
		tagnum = ASN_TAGNUM_NULL;
	}
	else if (ft_streq(type, ASN_TYPE_NAME_BOOL)) {
		tagnum = ASN_TAGNUM_BOOLEAN;
	}
	else {
		SSL_LOG(ERROR, "unknown asn type: %s", type);
		return (SSL_ERR);
	}
	item->tagnum = tagnum;

	return (SSL_OK);
}

char	*asn1_item_get_type_name(t_iasn *item)
{
	if (NULL == item) {
		return (NULL);
	}
	switch (item->tagnum) {
		case ASN_TAGNUM_SEQUENCE:
			return ASN_TYPE_NAME_SEQUENCE;
		case ASN_TAGNUM_OCTET_STRING:
			return ASN_TYPE_NAME_OSTRING;
		case ASN_TAGNUM_BIT_STRING:
			return ASN_TYPE_NAME_BITSTRING;
		case ASN_TAGNUM_OBJECT_ID:
			return ASN_TYPE_NAME_OBJECT_ID;
		case ASN_TAGNUM_OBJECT_DESCR:
			return ASN_TYPE_NAME_OBJECT_DESC;
		case ASN_TAGNUM_INT:
			return ASN_TYPE_NAME_INT;
		case ASN_TAGNUM_NULL:
			return ASN_TYPE_NAME_NULL;
		case ASN_TAGNUM_BOOLEAN:
			return ASN_TYPE_NAME_BOOL;
		default:
			return ASN_TYPE_NAME_UNKNOWN;
	}
}

char	*asn1_item_dumps(t_iasn *item)
{
	if (NULL == item) {
		return (NULL);
	}
	t_ostring ostring;
	ft_ostr_init_with_capacity(&ostring, 128);

	switch (item->tagnum) {
	case ASN_TAGNUM_SEQUENCE:
		ft_ostr_appendf(&ostring, "[");
		int	commas = 0;
		for (t_node *cur_node = item->content; cur_node; cur_node = cur_node->next) {
			if (commas++) {
				ft_ostr_append(&ostring, ",", 1);
			}
			t_ostring item_ostring;
			ft_ostr_init_with_capacity(&item_ostring, 128);
			if (SSL_OK != __f_asn1_node_dumper(cur_node, &item_ostring)) {
				ft_ostr_clear(&ostring);
				return (NULL);
			}
			char *node_dumps = ft_ostr_to_cstr(&item_ostring, 0, item_ostring.size);
			ft_ostr_clear(&item_ostring);
			ft_ostr_appendf(&ostring, "%s", node_dumps);
		}
		ft_ostr_append(&ostring, "]", 1);
		break;
	case ASN_TAGNUM_OCTET_STRING:
	case ASN_TAGNUM_BIT_STRING:
		ft_ostr_appendf(&ostring, "\"<ptr=%p,size=%zu>\"", item->content, item->size);
		break;
	case ASN_TAGNUM_OBJECT_ID:
	case ASN_TAGNUM_OBJECT_DESCR:
		if (item->content) {
			ft_ostr_append_cstr(&ostring, "\"");
			ft_ostr_append(&ostring, item->content, item->size);
			ft_ostr_append_cstr(&ostring, "\"");
		} else {
			ft_ostr_append_cstr(&ostring, "null");
		}
		break;
	case ASN_TAGNUM_INT:
		if (item->content) {
			char *hex = bnum_to_hex_u((const t_num *)(item->content));
			ft_ostr_appendf(&ostring, "\"%s\"", hex);
			SSL_FREE(hex);
		} else {
			ft_ostr_append_cstr(&ostring, "null");
		}
		break;
	case ASN_TAGNUM_NULL:
		ft_ostr_append_cstr(&ostring, "null");
		break;
	case ASN_TAGNUM_BOOLEAN:
		;;
		const char *bools = (*(uint8_t *)(item->content) == 1u) ? ft_strdup("true") : ft_strdup("false");
		ft_ostr_append_cstr(&ostring, bools);
		break;
	default:
		ft_ostr_append_cstr(&ostring, "\"unknown\"");
	}

	char *dumps = NULL;
	char *values = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	ft_sprintf(&dumps, "{\"description\":\"%s\",\"type\":\"%s\",\"tag\":\"%#x\",\"value\":%s}", item->description, asn1_item_get_type_name(item), item->tag, values);
	SSL_FREE(values);

	return (dumps);
}
