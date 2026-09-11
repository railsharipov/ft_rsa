#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <der.h>
#include <json.h>
#include <json_v2.h>
#include <io.h>
#include <libft.h>
#include <bnum.h>

static t_asn_v2_typedef	*__asn1_v2_typedef_create(void);
static void				__asn1_v2_typedef_delete(t_asn_v2_typedef *asn1_typedef);

t_asn_v2_value	*asn1_v2_value_create(void);
void			asn1_v2_value_delete(t_asn_v2_value *asn1_value);

t_asn_v2_type_kind	asn1_v2_get_type_by_name(const char *name);
const char			*asn1_v2_get_type_name(t_asn_v2_type_kind type);

# define ASN_V2_TAG_CLASS_NAME_UNIVERSAL   "universal"
# define ASN_V2_TAG_CLASS_NAME_APPLICATION "application"
# define ASN_V2_TAG_CLASS_NAME_CONTEXT     "context"
# define ASN_V2_TAG_CLASS_NAME_PRIVATE     "private"

# define ASN_V2_TAG_MODE_NAME_EXPLICIT     "explicit"
# define ASN_V2_TAG_MODE_NAME_IMPLICIT     "implicit"

# define ASN_V2_TYPE_NAME_BOOLEAN          "boolean"
# define ASN_V2_TYPE_NAME_INTEGER          "integer"
# define ASN_V2_TYPE_NAME_BIT_STRING       "bit_string"
# define ASN_V2_TYPE_NAME_OCTET_STRING     "octet_string"
# define ASN_V2_TYPE_NAME_IA5_STRING       "ia5_string"
# define ASN_V2_TYPE_NAME_UTF8_STRING      "utf8_string"
# define ASN_V2_TYPE_NAME_PRINTABLE_STRING "printable_string"
# define ASN_V2_TYPE_NAME_OBJECT_ID        "object_id"
# define ASN_V2_TYPE_NAME_OBJECT_DESCR     "object_descr"
# define ASN_V2_TYPE_NAME_SEQUENCE         "sequence"
# define ASN_V2_TYPE_NAME_SEQUENCE_OF      "sequence_of"
# define ASN_V2_TYPE_NAME_SET              "set"
# define ASN_V2_TYPE_NAME_SET_OF           "set_of"
# define ASN_V2_TYPE_NAME_CHOICE           "choice"
# define ASN_V2_TYPE_NAME_TAGGED           "tagged"
# define ASN_V2_TYPE_NAME_REF              "ref"
# define ASN_V2_TYPE_NAME_NULL             "null"

int __asn1_v2_schema_validate_tag_mode_name(const char *name)
{
	if (NULL == name) return (SSL_ERR);

	if (ft_streq(name, ASN_V2_TAG_MODE_NAME_EXPLICIT)) return (SSL_OK);
	if (ft_streq(name, ASN_V2_TAG_MODE_NAME_IMPLICIT)) return (SSL_OK);

	return (SSL_ERR);
}

int __asn1_v2_schema_validate_tag_class_name(const char *name)
{
	if (NULL == name) return (SSL_ERR);

	if (ft_streq(name, ASN_V2_TAG_CLASS_NAME_UNIVERSAL))   return (SSL_OK);
	if (ft_streq(name, ASN_V2_TAG_CLASS_NAME_APPLICATION)) return (SSL_OK);
	if (ft_streq(name, ASN_V2_TAG_CLASS_NAME_CONTEXT))     return (SSL_OK);
	if (ft_streq(name, ASN_V2_TAG_CLASS_NAME_PRIVATE))     return (SSL_OK);

	return (SSL_ERR);
}

int __asn1_v2_schema_validate_type_name(const char *name)
{
	if (NULL == name) return (SSL_ERR);

	if (ft_streq(name, ASN_V2_TYPE_NAME_INTEGER))          return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_BOOLEAN))          return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_BIT_STRING))       return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_OCTET_STRING))     return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_IA5_STRING))       return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_UTF8_STRING))      return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_PRINTABLE_STRING)) return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_NULL))             return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_OBJECT_ID))        return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_OBJECT_DESCR))     return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_SEQUENCE))         return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_SEQUENCE_OF))      return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_SET))              return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_SET_OF))           return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_CHOICE))           return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_TAGGED))           return (SSL_OK);
	if (ft_streq(name, ASN_V2_TYPE_NAME_REF))              return (SSL_OK);

	return (SSL_ERR);
}

int __asn1_v2_schema_validate_type_compatibility(t_asn_v2_type_kind asn1_type, t_json_v2_type json_type)
{
	if (json_type == JSON_V2_TYPE_NULL) return (SSL_OK);

	switch (asn1_type) {
	case ASN_V2_TYPE_KIND_INT:
		return (json_type == JSON_V2_TYPE_NUMBER) ? SSL_OK : SSL_ERR;
	case ASN_V2_TYPE_KIND_BOOLEAN:
		return (json_type == JSON_V2_TYPE_BOOL) ? SSL_OK : SSL_ERR;
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		return (json_type == JSON_V2_TYPE_STRING) ? SSL_OK : SSL_ERR;
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET:
	case ASN_V2_TYPE_KIND_SET_OF:
	case ASN_V2_TYPE_KIND_CHOICE:
		return (json_type == JSON_V2_TYPE_ARRAY) ? SSL_OK : SSL_ERR;
	case ASN_V2_TYPE_KIND_NULL:
		return (json_type == JSON_V2_TYPE_NULL) ? SSL_OK : SSL_ERR;
	case ASN_V2_TYPE_KIND_TAGGED:
	case ASN_V2_TYPE_KIND_REF:
	case ASN_V2_TYPE_KIND_UNKNOWN:
	default:
		return (SSL_ERR);
	}
}

# define __TYPE_KIND_KEY		".kind"
# define __TYPE_TAG_CLASS_KEY	".tagClass"
# define __TYPE_TAG_NUMBER_KEY	".tagNumber"
# define __TYPE_BASE_TYPE_KEY	".baseType"
# define __TYPE_TYPEDEF_KEY		".typedef"

int __asn1_v2_schema_validate_type(t_json_v2 *jtype)
{
	if (jtype->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid asn1 typedef: json object but got json %s", json_v2_get_type_name(jtype->type));
		return (SSL_ERR);
	}

	t_json_v2 *jkind = NULL;
	if (JSON_OK != json_v2_query(__TYPE_KIND_KEY, jtype, &jkind)) {
		SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` key", __TYPE_KIND_KEY);
		return (SSL_ERR);
	}
	else {
		if (jkind->type != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be a json string but got json %s", __TYPE_KIND_KEY, json_v2_get_type_name(jkind->type));
			return (SSL_ERR);
		}
		if (SSL_OK != __asn1_v2_schema_validate_type_name(jkind->as.cstr)) {
			SSL_LOG(ERROR, "invalid asn1 type: `%s`", jkind->as.cstr);
			return (SSL_ERR);
		}
	}
	t_asn_v2_type_kind asn1_type = asn1_v2_get_type_by_name(jkind->as.cstr);

	if (asn1_type == ASN_V2_TYPE_KIND_TAGGED) {
		t_json_v2 *jtag_class = NULL;
		if (JSON_OK != json_v2_query(__TYPE_TAG_CLASS_KEY, jtype, &jtag_class)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` key", __TYPE_TAG_CLASS_KEY);
			return (SSL_ERR);
		}
		else {
			if (jtag_class->type != JSON_V2_TYPE_STRING) {
				SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be a json string but got json %s", __TYPE_TAG_CLASS_KEY, json_v2_get_type_name(jtag_class->type));
				return (SSL_ERR);
			}
			if (SSL_OK != __asn1_v2_schema_validate_tag_class_name(jtag_class->as.cstr)) {
				SSL_LOG(ERROR, "invalid asn1 type: `%s`", jtag_class->as.cstr);
				return (SSL_ERR);
			}
		}

		t_json_v2 *jtag_number = NULL;
		if (JSON_OK != json_v2_query(__TYPE_TAG_NUMBER_KEY, jtype, &jtag_number)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` key", __TYPE_TAG_NUMBER_KEY);
			return (SSL_ERR);
		}
		else {
			if (jtag_number->type != JSON_V2_TYPE_NUMBER) {
				SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be a json number but got json %s", __TYPE_TAG_NUMBER_KEY, json_v2_get_type_name(jtag_number->type));
				return (SSL_ERR);
			}
		}

		t_json_v2 *jbase_type = NULL;
		if (JSON_OK != json_v2_query(__TYPE_BASE_TYPE_KEY, jtype, &jbase_type)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` key", __TYPE_BASE_TYPE_KEY);
			return (SSL_ERR);
		}
		else {
			if (SSL_OK != __asn1_v2_schema_validate_type(jbase_type)) {
				SSL_LOG(ERROR, "invalid asn1 typedef: invalid `%s`", __TYPE_BASE_TYPE_KEY);
				return (SSL_ERR);
			}
		}
	}

	if (asn1_type == ASN_V2_TYPE_KIND_REF) {
		t_json_v2 *jtypedef = NULL;
		if (JSON_OK != json_v2_query(__TYPE_TYPEDEF_KEY, jtype, &jtypedef)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be specified for asn1 %s type", __TYPE_TYPEDEF_KEY, ASN_V2_TYPE_NAME_REF);
			return (SSL_ERR);
		}
		else {
			if (jtypedef->type != JSON_V2_TYPE_OBJECT) {
				SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be a json string but got json %s", __TYPE_TYPEDEF_KEY, json_v2_get_type_name(jtypedef->type));
				return (SSL_ERR);
			}
		}
	}

	return (SSL_OK);
}

# define __TYPEDEF_TYPE_KEY 		".type"
# define __TYPEDEF_OPTIONAL_KEY		".optional"
# define __TYPEDEF_DEFAULT_KEY		".default"
# define __TYPEDEF_ELEMENT_TYPE_KEY	".elementType"
# define __TYPEDEF_ELEMENTS_KEY		".elements"

static int __asn1_v2_schema_validate_typedef(t_json_v2 *jtypedef)
{
	if (jtypedef->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid asn1 type definition: expected a json object but got json %s", json_v2_get_type_name(jtypedef->type));
		return (SSL_ERR);
	}

	t_asn_v2_type_kind asn1_type = ASN_V2_TYPE_KIND_UNKNOWN;
	t_json_v2 *jtype = NULL;
	if (JSON_OK != json_v2_query(__TYPEDEF_TYPE_KEY, jtypedef, &jtype)) {
		SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` key", __TYPEDEF_TYPE_KEY);
		return (SSL_ERR);
	}
	else {
		if (SSL_OK != __asn1_v2_schema_validate_type(jtype)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: invalid `%s`", __TYPEDEF_TYPE_KEY);
			return (SSL_ERR);
		}
		t_json_v2 *jkind = NULL;
		int ret = json_v2_query(__TYPE_KIND_KEY, jtype, &jkind);
		assert(JSON_OK == ret);
		asn1_type = asn1_v2_get_type_by_name(jkind->as.cstr);
	}

	t_json_v2 *joptional = NULL;
	if (JSON_OK != json_v2_query(__TYPEDEF_OPTIONAL_KEY, jtypedef, &joptional)) {
		SSL_LOG(TRACE, "asn1 typedef: no `%s` key specified", __TYPEDEF_OPTIONAL_KEY);
	}
	else {
		if (joptional->type != JSON_V2_TYPE_BOOL) {
			SSL_LOG(ERROR, "invalid asn1 type definition: expected `%s` to be a json boolean but got json %s", __TYPEDEF_OPTIONAL_KEY, json_v2_get_type_name(joptional->type));
			return (SSL_ERR);
		}
	}

	t_json_v2 *jdefault = NULL;
	if (JSON_OK != json_v2_query(__TYPEDEF_DEFAULT_KEY, jtypedef, &jdefault)) {
		SSL_LOG(TRACE, "asn1 typedef: no `%s` key specified", __TYPEDEF_DEFAULT_KEY);
	}
	else {
		SSL_LOG(TRACE, "asn1 typedef: `%s` key is set, value type: %s", __TYPEDEF_DEFAULT_KEY, json_v2_get_type_name(jdefault->type));
		if (SSL_OK != __asn1_v2_schema_validate_type_compatibility(asn1_type, jdefault->type)) {
			SSL_LOG(ERROR, "asn1 typedef: default value of json %s type is not compatible with asn1 %s type", json_v2_get_type_name(jdefault->type), asn1_v2_get_type_name(asn1_type));
			return (SSL_ERR);
		}
	}

	if (asn1_type == ASN_V2_TYPE_KIND_SEQUENCE_OF || asn1_type == ASN_V2_TYPE_KIND_SET_OF) {
		t_json_v2 *jelement_type = NULL;
		if (JSON_OK != json_v2_query(__TYPEDEF_ELEMENT_TYPE_KEY, jtypedef, &jelement_type)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: no `%s` key specified for asn1 %s type", __TYPEDEF_ELEMENT_TYPE_KEY, asn1_v2_get_type_name(asn1_type));
			return (SSL_ERR);
		}
	}
	else {
		SSL_LOG(TRACE, "asn1 typedef: ignoring `%s` key if specified", __TYPEDEF_ELEMENT_TYPE_KEY);
	}

	if (asn1_type == ASN_V2_TYPE_KIND_SEQUENCE || asn1_type == ASN_V2_TYPE_KIND_SEQUENCE_OF || asn1_type == ASN_V2_TYPE_KIND_SET || asn1_type == ASN_V2_TYPE_KIND_SET_OF) {
		t_json_v2 *jelements = NULL;
		if (JSON_OK != json_v2_query(__TYPEDEF_ELEMENTS_KEY, jtypedef, &jelements)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: no `%s` key specified for asn1 %s type", __TYPEDEF_ELEMENTS_KEY, asn1_v2_get_type_name(asn1_type));
			return (SSL_ERR);
		}
	}
	else {
		SSL_LOG(TRACE, "asn1 typedef: ignoring `%s` key if specified", __TYPEDEF_ELEMENTS_KEY);
	}

	return (SSL_OK);
}

# define __SCHEMA_TAG_MODE_KEY ".taggingMode"
# define __SCHEMA_TYPEDEFS_KEY ".typedefs"

int	asn1_v2_schema_validate(t_json_v2 *jschema)
{
	if (NULL == jschema) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "validating asn1 schema: %p", jschema);

	if (jschema->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid schema format: expected json object but got %s", json_v2_get_type_name(jschema->type));
		return (SSL_ERR);
	}

	t_json_v2 *jtagmode = NULL;
	if (JSON_OK != json_v2_query(__SCHEMA_TAG_MODE_KEY, jschema, &jtagmode)) {
		SSL_LOG(TRACE, "no asn1 tagging mode specified, using default");
	} else {
		if (jtagmode->type != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid json schema: expected `%s` to be a json string but got %s", __SCHEMA_TAG_MODE_KEY, json_v2_get_type_name(jtagmode->type));
			return (SSL_ERR);
		}
		if (SSL_OK != __asn1_v2_schema_validate_tag_mode_name(jtagmode->as.cstr)) {
			SSL_LOG(ERROR, "invalid `%s`: %s", __SCHEMA_TAG_MODE_KEY, jtagmode->as.cstr);
			return (SSL_ERR);
		}
	}

	t_json_v2 *jtypedefs = NULL;
	if (JSON_OK != json_v2_query(__SCHEMA_TYPEDEFS_KEY, jschema, &jtypedefs)) {
		SSL_LOG(ERROR, "invalid json schema: expected `%s` key", __SCHEMA_TYPEDEFS_KEY);
		return (SSL_ERR);
	}
	if (jtypedefs->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid json schema: expected `%s` to be a json object but got json %s", __SCHEMA_TYPEDEFS_KEY, json_v2_get_type_name(jtypedefs->type));
		return (SSL_ERR);
	}

	const char *key = NULL;
	void *value = NULL;
	t_htbl_v2_next next = {0};
	while (ft_htbl_v2_next(&jtypedefs->as.htable, &next, &key, &value)) {
		if (SSL_OK != __asn1_v2_schema_validate_typedef(value)) {
			SSL_LOG(ERROR, "failed to parse typedef: %s", key);
			return (SSL_ERR);
		}
	}

	return (SSL_OK);
}





// int	asn1_v2_schema_parse(t_asn_v2_module **asn1_schema, t_node *schema_node)
// {
// 	if (NULL == asn1_schema) {
// 		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
// 		return (SSL_ERR);
// 	}
// 	if (SSL_OK != asn1_v2_schema_validate(json_schema_node)) {
// 		SSL_LOG(ERROR, "cannot parse invalid json schema");
// 		return (SSL_ERR);
// 	}
// 	SSL_LOG(TRACE, "parsing asn1 schema json: %p", json_schema_node);

// 	t_node *json_tagmode_node = NULL;
// 	int ret = json_query(__SCHEMA_TAG_MODE_KEY, json_schema_node, &json_tagmode_node);
// 	assert(JSON_OK != ret);

// 	t_node *json_typedefs_node = NULL;
// 	ret = json_query(__SCHEMA_TAG_MODE_KEY, json_schema_node, &json_typedefs_node);
// 	assert(JSON_OK != ret);

// 	return (SSL_ERR);












// 	SSL_LOG(TRACE, "setting asn item description to '%s'", (char *)desc->content);
// 	asn1_item->description = ft_strdup((char *)desc->content);

// 	SSL_LOG(TRACE, "setting encode tag for asn item with tagnum: %#x", asn1_item->tagnum);
// 	switch (asn1_item->tagnum) {
// 	case ASN_TAGNUM_INT:
// 	case ASN_TAGNUM_NULL:
// 	case ASN_TAGNUM_OBJECT_ID:
// 	case ASN_TAGNUM_OBJECT_DESCR:
// 	case ASN_TAGNUM_BOOLEAN:
// 	case ASN_TAGNUM_OCTET_STRING:
// 	case ASN_TAGNUM_BIT_STRING:
// 		asn1_item->tag |= ASN_ENCODE_PRIMITIVE;
// 		break;
// 	case ASN_TAGNUM_SEQUENCE:
// 		asn1_item->tag |= ASN_ENCODE_CONSTRUCT;
// 		break;
// 	default:
// 		SSL_LOG(ERROR, "unknown asn1 tagnum: %#x", asn1_item->tagnum);
// 		goto label_error;
// 	}
// 	SSL_LOG(TRACE, "using asn1_item encode tag: %s", FLAG(ASN_ENCODE_CONSTRUCT, asn1_item->tag) ? "construct" : "primitive");

// 	if (value != NULL) {
// 		if (value->type == JSON_TYPE_ARRAY) {
// 			SSL_LOG(TRACE, "value type is json array, processing array nodes");

// 			for (t_node *child = value->content; child != NULL; child = child->next) {
// 				SSL_LOG(TRACE, "creating asn node from json array item: %p", child);
// 				t_node *asn1_node = asn1_node_create_from_schema(child);

// 				if (NULL == asn1_node) {
// 					SSL_LOG(ERROR, "failed to create asn node for asn sequence");
// 					goto label_error;
// 				}
// 				ft_lst_append((t_node **)&asn1_item->content, asn1_node);
// 			}
// 		} else if (value->type == JSON_TYPE_OBJECT) {
// 			SSL_LOG(TRACE, "value type is json object, processing object node");
// 			SSL_LOG(TRACE, "creating asn node from json object: %p", value);

// 			t_node *asn1_node = asn1_node_create_from_schema(value);
// 			asn1_item->content = asn1_node;
// 			asn1_item->size = 0;

// 		} else {
// 			SSL_LOG(TRACE, "value type is json %s", json_get_type_name(value->type));

// 			switch (asn1_item->tagnum) {
// 				case ASN_TAGNUM_INT:
// 					SSL_LOG(TRACE, "creating asn node from json number: %p, size: %zu", value->content, value->size);
// 					asn1_item->content = bnum_clone((t_num *)value->content);
// 					asn1_item->size = value->size;
// 					break;
// 				case ASN_TAGNUM_NULL:
// 					SSL_LOG(TRACE, "creating asn node from json null: %p, size: %zu", value->content, value->size);
// 					asn1_item->content = NULL;
// 					asn1_item->size = 0;
// 					break;
// 				case ASN_TAGNUM_OBJECT_ID:
// 					SSL_LOG(TRACE, "creating asn node from json object id: %p, size: %zu", value->content, value->size);
// 					asn1_item->content = ft_strdup((char *)value->content);
// 					asn1_item->size = value->size;
// 					break;
// 				case ASN_TAGNUM_OBJECT_DESCR:
// 					SSL_LOG(TRACE, "creating asn node from json object description: %p, size: %zu", value->content, value->size);
// 					asn1_item->content = ft_strdup((char *)value->content);
// 					asn1_item->size = value->size;
// 					break;
// 				case ASN_TAGNUM_BOOLEAN:
// 					SSL_LOG(TRACE, "creating asn node from json boolean: %p, size: %zu", value->content, value->size);
// 					SSL_ALLOC(asn1_item->content, sizeof(uint8_t));
// 					*(uint8_t *)asn1_item->content = (value->type == JSON_TYPE_BOOL_TRUE) ? (uint8_t)1u : (uint8_t)0u;
// 					asn1_item->size = 0;
// 					break;
// 				case ASN_TAGNUM_OCTET_STRING:
// 					SSL_LOG(TRACE, "creating asn node from json octet string: %p, size: %zu", value->content, value->size);
// 					asn1_item->content = ft_memdup((char *)value->content, value->size);
// 					asn1_item->size = value->size;
// 					break;
// 				case ASN_TAGNUM_BIT_STRING:
// 					SSL_LOG(TRACE, "creating asn node from json bit string: %p, size: %zu", value->content, value->size);
// 					asn1_item->content = ft_memdup((char *)value->content, value->size);
// 					asn1_item->size = value->size;
// 					break;
// 				case ASN_TAGNUM_SEQUENCE:
// 					SSL_LOG(TRACE, "creating asn node from json sequence: %p, size: %zu", value->content, value->size);
// 					asn1_item->content = value->content;
// 					asn1_item->size = ft_lst_size((t_node *)value->content);
// 					break;
// 				default:
// 					SSL_LOG(ERROR, "invalid json schema: unknown type: %s", (char *)type->content);
// 					goto label_error;
// 			}
// 		}
// 	}
// 	return (asn1_node_create(asn1_item));

// label_error:
// 	asn1_item_del(asn1_item);
// 	return (NULL);
// }

int	__asn1_v2_schema_parse_typedef(t_asn_v2_typedef **asn1_typedef, t_node *json_typedef_node)
{
	{
		t_asn_v2_type_kind		kind;
		struct s_asn_v2_typedef	*base_typedef;
		t_asn_v2_tag			tag;
		t_asn_v2_value			*default_value;
		uint8_t 				optional;
		struct s_asn_v2_typedef	*element_typedef;
		size_t					element_count;
		void 					*elements;
	}

	return (SSL_OK);

label_error:
	__asn1_v2_typedef_delete(asn1_typedef);
	return (SSL_ERR);
}

void __asn1_v2_typedef_node_del_content(void *content)
{
	if (NULL != content) {
		__asn1_v2_typedef_delete((t_asn_v2_typedef *)content);
	}
}

t_asn_v2_typedef *__asn1_v2_typedef_create(void)
{
	t_asn_v2_typedef *asn1_typedef = NULL;
	SSL_ALLOC(asn1_typedef, sizeof(t_asn_v2_typedef));
	*asn1_typedef = (t_asn_v2_typedef){0};
	return (asn1_typedef);
}

void __asn1_v2_typedef_delete(t_asn_v2_typedef *asn1_typedef)
{
	if (NULL == asn1_typedef) {
		return;
	}
	if (NULL != asn1_typedef->base_typedef) {
		__asn1_v2_typedef_delete(asn1_typedef->base_typedef);
		asn1_typedef->base_typedef = NULL;
	}
	if (NULL != asn1_typedef->element_typedef) {
		__asn1_v2_typedef_delete(asn1_typedef->element_typedef);
		asn1_typedef->element_typedef = NULL;
	}
	if (NULL != asn1_typedef->default_value) {
		asn1_v2_value_delete(asn1_typedef->default_value);
		asn1_typedef->default_value = NULL;
	}
	if (NULL != asn1_typedef->elements) {
		ft_lst_del((t_node *)asn1_typedef->elements);
		asn1_typedef->elements = NULL;
	}
	SSL_FREE(asn1_typedef);
}

t_asn_v2_value *asn1_v2_value_create(void)
{
	t_asn_v2_value *asn1_value = NULL;
	SSL_ALLOC(asn1_value, sizeof(t_asn_v2_value));
	*asn1_value = (t_asn_v2_value){0};
	return (asn1_value);
}

const char *asn1_v2_get_type_name(t_asn_v2_type_kind type)
{
	switch (type) {
	case ASN_V2_TYPE_KIND_INT:              return ASN_V2_TYPE_NAME_INTEGER;
	case ASN_V2_TYPE_KIND_BOOLEAN:          return ASN_V2_TYPE_NAME_BOOLEAN;
	case ASN_V2_TYPE_KIND_BIT_STRING:       return ASN_V2_TYPE_NAME_BIT_STRING;
	case ASN_V2_TYPE_KIND_OCTET_STRING:     return ASN_V2_TYPE_NAME_OCTET_STRING;
	case ASN_V2_TYPE_KIND_IA5_STRING:       return ASN_V2_TYPE_NAME_IA5_STRING;
	case ASN_V2_TYPE_KIND_UTF8_STRING:      return ASN_V2_TYPE_NAME_UTF8_STRING;
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING: return ASN_V2_TYPE_NAME_PRINTABLE_STRING;
	case ASN_V2_TYPE_KIND_NULL:             return ASN_V2_TYPE_NAME_NULL;
	case ASN_V2_TYPE_KIND_OBJECT_ID:        return ASN_V2_TYPE_NAME_OBJECT_ID;
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:     return ASN_V2_TYPE_NAME_OBJECT_DESCR;
	case ASN_V2_TYPE_KIND_SEQUENCE:         return ASN_V2_TYPE_NAME_SEQUENCE;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:      return ASN_V2_TYPE_NAME_SEQUENCE_OF;
	case ASN_V2_TYPE_KIND_SET:              return ASN_V2_TYPE_NAME_SET;
	case ASN_V2_TYPE_KIND_SET_OF:           return ASN_V2_TYPE_NAME_SET_OF;
	case ASN_V2_TYPE_KIND_CHOICE:           return ASN_V2_TYPE_NAME_CHOICE;
	case ASN_V2_TYPE_KIND_TAGGED:           return ASN_V2_TYPE_NAME_TAGGED;
	case ASN_V2_TYPE_KIND_REF:              return ASN_V2_TYPE_NAME_REF;
	default:                                return "unknown";
	}
}

t_asn_v2_type_kind asn1_v2_get_type_by_name(const char *name)
{
	assert(NULL != name);

	if (ft_streq(name, ASN_V2_TYPE_NAME_INTEGER))              return ASN_V2_TYPE_KIND_INT;
	if (ft_streq(name, ASN_V2_TYPE_NAME_BOOLEAN))          return ASN_V2_TYPE_KIND_BOOLEAN;
	if (ft_streq(name, ASN_V2_TYPE_NAME_BIT_STRING))       return ASN_V2_TYPE_KIND_BIT_STRING;
	if (ft_streq(name, ASN_V2_TYPE_NAME_OCTET_STRING))     return ASN_V2_TYPE_KIND_OCTET_STRING;
	if (ft_streq(name, ASN_V2_TYPE_NAME_IA5_STRING))       return ASN_V2_TYPE_KIND_IA5_STRING;
	if (ft_streq(name, ASN_V2_TYPE_NAME_UTF8_STRING))      return ASN_V2_TYPE_KIND_UTF8_STRING;
	if (ft_streq(name, ASN_V2_TYPE_NAME_PRINTABLE_STRING)) return ASN_V2_TYPE_KIND_PRINTABLE_STRING;
	if (ft_streq(name, ASN_V2_TYPE_NAME_NULL))             return ASN_V2_TYPE_KIND_NULL;
	if (ft_streq(name, ASN_V2_TYPE_NAME_OBJECT_ID))        return ASN_V2_TYPE_KIND_OBJECT_ID;
	if (ft_streq(name, ASN_V2_TYPE_NAME_OBJECT_DESCR))     return ASN_V2_TYPE_KIND_OBJECT_DESCR;
	if (ft_streq(name, ASN_V2_TYPE_NAME_SEQUENCE))         return ASN_V2_TYPE_KIND_SEQUENCE;
	if (ft_streq(name, ASN_V2_TYPE_NAME_SEQUENCE_OF))      return ASN_V2_TYPE_KIND_SEQUENCE_OF;
	if (ft_streq(name, ASN_V2_TYPE_NAME_SET))              return ASN_V2_TYPE_KIND_SET;
	if (ft_streq(name, ASN_V2_TYPE_NAME_SET_OF))           return ASN_V2_TYPE_KIND_SET_OF;
	if (ft_streq(name, ASN_V2_TYPE_NAME_CHOICE))           return ASN_V2_TYPE_KIND_CHOICE;
	if (ft_streq(name, ASN_V2_TYPE_NAME_TAGGED))           return ASN_V2_TYPE_KIND_TAGGED;
	if (ft_streq(name, ASN_V2_TYPE_NAME_REF))              return ASN_V2_TYPE_KIND_REF;

	return ASN_V2_TYPE_KIND_UNKNOWN;
}

void asn1_v2_value_delete(t_asn_v2_value *asn1_value)
{
	if (NULL == asn1_value) {
		return;
	}
	switch (asn1_value->kind) {
	case ASN_V2_TYPE_KIND_INT:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET:
	case ASN_V2_TYPE_KIND_SET_OF:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_TAGGED:
	case ASN_V2_TYPE_KIND_REF:
	case ASN_V2_TYPE_KIND_UNKNOWN:
		TODO("__asn1_v2_value_delete");
	}
}
