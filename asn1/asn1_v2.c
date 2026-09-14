#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <der.h>
#include <json.h>
#include <json_v2.h>
#include <io.h>
#include <libft.h>
#include <bnum.h>

/****************************************************************************/

#define ASN_V2_TAG_CLASS_NAME_UNIVERSAL   "universal"
#define ASN_V2_TAG_CLASS_NAME_APPLICATION "application"
#define ASN_V2_TAG_CLASS_NAME_CONTEXT     "context"
#define ASN_V2_TAG_CLASS_NAME_PRIVATE     "private"

#define ASN_V2_TAG_MODE_NAME_EXPLICIT     "explicit"
#define ASN_V2_TAG_MODE_NAME_IMPLICIT     "implicit"

#define ASN_V2_TYPE_NAME_BOOLEAN          "boolean"
#define ASN_V2_TYPE_NAME_INTEGER          "integer"
#define ASN_V2_TYPE_NAME_BIT_STRING       "bit-string"
#define ASN_V2_TYPE_NAME_OCTET_STRING     "octet-string"
#define ASN_V2_TYPE_NAME_IA5_STRING       "ia5-string"
#define ASN_V2_TYPE_NAME_UTF8_STRING      "utf8-string"
#define ASN_V2_TYPE_NAME_PRINTABLE_STRING "printable-string"
#define ASN_V2_TYPE_NAME_OBJECT_ID        "object-id"
#define ASN_V2_TYPE_NAME_OBJECT_DESCR     "object-descr"
#define ASN_V2_TYPE_NAME_SEQUENCE         "sequence"
#define ASN_V2_TYPE_NAME_SEQUENCE_OF      "sequence-of"
#define ASN_V2_TYPE_NAME_SET              "set"
#define ASN_V2_TYPE_NAME_SET_OF           "set-of"
#define ASN_V2_TYPE_NAME_CHOICE           "choice"
#define ASN_V2_TYPE_NAME_TAGGED           "tagged"
#define ASN_V2_TYPE_NAME_REF              "ref"
#define ASN_V2_TYPE_NAME_NULL             "null"

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

const char *asn1_v2_get_tag_class_name(t_asn_v2_tag_class tag_class)
{
	switch (tag_class) {
	case ASN_V2_TAG_CLASS_UNIVERSAL:	return ASN_V2_TAG_CLASS_NAME_UNIVERSAL;
	case ASN_V2_TAG_CLASS_APPLICATION:	return ASN_V2_TAG_CLASS_NAME_APPLICATION;
	case ASN_V2_TAG_CLASS_CONTEXT:		return ASN_V2_TAG_CLASS_NAME_CONTEXT;
	case ASN_V2_TAG_CLASS_PRIVATE:		return ASN_V2_TAG_CLASS_NAME_PRIVATE;
	default:							return "unknown";
	}
}

const char *asn1_v2_get_tag_mode_name(t_asn_v2_tag_mode tag_mode)
{
	switch (tag_mode) {
	case ASN_V2_TAG_MODE_EXPLICIT:		return ASN_V2_TAG_MODE_NAME_EXPLICIT;
	case ASN_V2_TAG_MODE_IMPLICIT:		return ASN_V2_TAG_MODE_NAME_IMPLICIT;
	default:							return "unknown";
	}
}

/****************************************************************************/

#define __ASN_V2_TYPE_KIND_UNKNOWN	-1
#define __ASN_V2_TAG_CLASS_UNKNOWN	-1
#define __ASN_V2_TAG_MODE_UNKNOWN	-1

static t_asn_v2_type_kind __asn1_v2_get_type_by_name(const char *name)
{
	assert(NULL != name);

	if (ft_streq(name, ASN_V2_TYPE_NAME_INTEGER))			return ASN_V2_TYPE_KIND_INT;
	if (ft_streq(name, ASN_V2_TYPE_NAME_BOOLEAN))			return ASN_V2_TYPE_KIND_BOOLEAN;
	if (ft_streq(name, ASN_V2_TYPE_NAME_BIT_STRING))		return ASN_V2_TYPE_KIND_BIT_STRING;
	if (ft_streq(name, ASN_V2_TYPE_NAME_OCTET_STRING))		return ASN_V2_TYPE_KIND_OCTET_STRING;
	if (ft_streq(name, ASN_V2_TYPE_NAME_IA5_STRING))		return ASN_V2_TYPE_KIND_IA5_STRING;
	if (ft_streq(name, ASN_V2_TYPE_NAME_UTF8_STRING))		return ASN_V2_TYPE_KIND_UTF8_STRING;
	if (ft_streq(name, ASN_V2_TYPE_NAME_PRINTABLE_STRING))	return ASN_V2_TYPE_KIND_PRINTABLE_STRING;
	if (ft_streq(name, ASN_V2_TYPE_NAME_NULL))				return ASN_V2_TYPE_KIND_NULL;
	if (ft_streq(name, ASN_V2_TYPE_NAME_OBJECT_ID))			return ASN_V2_TYPE_KIND_OBJECT_ID;
	if (ft_streq(name, ASN_V2_TYPE_NAME_OBJECT_DESCR))		return ASN_V2_TYPE_KIND_OBJECT_DESCR;
	if (ft_streq(name, ASN_V2_TYPE_NAME_SEQUENCE))			return ASN_V2_TYPE_KIND_SEQUENCE;
	if (ft_streq(name, ASN_V2_TYPE_NAME_SEQUENCE_OF))		return ASN_V2_TYPE_KIND_SEQUENCE_OF;
	if (ft_streq(name, ASN_V2_TYPE_NAME_SET))				return ASN_V2_TYPE_KIND_SET;
	if (ft_streq(name, ASN_V2_TYPE_NAME_SET_OF))			return ASN_V2_TYPE_KIND_SET_OF;
	if (ft_streq(name, ASN_V2_TYPE_NAME_CHOICE))			return ASN_V2_TYPE_KIND_CHOICE;
	if (ft_streq(name, ASN_V2_TYPE_NAME_TAGGED))			return ASN_V2_TYPE_KIND_TAGGED;
	if (ft_streq(name, ASN_V2_TYPE_NAME_REF))				return ASN_V2_TYPE_KIND_REF;

	return __ASN_V2_TYPE_KIND_UNKNOWN;
}

static t_asn_v2_tag_mode __asn1_v2_get_tag_mode_by_name(const char *name)
{
	assert(NULL != name);

	if (ft_streq(name, ASN_V2_TAG_MODE_NAME_EXPLICIT))		return (ASN_V2_TAG_MODE_EXPLICIT);
	if (ft_streq(name, ASN_V2_TAG_MODE_NAME_IMPLICIT))		return (ASN_V2_TAG_MODE_IMPLICIT);

	return (__ASN_V2_TAG_MODE_UNKNOWN);
}

static t_asn_v2_tag_class __asn1_v2_get_tag_class_by_name(const char *name)
{
	assert(NULL != name);

	if (ft_streq(name, ASN_V2_TAG_CLASS_NAME_UNIVERSAL))	return (ASN_V2_TAG_CLASS_UNIVERSAL);
	if (ft_streq(name, ASN_V2_TAG_CLASS_NAME_APPLICATION))	return (ASN_V2_TAG_CLASS_APPLICATION);
	if (ft_streq(name, ASN_V2_TAG_CLASS_NAME_CONTEXT))		return (ASN_V2_TAG_CLASS_CONTEXT);
	if (ft_streq(name, ASN_V2_TAG_CLASS_NAME_PRIVATE))		return (ASN_V2_TAG_CLASS_PRIVATE);

	return (__ASN_V2_TAG_CLASS_UNKNOWN);
}

int __asn1_v2_schema_validate_tag_mode_name(const char *name)
{
	if (NULL == name) return (SSL_ERR);
	if (__ASN_V2_TAG_MODE_UNKNOWN == __asn1_v2_get_tag_mode_by_name(name)) return (SSL_ERR);
	return (SSL_OK);
}

int __asn1_v2_schema_validate_tag_class_name(const char *name)
{
	if (NULL == name) return (SSL_ERR);
	if (__ASN_V2_TAG_CLASS_UNKNOWN == __asn1_v2_get_tag_class_by_name(name)) return (SSL_ERR);
	return (SSL_OK);
}

int __asn1_v2_schema_validate_type_name(const char *name)
{
	if (NULL == name) return (SSL_ERR);
	if (__ASN_V2_TYPE_KIND_UNKNOWN == __asn1_v2_get_type_by_name(name)) return (SSL_ERR);
	return (SSL_OK);
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
	default:
		return (SSL_ERR);
	}
}

static int __asn1_v2_schema_validate_type(t_json_v2 *jtype);
static int __asn1_v2_schema_validate_typedef(t_json_v2 *jtypedef);

#define __TYPE_KIND_KEY			".kind"
#define __TYPE_TAG_CLASS_KEY	".tagClass"
#define __TYPE_TAG_NUMBER_KEY	".tagNumber"
#define __TYPE_BASE_TYPE_KEY	".baseType"
#define __TYPE_REF_NAME_KEY		".refName"

static int __asn1_v2_schema_validate_type(t_json_v2 *jtype)
{
	if (jtype->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid asn1 typedef: expected json object but got json %s", json_v2_get_type_name(jtype->type));
		return (SSL_ERR);
	}

	t_json_v2 *jkind = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__TYPE_KIND_KEY, jtype, &jkind)) {
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
	t_asn_v2_type_kind asn1_type = __asn1_v2_get_type_by_name(jkind->as.cstr);

	if (asn1_type == ASN_V2_TYPE_KIND_TAGGED) {
		t_json_v2 *jtag_class = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__TYPE_TAG_CLASS_KEY, jtype, &jtag_class)) {
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
		if (JSON_V2_OK != json_v2_query_nonnull(__TYPE_TAG_NUMBER_KEY, jtype, &jtag_number)) {
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
		if (JSON_V2_OK != json_v2_query_nonnull(__TYPE_BASE_TYPE_KEY, jtype, &jbase_type)) {
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
		t_json_v2 *jref_name = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__TYPE_REF_NAME_KEY, jtype, &jref_name)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be specified for asn1 %s type", __TYPE_REF_NAME_KEY, ASN_V2_TYPE_NAME_REF);
			return (SSL_ERR);
		}
		else {
			if (jref_name->type != JSON_V2_TYPE_STRING) {
				SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be a json string but got json %s", __TYPE_KIND_KEY, json_v2_get_type_name(jref_name->type));
				return (SSL_ERR);
			}
		}
	}

	return (SSL_OK);
}

#define __TYPEDEF_ID_KEY 			".id"
#define __TYPEDEF_TYPE_KEY 			".type"
#define __TYPEDEF_OPTIONAL_KEY		".optional"
#define __TYPEDEF_DEFAULT_KEY		".default"
#define __TYPEDEF_ELEMENT_TYPE_KEY	".elementType"
#define __TYPEDEF_ELEMENTS_KEY		".elements"

static int __asn1_v2_schema_validate_typedef(t_json_v2 *jtypedef)
{
	if (jtypedef->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid asn1 typedef: expected a json object but got json %s", json_v2_get_type_name(jtypedef->type));
		return (SSL_ERR);
	}

	t_json_v2 *jid = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__TYPEDEF_ID_KEY, jtypedef, &jid)) {
		SSL_LOG(TRACE, "no asn1 `%s` key specified ", __TYPEDEF_ID_KEY);
	}
	else {
		if (jid->type != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be a json string but got json %s", __TYPEDEF_ID_KEY, json_v2_get_type_name(jid->type));
			return (SSL_ERR);
		}
	}

	t_json_v2 *jtype = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__TYPEDEF_TYPE_KEY, jtypedef, &jtype)) {
		SSL_LOG(TRACE, "no asn1 `%s` key specified ", __TYPEDEF_TYPE_KEY);
	}
	else {
		if (SSL_OK != __asn1_v2_schema_validate_type(jtype)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: invalid `%s`", __TYPEDEF_TYPE_KEY);
			return (SSL_ERR);
		}
	}

	t_json_v2 *joptional = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__TYPEDEF_OPTIONAL_KEY, jtypedef, &joptional)) {
		SSL_LOG(TRACE, "asn1 typedef: no `%s` key specified", __TYPEDEF_OPTIONAL_KEY);
	}
	else {
		if (joptional->type != JSON_V2_TYPE_BOOL) {
			SSL_LOG(ERROR, "invalid asn1 type definition: expected `%s` to be a json boolean but got json %s", __TYPEDEF_OPTIONAL_KEY, json_v2_get_type_name(joptional->type));
			return (SSL_ERR);
		}
	}

	t_json_v2 *jdefault = NULL;
	if (JSON_V2_OK == json_v2_query(__TYPEDEF_DEFAULT_KEY, jtypedef, &jdefault)) {
		SSL_LOG(TRACE, "asn1 typedef: `%s` key is set, value type: %s", __TYPEDEF_DEFAULT_KEY, json_v2_get_type_name(jdefault->type));
	}

	t_json_v2 *jelement_type = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__TYPEDEF_ELEMENT_TYPE_KEY, jtypedef, &jelement_type)) {
		SSL_LOG(TRACE, "asn1 typedef: no `%s` key specified", __TYPEDEF_ELEMENT_TYPE_KEY);
	}
	else {
		if (SSL_OK != __asn1_v2_schema_validate_type(jelement_type)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: invalid `%s`", __TYPEDEF_ELEMENT_TYPE_KEY);
			return (SSL_ERR);
		}
	}

	t_json_v2 *jelements = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__TYPEDEF_ELEMENTS_KEY, jtypedef, &jelements)) {
		SSL_LOG(TRACE, "asn1 typedef: no `%s` key specified", __TYPEDEF_ELEMENT_TYPE_KEY);
	}
	else {
		void *content = NULL;
		t_list_next next = {0};
		while (ft_list_next_content(&jelements->as.list, &next, &content)) {
			if (SSL_OK != __asn1_v2_schema_validate_typedef(content)) {
				SSL_LOG(ERROR, "invalid asn1 typedef: `%s` contains invalid typedef", __TYPEDEF_ELEMENTS_KEY);
				return (SSL_ERR);
			}
		}
	}
	return (SSL_OK);
}

#define __SCHEMA_TAG_MODE_KEY ".taggingMode"
#define __SCHEMA_TYPEDEFS_KEY ".typedefs"

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
	if (JSON_V2_OK != json_v2_query_nonnull(__SCHEMA_TAG_MODE_KEY, jschema, &jtagmode)) {
		SSL_LOG(TRACE, "no asn1 tagging mode specified, using default");
	}
	else {
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
	if (JSON_V2_OK != json_v2_query_nonnull(__SCHEMA_TYPEDEFS_KEY, jschema, &jtypedefs)) {
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
			SSL_LOG(ERROR, "invalid asn1 typedef: %s", key);
			return (SSL_ERR);
		}
	}
	return (SSL_OK);
}

/****************************************************************************/

static t_asn_v2_module *__asn1_v2_module_create(void);
static void __asn1_v2_module_delete(t_asn_v2_module *asn1_module);

static t_asn_v2_typedef *__asn1_v2_typedef_create(void);
static void __asn1_v2_typedef_delete(t_asn_v2_typedef *asn1_typedef);

static t_asn_v2_value *__asn1_v2_value_create(void);
static void __asn1_v2_value_delete(t_asn_v2_value *asn1_value);

static t_asn_v2_type *__asn1_v2_type_create(void);
static void __asn1_v2_type_delete(t_asn_v2_type *asn1_type);

static inline void __asn1_v2_typedef_delete_adapter(void *p) {__asn1_v2_typedef_delete((t_asn_v2_typedef *)p);}
static inline void __asn1_v2_type_delete_adapter(void *p)	 {__asn1_v2_type_delete((t_asn_v2_type *)p);}
static inline void __asn1_v2_value_delete_adapter(void *p)	 {__asn1_v2_value_delete((t_asn_v2_value *)p);}
static inline void __asn1_v2_module_delete_adapter(void *p)	 {__asn1_v2_module_delete((t_asn_v2_module *)p);}

static int	__asn1_v2_schema_parse_typedef(t_asn_v2_typedef **asn1_typedef, t_json_v2 *jtypedef);
static int	__asn1_v2_schema_parse_type(t_asn_v2_type **asn1_type, t_json_v2 *jtype);
static int	__asn1_v2_schema_parse_value(t_asn_v2_value **asn1_value, t_json_v2 *jvalue);

static t_asn_v2_type *__asn1_v2_type_create(void)
{
	t_asn_v2_type *asn1_type = NULL;
	SSL_ALLOC(asn1_type, sizeof(t_asn_v2_type));
	*asn1_type = (t_asn_v2_type){0};
	return (asn1_type);
}

static void __asn1_v2_type_delete(t_asn_v2_type *asn1_type)
{
	if (NULL == asn1_type) return;

	if (NULL != asn1_type->base_type) __asn1_v2_type_delete(asn1_type->base_type);
	if (NULL != asn1_type->ref_name) SSL_FREE(asn1_type->ref_name);

	SSL_FREE(asn1_type);
}

static t_asn_v2_typedef *__asn1_v2_typedef_create(void)
{
	t_asn_v2_typedef *asn1_typedef = NULL;
	SSL_ALLOC(asn1_typedef, sizeof(t_asn_v2_typedef));
	*asn1_typedef = (t_asn_v2_typedef){0};
	return (asn1_typedef);
}

static void __asn1_v2_typedef_delete(t_asn_v2_typedef *asn1_typedef)
{
	if (NULL == asn1_typedef) return;

	if (NULL != asn1_typedef->id) SSL_FREE(asn1_typedef->id);
	if (NULL != asn1_typedef->type) __asn1_v2_type_delete(asn1_typedef->type);
	if (NULL != asn1_typedef->default_value) __asn1_v2_value_delete(asn1_typedef->default_value);
	if (NULL != asn1_typedef->element_type) __asn1_v2_type_delete(asn1_typedef->element_type);
	ft_list_clear_all_content(&asn1_typedef->elements, __asn1_v2_typedef_delete_adapter);

	SSL_FREE(asn1_typedef);
}

static t_asn_v2_module *__asn1_v2_module_create(void)
{
	t_asn_v2_module *asn1_module = NULL;
	SSL_ALLOC(asn1_module, sizeof(t_asn_v2_module));
	*asn1_module = (t_asn_v2_module){0};
	ft_htbl_v2_init(&asn1_module->typedefs, 256);
	return (asn1_module);
}

static void __asn1_v2_module_delete(t_asn_v2_module *asn1_module)
{
	if (NULL == asn1_module) return;

	ft_htbl_v2_clear(&asn1_module->typedefs, __asn1_v2_typedef_delete_adapter);
	SSL_FREE(asn1_module);
}

static t_asn_v2_value *__asn1_v2_value_create(void)
{
	t_asn_v2_value *asn1_value = NULL;
	SSL_ALLOC(asn1_value, sizeof(t_asn_v2_value));
	*asn1_value = (t_asn_v2_value){0};
	return (asn1_value);
}

static void __asn1_v2_value_delete(t_asn_v2_value *asn1_value)
{
	if (NULL == asn1_value) return;

	switch (asn1_value->type) {
	case ASN_V2_VALUE_TYPE_NULL:
	case ASN_V2_VALUE_TYPE_BOOLEAN:
		break;
	case ASN_V2_VALUE_TYPE_NUMBER:
		bnum_clear(&asn1_value->as.number);
		break;
	case ASN_V2_VALUE_TYPE_CSTRING:
		SSL_FREE(asn1_value->as.cstr);
		break;
	case ASN_V2_VALUE_TYPE_OSTRING:
		ft_ostr_clear(&asn1_value->as.ostring);
		break;
	case ASN_V2_VALUE_TYPE_LIST:
		ft_list_clear_all_content(&asn1_value->as.list, __asn1_v2_value_delete_adapter);
		break;
	default:
		UNREACHABLE("__asn1_v2_value_delete");
	}
	SSL_FREE(asn1_value);
}

static int	__asn1_v2_schema_parse_value(t_asn_v2_value **asn1_value, t_json_v2 *jvalue)
{
	*asn1_value = NULL;

	t_asn_v2_value *avalue = __asn1_v2_value_create();
	char cbuf[1024] = {0};

	switch (jvalue->type) {
	case JSON_V2_TYPE_NULL:
		avalue->type = ASN_V2_VALUE_TYPE_NULL;
		break;
	case JSON_V2_TYPE_ARRAY:
		avalue->type = ASN_V2_VALUE_TYPE_LIST;
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&jvalue->as.list, &next, &content)) {
			t_asn_v2_value *elem_avalue = NULL;
			if (SSL_OK != __asn1_v2_schema_parse_value(&elem_avalue, content)) {
				SSL_LOG(ERROR, "failed to parse element value: %s", json_v2_dumpb(content, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			ft_list_append_content(&avalue->as.list, elem_avalue);
		}
		break;
	case JSON_V2_TYPE_STRING:
		avalue->type = ASN_V2_VALUE_TYPE_CSTRING;
		avalue->as.cstr = ft_strdup(jvalue->as.cstr);
		break;
	case JSON_V2_TYPE_NUMBER:
		avalue->type = ASN_V2_VALUE_TYPE_NUMBER;
		bnum_copy(&jvalue->as.number, &avalue->as.number);
		break;
	case JSON_V2_TYPE_BOOL:
		avalue->type = ASN_V2_VALUE_TYPE_BOOLEAN;
		avalue->as.boolean = jvalue->as.boolean;
		break;
	case JSON_V2_TYPE_OBJECT:
	default:
		UNREACHABLE("__asn1_v2_schema_parse_value");
	}
	*asn1_value = avalue;
	return (SSL_OK);

label_error:
	__asn1_v2_value_delete(avalue);
	return (SSL_ERR);
}

static int	__asn1_v2_schema_parse_type(t_asn_v2_type **asn1_type, t_json_v2 *jtype)
{
	assert(jtype->type == JSON_V2_TYPE_OBJECT);
	*asn1_type = NULL;

	t_asn_v2_type *atype = __asn1_v2_type_create();
	char cbuf[1024] = {0};

	t_json_v2 *jkind = NULL;
	json_v2_query_nonnull(__TYPE_KIND_KEY, jtype, &jkind);
	assert(NULL != jkind && jkind->type == JSON_V2_TYPE_STRING);
	SSL_LOG(TRACE, "asn1 type: `%s` is %s", __TYPEDEF_ID_KEY, json_v2_dumpb(jkind, cbuf, sizeof(cbuf)));
	atype->kind = __asn1_v2_get_type_by_name(jkind->as.cstr);

	if (atype->kind == ASN_V2_TYPE_KIND_TAGGED) {
		t_json_v2 *jtag_class = NULL;
		json_v2_query_nonnull(__TYPE_TAG_CLASS_KEY, jtype, &jtag_class);
		assert(NULL != jtag_class && jtag_class->type == JSON_V2_TYPE_STRING);
		SSL_LOG(TRACE, "asn1 type: `%s` is %s", __TYPE_TAG_CLASS_KEY, json_v2_dumpb(jtag_class, cbuf, sizeof(cbuf)));
		atype->tag.class = __asn1_v2_get_tag_class_by_name(jtag_class->as.cstr);

		t_json_v2 *jtag_number = NULL;
		json_v2_query_nonnull(__TYPE_TAG_NUMBER_KEY, jtype, &jtag_number);
		assert(NULL != jtag_number && jtag_number->type == JSON_V2_TYPE_NUMBER);
		SSL_LOG(TRACE, "asn1 type: `%s` is %s", __TYPE_TAG_NUMBER_KEY, json_v2_dumpb(jtag_number, cbuf, sizeof(cbuf)));
		atype->tag.number = (uint32_t)bnum_to_dig_u(&jtag_number->as.number);

		t_json_v2 *jbase_type = NULL;
		json_v2_query_nonnull(__TYPE_BASE_TYPE_KEY, jtype, &jbase_type);
		assert(NULL != jbase_type && jbase_type->type == JSON_V2_TYPE_OBJECT);
		SSL_LOG(TRACE, "asn1 type: `%s` is %s", __TYPE_BASE_TYPE_KEY, json_v2_dumpb(jbase_type, cbuf, sizeof(cbuf)));
		if (SSL_OK != __asn1_v2_schema_parse_type(&atype->base_type, jbase_type)) {
			SSL_LOG(ERROR, "failed to parse `%s` from asn1 type: %s", __TYPE_BASE_TYPE_KEY, json_v2_dumpb(jbase_type, cbuf, sizeof(cbuf)));
			goto label_error;
		}
	}
	else if (atype->kind == ASN_V2_TYPE_KIND_REF) {
		t_json_v2 *jref_name = NULL;
		json_v2_query_nonnull(__TYPE_REF_NAME_KEY, jtype, &jref_name);
		assert(NULL != jref_name && jref_name->type == JSON_V2_TYPE_STRING);
		SSL_LOG(TRACE, "asn1 type: `%s` is %s", __TYPE_REF_NAME_KEY, json_v2_dumpb(jref_name, cbuf, sizeof(cbuf)));
		atype->ref_name = ft_strdup(jref_name->as.cstr);
	}

	*asn1_type = atype;
	return (SSL_OK);

label_error:
	__asn1_v2_type_delete(atype);
	return (SSL_ERR);
}

static int	__asn1_v2_schema_parse_typedef(t_asn_v2_typedef **asn1_typedef, t_json_v2 *jtypedef)
{
	assert(jtypedef->type == JSON_V2_TYPE_OBJECT);
	*asn1_typedef = NULL;

	t_asn_v2_typedef *atypedef = __asn1_v2_typedef_create();
	char cbuf[1024] = {0};

	t_json_v2 *jid = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__TYPEDEF_ID_KEY, jtypedef, &jid)) {
		assert(jid->type == JSON_V2_TYPE_STRING);
		SSL_LOG(TRACE, "asn1 typedef: `%s` is %s", __TYPEDEF_ID_KEY, json_v2_dumpb(jid, cbuf, sizeof(cbuf)));
		atypedef->id = ft_strdup(jid->as.cstr);
	}

	t_json_v2 *jtype = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__TYPEDEF_TYPE_KEY, jtypedef, &jtype)) {
		assert(jtype->type == JSON_V2_TYPE_OBJECT);
		SSL_LOG(TRACE, "asn1 typedef: `%s` is %s", __TYPEDEF_TYPE_KEY, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
		if (SSL_OK != __asn1_v2_schema_parse_type(&atypedef->type, jtype)) {
			SSL_LOG(ERROR, "failed to parse `%s` in asn1 typedef: %s", __TYPEDEF_TYPE_KEY, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			goto label_error;
		}
	}

	t_json_v2 *joptional = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__TYPEDEF_OPTIONAL_KEY, jtypedef, &joptional)) {
		assert(joptional->type == JSON_V2_TYPE_BOOL);
		SSL_LOG(TRACE, "asn1 typedef: `%s` is %s", __TYPEDEF_OPTIONAL_KEY, json_v2_dumpb(joptional, cbuf, sizeof(cbuf)));
		atypedef->optional = joptional->as.boolean;
	}

	t_json_v2 *jdefault = NULL;
	if (JSON_V2_OK == json_v2_query(__TYPEDEF_DEFAULT_KEY, jtypedef, &jdefault)) {
		SSL_LOG(TRACE, "asn1 typedef: `%s` is %s", __TYPEDEF_DEFAULT_KEY, json_v2_dumpb(jdefault, cbuf, sizeof(cbuf)));
		if (SSL_OK != __asn1_v2_schema_parse_value(&atypedef->default_value, jdefault)) {
			SSL_LOG(ERROR, "failed to parse `%s` in asn1 typedef: %s", __TYPEDEF_DEFAULT_KEY, json_v2_dumpb(jdefault, cbuf, sizeof(cbuf)));
			goto label_error;
		}
	}

	t_json_v2 *jelement_type = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__TYPEDEF_ELEMENT_TYPE_KEY, jtypedef, &jelement_type)) {
		assert(jelement_type->type == JSON_V2_TYPE_OBJECT);
		SSL_LOG(ERROR, "asn1 typedef: `%s` is %s", __TYPEDEF_ELEMENT_TYPE_KEY, json_v2_dumpb(jelement_type, cbuf, sizeof(cbuf)));
		if (SSL_OK != __asn1_v2_schema_parse_type(&atypedef->element_type, jelement_type)) {
			SSL_LOG(ERROR, "failed to parse `%s` in asn1 typedef: %s", __TYPEDEF_ELEMENT_TYPE_KEY, json_v2_dumpb(jelement_type, cbuf, sizeof(cbuf)));
			goto label_error;
		}
	}

	t_json_v2 *jelements = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__TYPEDEF_ELEMENTS_KEY, jtypedef, &jelements)) {
		assert(jelements->type == JSON_V2_TYPE_ARRAY);
		void *content = NULL;
		t_list_next next = {0};
		while (ft_list_next_content(&jelements->as.list, &next, &content)) {
			t_asn_v2_typedef *element_typedef = NULL;
			if (SSL_OK != __asn1_v2_schema_parse_typedef(&element_typedef, content)) {
				SSL_LOG(ERROR, "failed to parse `%s` in asn1 typedef: %s", __TYPEDEF_ELEMENT_TYPE_KEY, json_v2_dumpb(jelement_type, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			ft_list_append_content(&atypedef->elements, element_typedef);
		}
	}
	*asn1_typedef = atypedef;
	return (SSL_OK);

label_error:
	__asn1_v2_typedef_delete(atypedef);
	return (SSL_ERR);
}

int	asn1_v2_schema_parse(t_asn_v2_module **asn1_module, t_json_v2 *jschema)
{
	if (NULL == asn1_module) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != asn1_v2_schema_validate(jschema)) {
		SSL_LOG(ERROR, "cannot parse invalid json schema");
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "parsing asn1 schema json: %p", jschema);

	t_asn_v2_module *amodule = __asn1_v2_module_create();

	amodule->tag_mode = ASN_V2_TAG_MODE_EXPLICIT;
	t_json_v2 *jtagmode = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__SCHEMA_TAG_MODE_KEY, jschema, &jtagmode)) {
		amodule->tag_mode = __asn1_v2_get_tag_mode_by_name(jtagmode->as.cstr);
	}

	t_json_v2 *jtypedefs = NULL;
	json_v2_query_nonnull(__SCHEMA_TYPEDEFS_KEY, jschema, &jtypedefs);
	assert(NULL != jtypedefs && jtypedefs->type == JSON_V2_TYPE_OBJECT);

	const char *key = NULL;
	void *value = NULL;
	t_htbl_v2_next next = {0};
	while (ft_htbl_v2_next(&jtypedefs->as.htable, &next, &key, &value)) {
		t_asn_v2_typedef *atypedef = NULL;
		if (SSL_OK != __asn1_v2_schema_parse_typedef(&atypedef, value)) {
			SSL_LOG(ERROR, "failed to parse typedef: `%s`", key);
			goto label_error;
		}
		if (!ft_htbl_v2_set(&amodule->typedefs, key, atypedef)) {
			SSL_LOG(ERROR, "failed to set typedef: `%s`, duplicate key?", key);
			goto label_error;
		}
	}
	*asn1_module = amodule;
	return (SSL_OK);

label_error:
	__asn1_v2_module_delete(amodule);
	return (SSL_ERR);
}

/****************************************************************************/

static char *__asn1_v2_value_dumps(t_asn_v2_value *asn1_value)
{
	if (NULL == asn1_value) return ft_strdup("null");

	switch (asn1_value->type) {
	case ASN_V2_VALUE_TYPE_NULL:
		return ft_strdup("null");
	case ASN_V2_VALUE_TYPE_BOOLEAN:
		return (asn1_value->as.boolean) ? ft_strdup("true") : ft_strdup("false");
	case ASN_V2_VALUE_TYPE_NUMBER:
		return bnum_to_dec(&asn1_value->as.number);
	case ASN_V2_VALUE_TYPE_CSTRING:
		return ft_strjoin_multi(3, "\"", asn1_value->as.cstr, "\"");
	case ASN_V2_VALUE_TYPE_OSTRING:
		return ft_strjoin_multi(3, "\"", ft_ostr_to_cstr(&asn1_value->as.ostring, 0, asn1_value->as.ostring.size), "\"");
	case ASN_V2_VALUE_TYPE_LIST:
		;;
		t_ostring ostring = {0};
		ft_ostr_init_with_capacity(&ostring, 1024);
		t_list_next next = {0};
		void *content;
		size_t commas = 0;
		ft_ostr_append_cstr(&ostring, "[");
		while (ft_list_next_content(&asn1_value->as.list, &next, &content)) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = __asn1_v2_value_dumps(content);
			ft_ostr_append_cstr(&ostring, dumps);
			SSL_FREE(dumps);
		}
		ft_ostr_append_cstr(&ostring, "]");
		char *dumps = ft_ostr_to_cstr(&asn1_value->as.ostring, 0, asn1_value->as.ostring.size);
		ft_ostr_clear(&ostring);
		return (dumps);
	}
}

static char *__asn1_v2_type_dumps(t_asn_v2_type *asn1_type)
{
	if (NULL == asn1_type) return ft_strdup("null");

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);

	ft_ostr_appendf(&ostring, "{\"kind\":\"%s\",", asn1_v2_get_type_name(asn1_type->kind));
	ft_ostr_appendf(&ostring, "\"tag\":{\"mode\":\"%s\",\"class\":\"%s\",\"number\":%d,\"complex\":%s}",
		asn1_v2_get_tag_mode_name(asn1_type->tag.mode),
		asn1_v2_get_tag_class_name(asn1_type->tag.class),
		asn1_type->tag.number,
		asn1_type->tag.complex ? "true" : "false"
	);
	char *dumps = __asn1_v2_type_dumps(asn1_type->base_type);
	ft_ostr_appendf(&ostring, ",\"base_type\":%s", dumps);
	SSL_FREE(dumps);

	if (NULL != asn1_type->ref_name) {
		ft_ostr_appendf(&ostring, ",\"ref_name\":\"%s\"", asn1_type->ref_name);
	} else {
		ft_ostr_append_cstr(&ostring, ",\"ref_name\":null");
	}

	ft_ostr_append_cstr(&ostring, "}");
	dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

static char *__asn1_v2_typedef_dumps(t_asn_v2_typedef *asn1_typedef)
{
	if (NULL == asn1_typedef) return ft_strdup("null");

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);

	if (NULL != asn1_typedef->id) {
		ft_ostr_appendf(&ostring, "{\"id\":\"%s\",", asn1_typedef->id);
	} else {
		ft_ostr_append_cstr(&ostring, "{\"id\":null,");
	}

	char *dumps = __asn1_v2_type_dumps(asn1_typedef->type);
	ft_ostr_appendf(&ostring, "\"type\":%s,", dumps);
	SSL_FREE(dumps);

	dumps = __asn1_v2_value_dumps(asn1_typedef->default_value);
	ft_ostr_appendf(&ostring, "\"default_value\":%s,", dumps);
	SSL_FREE(dumps);

	dumps = __asn1_v2_type_dumps(asn1_typedef->element_type);
	ft_ostr_appendf(&ostring, "\"element_type\":%s,", dumps);
	SSL_FREE(dumps);

	t_list_next next = {0};
	void *content;
	size_t commas = 0;
	ft_ostr_append_cstr(&ostring, "\"elements\":[");
	while (ft_list_next_content(&asn1_typedef->elements, &next, &content)) {
		if (commas++) ft_ostr_append_cstr(&ostring, ",");
		dumps = __asn1_v2_typedef_dumps(content);
		ft_ostr_append_cstr(&ostring, dumps);
		SSL_FREE(dumps);
	}
	ft_ostr_append_cstr(&ostring, "],");

	ft_ostr_appendf(&ostring, "\"optional\":%s}", asn1_typedef->optional ? "true" : "false");

	dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);

	return (dumps);
}

char *asn1_v2_module_dumps(t_asn_v2_module *asn1_module)
{
	if (NULL == asn1_module) return ft_strdup("null");

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);

	ft_ostr_appendf(&ostring, "{\"tag_mode\":\"%s\",\"typedefs\":{", asn1_v2_get_tag_mode_name(asn1_module->tag_mode));

	t_htbl_v2_next next = {0};
	const char *key = NULL;
	void *value = NULL;
	size_t commas = 0;
	while (ft_htbl_v2_next(&asn1_module->typedefs, &next, &key, &value)) {
		if (commas++) ft_ostr_append_cstr(&ostring, ",");
		char *dumps = __asn1_v2_typedef_dumps(value);
		ft_ostr_appendf(&ostring, "\"%s\":%s", key, dumps);
		SSL_FREE(dumps);
	}
	ft_ostr_append_cstr(&ostring, "}}");
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);

	return (dumps);
}
