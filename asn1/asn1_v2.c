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
#define ASN_V2_TAG_MODE_NAME_AUTOMATIC    "automatic"

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

#define ASN_V2_CONSTRAINT_TYPE_NAME_RANGE "range"
#define ASN_V2_CONSTRAINT_TYPE_NAME_SIZE  "size"

#define __JQ_SCHEMA_TAG_MODE	".tagMode"
#define __JQ_SCHEMA_TYPEDEFS	".typedefs"

#define __JQ_TYPEDEF_ID 		".id"
#define __JQ_TYPEDEF_TYPE 		".type"
#define __JQ_TYPEDEF_OPTIONAL	".optional"
#define __JQ_TYPEDEF_DEFAULT	".default"

#define __JQ_TYPE_KIND			".kind"
#define __JQ_TYPE_TAG_MODE		".tagMode"
#define __JQ_TYPE_TAG_CLASS		".tagClass"
#define __JQ_TYPE_TAG_NUMBER	".tagNumber"
#define __JQ_TYPE_BASE_TYPE		".baseType"
#define __JQ_TYPE_REF_NAME		".refName"
#define __JQ_TYPE_ELEMENT_TYPE	".elementType"
#define __JQ_TYPE_ELEMENTS		".elements"

t_asn_v2_tag asn1_v2_get_universal_tag(t_asn_v2_type_kind type)
{
	t_asn_v2_tag_class class = ASN_V2_TAG_CLASS_UNIVERSAL;
	switch (type) {
	case ASN_V2_TYPE_KIND_INT:
		return (t_asn_v2_tag){ .class = class, .constructed = false, .number = ASN_V2_TAG_NUMBER_INTEGER, };
	case ASN_V2_TYPE_KIND_BOOLEAN:
		return (t_asn_v2_tag){ .class = class, .constructed = false, .number = ASN_V2_TAG_NUMBER_BOOLEAN, };
	case ASN_V2_TYPE_KIND_BIT_STRING:
		return (t_asn_v2_tag){ .class = class, .constructed = false, .number = ASN_V2_TAG_NUMBER_BIT_STRING, };
	case ASN_V2_TYPE_KIND_OCTET_STRING:
		return (t_asn_v2_tag){ .class = class, .constructed = false, .number = ASN_V2_TAG_NUMBER_OCTET_STRING, };
	case ASN_V2_TYPE_KIND_IA5_STRING:
		return (t_asn_v2_tag){ .class = class, .constructed = false, .number = ASN_V2_TAG_NUMBER_IA5_STRING, };
	case ASN_V2_TYPE_KIND_UTF8_STRING:
		return (t_asn_v2_tag){ .class = class, .constructed = false, .number = ASN_V2_TAG_NUMBER_UTF8_STRING, };
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
		return (t_asn_v2_tag){ .class = class, .constructed = false, .number = ASN_V2_TAG_NUMBER_PRINTABLE_STRING, };
	case ASN_V2_TYPE_KIND_NULL:
		return (t_asn_v2_tag){ .class = class, .constructed = false, .number = ASN_V2_TAG_NUMBER_NULL, };
	case ASN_V2_TYPE_KIND_OBJECT_ID:
		return (t_asn_v2_tag){ .class = class, .constructed = false, .number = ASN_V2_TAG_NUMBER_OBJECT_ID, };
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		return (t_asn_v2_tag){ .class = class, .constructed = false, .number = ASN_V2_TAG_NUMBER_OBJECT_DESCR, };
	case ASN_V2_TYPE_KIND_SEQUENCE:
		return (t_asn_v2_tag){ .class = class, .constructed = true, .number = ASN_V2_TAG_NUMBER_SEQUENCE, };
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
		return (t_asn_v2_tag){ .class = class, .constructed = true, .number = ASN_V2_TAG_NUMBER_SEQUENCE_OF, };
	case ASN_V2_TYPE_KIND_SET:
		return (t_asn_v2_tag){ .class = class, .constructed = true, .number = ASN_V2_TAG_NUMBER_SET, };
	case ASN_V2_TYPE_KIND_SET_OF:
		return (t_asn_v2_tag){ .class = class, .constructed = true, .number = ASN_V2_TAG_NUMBER_SET_OF, };
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_TAGGED:
	case ASN_V2_TYPE_KIND_REF:
		UNREACHABLE("asn1_v2_get_universal_tag");
	}
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
	case ASN_V2_TAG_MODE_AUTOMATIC:		return ASN_V2_TAG_MODE_NAME_AUTOMATIC;
	default:							return "unknown";
	}
}

const char *asn1_v2_get_constraint_type_name(t_asn_v2_constraint_type type)
{
	switch (type) {
	case ASN_V2_CONSTRAINT_TYPE_RANGE:	return ASN_V2_CONSTRAINT_TYPE_NAME_RANGE;
	case ASN_V2_CONSTRAINT_TYPE_SIZE:	return ASN_V2_CONSTRAINT_TYPE_NAME_SIZE;
	default:							return "unknown";
	}
}

/****************************************************************************/

#define __ASN_V2_TYPE_KIND_UNKNOWN       -1
#define __ASN_V2_TAG_CLASS_UNKNOWN       -1
#define __ASN_V2_TAG_MODE_UNKNOWN        -1
#define __ASN_V2_CONSTRAINT_TYPE_UNKNOWN -1

static t_asn_v2_constraint_type __asn1_v2_get_constraint_type_by_name(const char *name)
{
	assert(NULL != name);

	if (ft_streq(name, ASN_V2_CONSTRAINT_TYPE_NAME_RANGE))	return ASN_V2_CONSTRAINT_TYPE_RANGE;
	if (ft_streq(name, ASN_V2_CONSTRAINT_TYPE_NAME_SIZE))	return ASN_V2_CONSTRAINT_TYPE_SIZE;

	return __ASN_V2_CONSTRAINT_TYPE_UNKNOWN;
}

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
	if (ft_streq(name, ASN_V2_TAG_MODE_NAME_AUTOMATIC))		return (ASN_V2_TAG_MODE_AUTOMATIC);

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

/****************************************************************************/

int __asn1_v2_schema_validate_constraint_type_name(const char *name)
{
	if (NULL == name) return (SSL_ERR);
	if (__ASN_V2_CONSTRAINT_TYPE_UNKNOWN == __asn1_v2_get_constraint_type_by_name(name)) return (SSL_ERR);
	return (SSL_OK);
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

static int __asn1_v2_schema_validate_type(t_json_v2 *jtype)
{
	if (jtype->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid asn1 typedef: expected json object but got json %s", json_v2_get_type_name(jtype->type));
		return (SSL_ERR);
	}

	t_json_v2 *jkind = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_KIND, jtype, &jkind)) {
		SSL_LOG(ERROR, "invalid asn1 type: expected `%s` key", __JQ_TYPE_KIND);
		return (SSL_ERR);
	}
	else {
		if (jkind->type != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid asn1 type: expected `%s` to be a json string but got json %s", __JQ_TYPE_KIND, json_v2_get_type_name(jkind->type));
			return (SSL_ERR);
		}
		if (SSL_OK != __asn1_v2_schema_validate_type_name(jkind->as.cstr)) {
			SSL_LOG(ERROR, "invalid asn1 type: `%s`", jkind->as.cstr);
			return (SSL_ERR);
		}
	}
	t_asn_v2_type_kind asn1_type = __asn1_v2_get_type_by_name(jkind->as.cstr);

	switch (asn1_type) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET:
		;;
		t_json_v2 *jelements = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_ELEMENTS, jtype, &jelements)) {
			SSL_LOG(ERROR, "asn1 typedef: no `%s` key specified", __JQ_TYPE_ELEMENTS);
			return (SSL_ERR);
		}
		else {
			void *content = NULL;
			t_list_next next = {0};
			while (ft_list_next_content(&jelements->as.list, &next, &content)) {
				if (SSL_OK != __asn1_v2_schema_validate_typedef(content)) {
					SSL_LOG(ERROR, "invalid asn1 typedef: `%s` contains invalid typedef", __JQ_TYPE_ELEMENTS);
					return (SSL_ERR);
				}
			}
		}
		break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		;;
		t_json_v2 *jelement_type = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_ELEMENT_TYPE, jtype, &jelement_type)) {
			SSL_LOG(ERROR, "asn1 type: no `%s` key specified", __JQ_TYPE_ELEMENT_TYPE);
			return (SSL_ERR);
		}
		else {
			if (SSL_OK != __asn1_v2_schema_validate_type(jelement_type)) {
				SSL_LOG(ERROR, "invalid asn1 type: invalid `%s`", __JQ_TYPE_ELEMENT_TYPE);
				return (SSL_ERR);
			}
		}
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
		;;
		t_json_v2 *jtag_mode = NULL;
		if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_TAG_MODE, jtype, &jtag_mode)) {
			if (jtag_mode->type != JSON_V2_TYPE_STRING) {
				SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be a json string but got json %s", __JQ_TYPE_TAG_MODE, json_v2_get_type_name(jtag_mode->type));
				return (SSL_ERR);
			}
			if (SSL_OK != __asn1_v2_schema_validate_tag_mode_name(jtag_mode->as.cstr)) {
				SSL_LOG(ERROR, "invalid asn1 type: bad tag mode `%s`", jtag_mode->as.cstr);
				return (SSL_ERR);
			}
		}
		t_json_v2 *jtag_class = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_TAG_CLASS, jtype, &jtag_class)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` key", __JQ_TYPE_TAG_CLASS);
			return (SSL_ERR);
		}
		else {
			if (jtag_class->type != JSON_V2_TYPE_STRING) {
				SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be a json string but got json %s", __JQ_TYPE_TAG_CLASS, json_v2_get_type_name(jtag_class->type));
				return (SSL_ERR);
			}
			if (SSL_OK != __asn1_v2_schema_validate_tag_class_name(jtag_class->as.cstr)) {
				SSL_LOG(ERROR, "invalid asn1 type: bad tag class: `%s`", jtag_class->as.cstr);
				return (SSL_ERR);
			}
		}
		t_json_v2 *jtag_number = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_TAG_NUMBER, jtype, &jtag_number)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` key", __JQ_TYPE_TAG_NUMBER);
			return (SSL_ERR);
		}
		else {
			if (jtag_number->type != JSON_V2_TYPE_NUMBER) {
				SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be a json number but got json %s", __JQ_TYPE_TAG_NUMBER, json_v2_get_type_name(jtag_number->type));
				return (SSL_ERR);
			}
		}
		t_json_v2 *jbase_type = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_BASE_TYPE, jtype, &jbase_type)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` key", __JQ_TYPE_BASE_TYPE);
			return (SSL_ERR);
		}
		else {
			if (SSL_OK != __asn1_v2_schema_validate_type(jbase_type)) {
				SSL_LOG(ERROR, "invalid asn1 typedef: bad type: `%s`", __JQ_TYPE_BASE_TYPE);
				return (SSL_ERR);
			}
		}
		break;
	case ASN_V2_TYPE_KIND_REF:
		;;
		t_json_v2 *jref_name = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_REF_NAME, jtype, &jref_name)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be specified for asn1 %s type", __JQ_TYPE_REF_NAME, ASN_V2_TYPE_NAME_REF);
			return (SSL_ERR);
		}
		else {
			if (jref_name->type != JSON_V2_TYPE_STRING) {
				SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be a json string but got json %s", __JQ_TYPE_KIND, json_v2_get_type_name(jref_name->type));
				return (SSL_ERR);
			}
		}
		break;
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INT:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		break;
	default:
		UNREACHABLE("__asn1_v2_schema_validate_type");
	}

	return (SSL_OK);
}

static int __asn1_v2_schema_validate_typedef(t_json_v2 *jtypedef)
{
	if (jtypedef->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid asn1 typedef: expected a json object but got json %s", json_v2_get_type_name(jtypedef->type));
		return (SSL_ERR);
	}

	t_json_v2 *jid = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPEDEF_ID, jtypedef, &jid)) {
		SSL_LOG(TRACE, "no asn1 `%s` key specified ", __JQ_TYPEDEF_ID);
	}
	else {
		if (jid->type != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be a json string but got json %s", __JQ_TYPEDEF_ID, json_v2_get_type_name(jid->type));
			return (SSL_ERR);
		}
	}

	t_json_v2 *jtype = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPEDEF_TYPE, jtypedef, &jtype)) {
		SSL_LOG(TRACE, "no asn1 `%s` key specified ", __JQ_TYPEDEF_TYPE);
	}
	else {
		if (SSL_OK != __asn1_v2_schema_validate_type(jtype)) {
			SSL_LOG(ERROR, "invalid asn1 typedef: invalid `%s`", __JQ_TYPEDEF_TYPE);
			return (SSL_ERR);
		}
	}

	t_json_v2 *joptional = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPEDEF_OPTIONAL, jtypedef, &joptional)) {
		SSL_LOG(TRACE, "asn1 typedef: no `%s` key specified", __JQ_TYPEDEF_OPTIONAL);
	}
	else {
		if (joptional->type != JSON_V2_TYPE_BOOL) {
			SSL_LOG(ERROR, "invalid asn1 typedef: expected `%s` to be a json boolean but got json %s", __JQ_TYPEDEF_OPTIONAL, json_v2_get_type_name(joptional->type));
			return (SSL_ERR);
		}
	}

	t_json_v2 *jdefault = NULL;
	if (JSON_V2_OK == json_v2_query(__JQ_TYPEDEF_DEFAULT, jtypedef, &jdefault)) {
		SSL_LOG(TRACE, "asn1 typedef: `%s` key is set, value type: %s", __JQ_TYPEDEF_DEFAULT, json_v2_get_type_name(jdefault->type));
	}

	return (SSL_OK);
}

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
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_SCHEMA_TAG_MODE, jschema, &jtagmode)) {
		SSL_LOG(TRACE, "no default asn1 tagging mode specified");
	}
	else {
		if (jtagmode->type != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid json schema: expected `%s` to be a json string but got %s", __JQ_SCHEMA_TAG_MODE, json_v2_get_type_name(jtagmode->type));
			return (SSL_ERR);
		}
		if (SSL_OK != __asn1_v2_schema_validate_tag_mode_name(jtagmode->as.cstr)) {
			SSL_LOG(ERROR, "invalid `%s`: %s", __JQ_SCHEMA_TAG_MODE, jtagmode->as.cstr);
			return (SSL_ERR);
		}
	}

	t_json_v2 *jtypedefs = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_SCHEMA_TYPEDEFS, jschema, &jtypedefs)) {
		SSL_LOG(ERROR, "invalid json schema: expected `%s` key", __JQ_SCHEMA_TYPEDEFS);
		return (SSL_ERR);
	}
	if (jtypedefs->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid json schema: expected `%s` to be a json object but got json %s", __JQ_SCHEMA_TYPEDEFS, json_v2_get_type_name(jtypedefs->type));
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

static char *__asn1_v2_value_dumps(const t_asn_v2_value *asn1_value);
static char *__asn1_v2_type_dumps(const t_asn_v2_type *asn1_type);
static char *__asn1_v2_typedef_dumps(const t_asn_v2_typedef *asn1_typedef);

static char *__asn1_v2_value_dumpb(const t_asn_v2_value *asn1_value, char *buf, size_t size);
static char *__asn1_v2_type_dumpb(const t_asn_v2_type *asn1_type, char *buf, size_t size);
static char *__asn1_v2_typedef_dumpb(const t_asn_v2_typedef *asn1_typedef, char *buf, size_t size);

static char *__asn1_v2_value_dumps(const t_asn_v2_value *asn1_value)
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
	case ASN_V2_VALUE_TYPE_BSTRING:
		;;
		char *ostring_dumps = NULL;
		ft_sprintf(&ostring_dumps, "\"<octets=%p,size=%zu\"", &asn1_value->as.ostring, asn1_value->as.ostring.size);
		return ostring_dumps;
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

static char *__asn1_v2_type_dumps(const t_asn_v2_type *asn1_type)
{
	if (NULL == asn1_type) return ft_strdup("null");

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);

	ft_ostr_appendf(&ostring, "{\"kind\":\"%s\"", asn1_v2_get_type_name(asn1_type->kind));

	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET:
		;;
		t_list_next next = {0};
		void *content;
		size_t commas = 0;
		ft_ostr_append_cstr(&ostring, ",\"elements\":[");
		while (ft_list_next_content(&asn1_type->as.constructed.elements, &next, &content)) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = __asn1_v2_typedef_dumps(content);
			ft_ostr_append_cstr(&ostring, dumps);
			SSL_FREE(dumps);
		}
		ft_ostr_append_cstr(&ostring, "]");
		break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		;;
		char *dumps = __asn1_v2_type_dumps(asn1_type->as.collection.element_type);
		ft_ostr_appendf(&ostring, ",\"element_type\":%s", dumps);
		SSL_FREE(dumps);
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
		ft_ostr_appendf(&ostring, ",\"tag_mode\":\"%s\"", asn1_v2_get_tag_mode_name(asn1_type->as.tagged.tag_mode));
		ft_ostr_appendf(&ostring, ",\"tag\":{\"class\":\"%s\",\"number\":%d,\"constructed\":%s}",
			asn1_v2_get_tag_class_name(asn1_type->as.tagged.tag.class),
			asn1_type->as.tagged.tag.number,
			asn1_type->as.tagged.tag.constructed ? "true" : "false"
		);
		dumps = __asn1_v2_type_dumps(asn1_type->as.tagged.base_type);
		ft_ostr_appendf(&ostring, ",\"base_type\":%s", dumps);
		SSL_FREE(dumps);
		break;
	case ASN_V2_TYPE_KIND_REF:
		if (NULL != asn1_type->as.ref.name) {
			ft_ostr_appendf(&ostring, ",\"ref_name\":\"%s\"", asn1_type->as.ref.name);
		} else {
			ft_ostr_append_cstr(&ostring, ",\"ref_name\":null");
		}
		break;
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INT:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		break;
	default:
		UNREACHABLE("__asn1_v2_type_delete");
	}

	ft_ostr_append_cstr(&ostring, "}");
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

static char *__asn1_v2_typedef_dumps(const t_asn_v2_typedef *asn1_typedef)
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

	ft_ostr_appendf(&ostring, "\"optional\":%s", asn1_typedef->optional ? "true" : "false");
	ft_ostr_append_cstr(&ostring, "}");

	dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);

	return (dumps);
}

char *asn1_v2_module_dumps(const t_asn_v2_module *asn1_module)
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

static char *__asn1_v2_value_dumpb(const t_asn_v2_value *asn1_value, char *buf, size_t size)
{
	assert(NULL != buf);
	if (size == 0) return (NULL);

	char *dumps = __asn1_v2_value_dumps(asn1_value);
	size_t len = ft_strlen(dumps);
	if (len >= size) {
		len = size-1;
	}
	ft_strncpy(buf, dumps, len);
	buf[len] = '\0';
	SSL_FREE(dumps);

	return (buf);
}

static char *__asn1_v2_type_dumpb(const t_asn_v2_type *asn1_type, char *buf, size_t size)
{
	assert(NULL != buf);
	if (size == 0) return (NULL);

	char *dumps = __asn1_v2_type_dumps(asn1_type);
	size_t len = ft_strlen(dumps);
	if (len >= size) {
		len = size-1;
	}
	ft_strncpy(buf, dumps, len);
	buf[len] = '\0';
	SSL_FREE(dumps);

	return (buf);
}

static char *__asn1_v2_typedef_dumpb(const t_asn_v2_typedef *asn1_typedef, char *buf, size_t size)
{
	assert(NULL != buf);
	if (size == 0) return (NULL);

	char *dumps = __asn1_v2_typedef_dumps(asn1_typedef);
	size_t len = ft_strlen(dumps);
	if (len >= size) {
		len = size-1;
	}
	ft_strncpy(buf, dumps, len);
	buf[len] = '\0';
	SSL_FREE(dumps);

	return (buf);
}

char *asn1_v2_module_dumpb(const t_asn_v2_module *asn1_module, char *buf, size_t size)
{
	assert(NULL != buf);
	if (size == 0) return (NULL);

	char *dumps = asn1_v2_module_dumps(asn1_module);
	size_t len = ft_strlen(dumps);
	if (len >= size) {
		len = size-1;
	}
	ft_strncpy(buf, dumps, len);
	buf[len] = '\0';
	SSL_FREE(dumps);

	return (buf);
}

/****************************************************************************/

static t_asn_v2_tag	*__asn1_v2_tag_create(void);
static void __asn1_v2_tag_clear(t_asn_v2_tag *asn1_tag);
static void __asn1_v2_tag_delete(t_asn_v2_tag *asn1_tag);
static void	__asn1_v2_tag_copy(const t_asn_v2_tag *src, t_asn_v2_tag *dst);

static t_asn_v2_module *__asn1_v2_module_create(void);
static void __asn1_v2_module_clear(t_asn_v2_module *asn1_module);
static void __asn1_v2_module_delete(t_asn_v2_module *asn1_module);
static void __asn1_v2_module_copy(const t_asn_v2_module *src, t_asn_v2_module *dst);

static t_asn_v2_typedef *__asn1_v2_typedef_create(void);
static void __asn1_v2_typedef_clear(t_asn_v2_typedef *asn1_typedef);
static void __asn1_v2_typedef_delete(t_asn_v2_typedef *asn1_typedef);
static void __asn1_v2_typedef_copy(const t_asn_v2_typedef *src, t_asn_v2_typedef *dst);

static t_asn_v2_value *__asn1_v2_value_create(void);
static void __asn1_v2_value_clear(t_asn_v2_value *asn1_value);
static void __asn1_v2_value_delete(t_asn_v2_value *asn1_value);
static void __asn1_v2_value_copy(const t_asn_v2_value *src, t_asn_v2_value *dst);

static t_asn_v2_type *__asn1_v2_type_create(void);
static void __asn1_v2_type_clear(t_asn_v2_type *asn1_type);
static void __asn1_v2_type_delete(t_asn_v2_type *asn1_type);
static void __asn1_v2_type_copy(const t_asn_v2_type *src, t_asn_v2_type *dst);

static inline void __asn1_v2_tag_delete_adapter(void *p)	 {__asn1_v2_tag_delete((t_asn_v2_tag *)p);}
static inline void __asn1_v2_value_delete_adapter(void *p)	 {__asn1_v2_value_delete((t_asn_v2_value *)p);}
static inline void __asn1_v2_type_delete_adapter(void *p)	 {__asn1_v2_type_delete((t_asn_v2_type *)p);}
static inline void __asn1_v2_typedef_delete_adapter(void *p) {__asn1_v2_typedef_delete((t_asn_v2_typedef *)p);}
static inline void __asn1_v2_module_delete_adapter(void *p)	 {__asn1_v2_module_delete((t_asn_v2_module *)p);}

static inline void *__asn1_v2_tag_copy_adapter(void *p)
{
	t_asn_v2_tag *dst = __asn1_v2_tag_create();
	__asn1_v2_tag_copy((t_asn_v2_tag *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_value_copy_adapter(void *p)
{
	t_asn_v2_value *dst = __asn1_v2_value_create();
	__asn1_v2_value_copy((t_asn_v2_value *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_type_copy_adapter(void *p)
{
	t_asn_v2_type *dst = __asn1_v2_type_create();
	__asn1_v2_type_copy((t_asn_v2_type *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_typedef_copy_adapter(void *p)
{
	t_asn_v2_typedef *dst = __asn1_v2_typedef_create();
	__asn1_v2_typedef_copy((t_asn_v2_typedef *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_module_copy_adapter(void *p)
{
	t_asn_v2_module *dst = __asn1_v2_module_create();
	__asn1_v2_module_copy((t_asn_v2_module *)p, dst);
	return (dst);
}

static t_asn_v2_tag	*__asn1_v2_tag_create(void)
{
	t_asn_v2_tag *tag = NULL;
	SSL_ALLOC(tag, sizeof(t_asn_v2_tag));
	*tag = (t_asn_v2_tag){0};
	return (tag);
}

static void __asn1_v2_tag_clear(t_asn_v2_tag *asn1_tag)
{
	*asn1_tag = (t_asn_v2_tag){0};
}

static void __asn1_v2_tag_delete(t_asn_v2_tag *asn1_tag)
{
	__asn1_v2_tag_clear(asn1_tag);
	SSL_FREE(asn1_tag);
}

static void	__asn1_v2_tag_copy(const t_asn_v2_tag *src, t_asn_v2_tag *dst)
{
	*dst = *src;
}

static t_asn_v2_value *__asn1_v2_value_create(void)
{
	t_asn_v2_value *asn1_value = NULL;
	SSL_ALLOC(asn1_value, sizeof(t_asn_v2_value));
	*asn1_value = (t_asn_v2_value){0};
	return (asn1_value);
}

static void __asn1_v2_value_clear(t_asn_v2_value *asn1_value)
{
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
}

static void __asn1_v2_value_delete(t_asn_v2_value *asn1_value)
{
	__asn1_v2_value_clear(asn1_value);
	SSL_FREE(asn1_value);
}

static void	__asn1_v2_value_copy(const t_asn_v2_value *src, t_asn_v2_value *dst)
{
	dst->type = src->type;

	switch (src->type) {
	case ASN_V2_VALUE_TYPE_NULL:
		break;
	case ASN_V2_VALUE_TYPE_BOOLEAN:
		dst->as.boolean = src->as.boolean;
		break;
	case ASN_V2_VALUE_TYPE_NUMBER:
		bnum_copy(&src->as.number, &dst->as.number);
		break;
	case ASN_V2_VALUE_TYPE_CSTRING:
		dst->as.cstr = ft_strdup(src->as.cstr);
		break;
	case ASN_V2_VALUE_TYPE_OSTRING:
		ft_ostr_copy(&src->as.ostring, &dst->as.ostring);
		break;
	case ASN_V2_VALUE_TYPE_LIST:
		;;
		void *content = NULL;
		t_list_next next = {0};
		while (ft_list_next_content(&src->as.list, &next, &content)) {
			t_asn_v2_value *copy_value = __asn1_v2_value_create();
			__asn1_v2_value_copy(content, copy_value);
			ft_list_append_content(&dst->as.list, copy_value);
		}
	default:
		UNREACHABLE("__asn1_v2_type_copy");
	}
}

static t_asn_v2_constraint *__asn1_v2_constraint_create(void)
{
	t_asn_v2_constraint *constraint = NULL;
	SSL_ALLOC(constraint, sizeof(t_asn_v2_constraint));
	*constraint = (t_asn_v2_constraint){0};
	return (constraint);
}

static void	__asn1_v2_constraint_clear(t_asn_v2_constraint *asn1_constraint)
{
	switch (asn1_constraint->type) {
	case ASN_V2_CONSTRAINT_TYPE_RANGE:
		bnum_clear(asn1_constraint->as.range.min);
		bnum_clear(asn1_constraint->as.range.max);
		break;
	case ASN_V2_CONSTRAINT_TYPE_SIZE:
		break;
	default:
		UNREACHABLE("__asn1_v2_constrain_copy");
	}
}

static void	__asn1_v2_constraint_delete(t_asn_v2_constraint *asn1_constraint)
{
	__asn1_v2_constraint_clear(asn1_constraint);
	SSL_FREE(asn1_constraint);
}

static void	__asn1_v2_constraint_copy(const t_asn_v2_constraint *src, t_asn_v2_constraint *dst)
{
	dst->type = src->type;
	switch (src->type) {
	case ASN_V2_CONSTRAINT_TYPE_RANGE:
		bnum_copy(src->as.range.min, dst->as.range.min);
		bnum_copy(src->as.range.max, dst->as.range.max);
		break;
	case ASN_V2_CONSTRAINT_TYPE_SIZE:
		dst->as.size.min = src->as.size.min;
		dst->as.size.max = src->as.size.max;
		break;
	default:
		UNREACHABLE("__asn1_v2_constrain_copy");
	}
}

static t_asn_v2_type *__asn1_v2_type_create(void)
{
	t_asn_v2_type *asn1_type = NULL;
	SSL_ALLOC(asn1_type, sizeof(t_asn_v2_type));
	*asn1_type = (t_asn_v2_type){0};
	return (asn1_type);
}

static void __asn1_v2_type_clear(t_asn_v2_type *asn1_type)
{
	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET:
		ft_list_clear_all_content(&asn1_type->as.constructed.elements, __asn1_v2_typedef_delete_adapter);
		break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		if (NULL != asn1_type->as.collection.element_type) __asn1_v2_type_delete(asn1_type->as.collection.element_type);
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
		if (NULL != asn1_type->as.tagged.base_type) __asn1_v2_type_delete(asn1_type->as.tagged.base_type);
		break;
	case ASN_V2_TYPE_KIND_REF:
		if (NULL != asn1_type->as.ref.name) SSL_FREE(asn1_type->as.ref.name);
		break;
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INT:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		break;
	default:
		UNREACHABLE("__asn1_v2_type_delete");
	}
}

static void __asn1_v2_type_delete(t_asn_v2_type *asn1_type)
{
	__asn1_v2_type_clear(asn1_type);
	SSL_FREE(asn1_type);
}

static void	__asn1_v2_type_copy(const t_asn_v2_type *src, t_asn_v2_type *dst)
{
	*dst = (t_asn_v2_type){0};
	dst->kind = src->kind;

	void *content = NULL;
	t_list_next next = {0};
	while (ft_list_next_content(&src->constraints, &next, &content)) {
		t_asn_v2_constraint *dst_constraint = __asn1_v2_constraint_create();
		__asn1_v2_constraint_copy(content, dst_constraint);
		ft_list_append_content(&dst->constraints, dst_constraint);
	}

	switch (src->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET:
		;;
		void *content = NULL;
		t_list_next next = {0};
		while (ft_list_next_content(&src->as.constructed.elements, &next, &content)) {
			t_asn_v2_typedef *dst_element = __asn1_v2_typedef_create();
			__asn1_v2_typedef_copy(content, dst_element);
			ft_list_append_content(&dst->as.constructed.elements, dst_element);
		}
		break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		dst->as.collection.element_type = __asn1_v2_type_create();
		__asn1_v2_type_copy(src->as.collection.element_type, dst->as.collection.element_type);
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
		dst->as.tagged.base_type = __asn1_v2_type_create();
		__asn1_v2_type_copy(src->as.tagged.base_type, dst->as.tagged.base_type);
		break;
	case ASN_V2_TYPE_KIND_REF:
		dst->as.ref.name = ft_strdup(src->as.ref.name);
		break;
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INT:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		break;
	default:
		UNREACHABLE("__asn1_v2_type_copy");
	}
}

static t_asn_v2_typedef *__asn1_v2_typedef_create(void)
{
	t_asn_v2_typedef *asn1_typedef = NULL;
	SSL_ALLOC(asn1_typedef, sizeof(t_asn_v2_typedef));
	*asn1_typedef = (t_asn_v2_typedef){0};
	return (asn1_typedef);
}

static void __asn1_v2_typedef_clear(t_asn_v2_typedef *asn1_typedef)
{
	if (NULL != asn1_typedef->id) SSL_FREE(asn1_typedef->id);
	if (NULL != asn1_typedef->type) __asn1_v2_type_delete(asn1_typedef->type);
	if (NULL != asn1_typedef->default_value) __asn1_v2_value_delete(asn1_typedef->default_value);
}

static void __asn1_v2_typedef_delete(t_asn_v2_typedef *asn1_typedef)
{
	__asn1_v2_typedef_clear(asn1_typedef);
	SSL_FREE(asn1_typedef);
}

static void	__asn1_v2_typedef_copy(const t_asn_v2_typedef *src, t_asn_v2_typedef *dst)
{
	dst->id = ft_strdup(src->id);
	dst->type = __asn1_v2_type_create();
	__asn1_v2_type_copy(src->type, dst->type);
	if (NULL != src->default_value) {
		dst->default_value = __asn1_v2_value_create();
		__asn1_v2_value_copy(src->default_value, dst->default_value);
	}
	dst->optional = src->optional;
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
	__asn1_v2_module_clear(asn1_module);
	SSL_FREE(asn1_module);
}

static void __asn1_v2_module_clear(t_asn_v2_module *asn1_module)
{
	ft_htbl_v2_clear(&asn1_module->typedefs, __asn1_v2_typedef_delete_adapter);
}

static void __asn1_v2_module_copy(const t_asn_v2_module *src, t_asn_v2_module *dst)
{
	dst->tag_mode = src->tag_mode;
	ft_htbl_v2_init(&dst->typedefs, src->typedefs.size);

	const char *key = NULL;
	void *value = NULL;
	t_htbl_v2_next next = {0};
	while (ft_htbl_v2_next(&src->typedefs, &next, &key, &value)) {
		t_asn_v2_typedef *copy_typedef = __asn1_v2_typedef_create();
		__asn1_v2_typedef_copy(value, copy_typedef);
		int ok = ft_htbl_v2_set(&dst->typedefs, key, copy_typedef);
		assert(ok);
	}
}

/****************************************************************************/

static int	__asn1_v2_type_resolve_refs(const t_asn_v2_module *asn1_module, t_asn_v2_type *asn1_type);
static int	__asn1_v2_typedef_resolve_refs(const t_asn_v2_module *asn1_module, t_asn_v2_typedef *asn1_typedef);

static int	__asn1_v2_type_resolve_refs(const t_asn_v2_module *asn1_module, t_asn_v2_type *asn1_type)
{
	char cbuf[1024] = {0};

	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET:
		;;
		void *content = NULL;
		t_list_next next = {0};
		while (ft_list_next_content(&asn1_type->as.constructed.elements, &next, &content)) {
			t_asn_v2_typedef *element = content;
			if (SSL_OK != __asn1_v2_typedef_resolve_refs(asn1_module, element)) {
				SSL_LOG(ERROR, "failed to resolve refs for asn1 typedef: %s", __asn1_v2_typedef_dumpb(element, cbuf, sizeof(cbuf)));
				return (SSL_ERR);
			}
		}
		return (SSL_OK);
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		if (SSL_OK != __asn1_v2_type_resolve_refs(asn1_module, asn1_type->as.collection.element_type)) {
			SSL_LOG(ERROR, "failed to resolve refs for `%s` asn1 type: %s", asn1_v2_get_type_name(asn1_type->kind), __asn1_v2_type_dumpb(asn1_type, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		return (SSL_OK);
	case ASN_V2_TYPE_KIND_TAGGED:
		if (SSL_OK != __asn1_v2_type_resolve_refs(asn1_module, asn1_type->as.tagged.base_type)) {
			SSL_LOG(ERROR, "failed to resolve refs for `%s` asn1 type: %s", asn1_v2_get_type_name(asn1_type->kind), __asn1_v2_type_dumpb(asn1_type, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		return (SSL_OK);
	case ASN_V2_TYPE_KIND_REF:
		;;
		t_asn_v2_typedef *atypedef = ft_htbl_v2_get(&asn1_module->typedefs, asn1_type->as.ref.name);
		assert(NULL != atypedef);
		if (SSL_OK != __asn1_v2_typedef_resolve_refs(asn1_module, atypedef)) {
			SSL_LOG(ERROR, "failed to resolve refs for `%s` asn1 typedef: %s", asn1_type->as.ref.name, __asn1_v2_typedef_dumpb(atypedef, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		__asn1_v2_type_clear(asn1_type);
		__asn1_v2_type_copy(atypedef->type, asn1_type);
		return (SSL_OK);
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INT:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		return (SSL_OK);
	default:
		UNREACHABLE("__asn1_v2_type_resolve_refs");
	}
}

static int	__asn1_v2_typedef_resolve_refs(const t_asn_v2_module *asn1_module, t_asn_v2_typedef *asn1_typedef)
{
	return (__asn1_v2_type_resolve_refs(asn1_module, asn1_typedef->type));
}

/****************************************************************************/

static int	__asn1_v2_type_set_automatic_tags(const t_asn_v2_module *asn1_module, t_asn_v2_type *asn1_type);
static int	__asn1_v2_typedef_set_automatic_tags(const t_asn_v2_module *asn1_module, t_asn_v2_typedef *asn1_typedef);

static int	__asn1_v2_type_set_automatic_tags(const t_asn_v2_module *asn1_module, t_asn_v2_type *asn1_type)
{
	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET:
		;;
		bool has_tagged_element = false;
		void *content = NULL;
		t_list_next next = {0};
		while (ft_list_next_content(&asn1_type->as.constructed.elements, &next, &content)) {
			t_asn_v2_typedef *element = content;
			assert(NULL != element->type);
			if (ASN_V2_TYPE_KIND_TAGGED == element->type->kind) has_tagged_element = true;
			if (SSL_OK != __asn1_v2_typedef_set_automatic_tags(asn1_module, element)) {
				return (SSL_ERR);
			}
		}
		if (ASN_V2_TAG_MODE_AUTOMATIC == asn1_module->tag_mode && !has_tagged_element) {
			// We must tag types for all elements: implicit for SEQUENCE/SET or explicit for CHOICE.
			uint32_t tag_number = 0;
			next = (t_list_next){0};
			while (ft_list_next_content(&asn1_type->as.constructed.elements, &next, &content)) {
				t_asn_v2_typedef *element = content;
				t_asn_v2_type *element_type = element->type;
				t_asn_v2_type *wrapper_type = __asn1_v2_type_create();
				wrapper_type->kind = ASN_V2_TYPE_KIND_TAGGED;
				switch (asn1_type->kind) {
				case ASN_V2_TYPE_KIND_SEQUENCE:
				case ASN_V2_TYPE_KIND_SET:
					wrapper_type->as.tagged.tag_mode = ASN_V2_TAG_MODE_IMPLICIT;
					break;
				case ASN_V2_TYPE_KIND_CHOICE:
					wrapper_type->as.tagged.tag_mode = ASN_V2_TAG_MODE_EXPLICIT;
					break;
				default:
					UNREACHABLE("__asn1_v2_schema_parse_type");
				}
				wrapper_type->as.tagged.tag.class = ASN_V2_TAG_CLASS_CONTEXT;
				wrapper_type->as.tagged.tag.number = tag_number;
				wrapper_type->as.tagged.base_type = element_type;
				element->type = wrapper_type;
				tag_number++;
			}
		}
		return (SSL_OK);
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		return (__asn1_v2_type_set_automatic_tags(asn1_module, asn1_type->as.collection.element_type));
	case ASN_V2_TYPE_KIND_TAGGED:
		return (__asn1_v2_type_set_automatic_tags(asn1_module, asn1_type->as.tagged.base_type));
	case ASN_V2_TYPE_KIND_REF:
		SSL_LOG(ERROR, "failed to set automatic tags for `%s` asn1 type: unresolved ref found", asn1_v2_get_type_name(asn1_type->kind));
		return (SSL_ERR);
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INT:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		return (SSL_OK);
	default:
		UNREACHABLE("__asn1_v2_type_set_automatic_tags");
	}
}

static int	__asn1_v2_typedef_set_automatic_tags(const t_asn_v2_module *asn1_module, t_asn_v2_typedef *asn1_typedef)
{
	return (__asn1_v2_type_set_automatic_tags(asn1_module, asn1_typedef->type));
}

/****************************************************************************/

static int	__asn1_v2_schema_parse_typedef(const t_asn_v2_module *asn1_module, t_asn_v2_typedef **asn1_typedef, t_json_v2 *jtypedef);
static int	__asn1_v2_schema_parse_type(const t_asn_v2_module *asn1_module, t_asn_v2_type **asn1_type, t_json_v2 *jtype);
static int	__asn1_v2_schema_parse_value(t_asn_v2_value **asn1_value, t_json_v2 *jvalue);

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

static int	__asn1_v2_schema_parse_type(const t_asn_v2_module *asn1_module, t_asn_v2_type **asn1_type, t_json_v2 *jtype)
{
	assert(jtype->type == JSON_V2_TYPE_OBJECT);
	*asn1_type = NULL;

	t_asn_v2_type *atype = __asn1_v2_type_create();
	char cbuf[1024] = {0};

	t_json_v2 *jkind = NULL;
	json_v2_query_nonnull(__JQ_TYPE_KIND, jtype, &jkind);
	assert(NULL != jkind && jkind->type == JSON_V2_TYPE_STRING);
	SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TYPE_KIND, json_v2_dumpb(jkind, cbuf, sizeof(cbuf)));
	atype->kind = __asn1_v2_get_type_by_name(jkind->as.cstr);

	switch (atype->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET:
		;;
		t_json_v2 *jelements = NULL;
		if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_ELEMENTS, jtype, &jelements)) {
			assert(jelements->type == JSON_V2_TYPE_ARRAY);
			void *content = NULL;
			t_list_next next = {0};
			while (ft_list_next_content(&jelements->as.list, &next, &content)) {
				t_asn_v2_typedef *element = NULL;
				if (SSL_OK != __asn1_v2_schema_parse_typedef(asn1_module, &element, content)) {
					SSL_LOG(ERROR, "failed to parse `%s` in asn1 type: %s", __JQ_TYPE_ELEMENTS, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
					goto label_error;
				}
				ft_list_append_content(&atype->as.constructed.elements, element);
			}
		}
		break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		;;
		t_json_v2 *jelement_type = NULL;
		if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_ELEMENT_TYPE, jtype, &jelement_type)) {
			assert(jelement_type->type == JSON_V2_TYPE_OBJECT);
			SSL_LOG(TRACE, "asn1 typedef: `%s`: %s", __JQ_TYPE_ELEMENT_TYPE, json_v2_dumpb(jelement_type, cbuf, sizeof(cbuf)));
			if (SSL_OK != __asn1_v2_schema_parse_type(asn1_module, &atype->as.collection.element_type, jelement_type)) {
				SSL_LOG(ERROR, "failed to parse `%s` in asn1 type: %s", __JQ_TYPE_ELEMENT_TYPE, json_v2_dumpb(jelement_type, cbuf, sizeof(cbuf)));
				goto label_error;
			}
		}
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
		;;
		t_json_v2 *jtag_mode = NULL;
		if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_TAG_MODE, jtype, &jtag_mode)) {
			assert(jtag_mode->type == JSON_V2_TYPE_STRING);
			SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TYPE_TAG_MODE, json_v2_dumpb(jtag_mode, cbuf, sizeof(cbuf)));
			atype->as.tagged.tag_mode = __asn1_v2_get_tag_mode_by_name(jtag_mode->as.cstr);
		}
		else {
			if (ASN_V2_TAG_MODE_AUTOMATIC == asn1_module->tag_mode) {
				// Use explicit tagging mode as default.
				atype->as.tagged.tag_mode = ASN_V2_TAG_MODE_EXPLICIT;
			} else {
				// Use asn1 module's tagging mode as default.
				atype->as.tagged.tag_mode = asn1_module->tag_mode;
			}
		}

		t_json_v2 *jtag_class = NULL;
		json_v2_query_nonnull(__JQ_TYPE_TAG_CLASS, jtype, &jtag_class);
		assert(NULL != jtag_class && jtag_class->type == JSON_V2_TYPE_STRING);
		SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TYPE_TAG_CLASS, json_v2_dumpb(jtag_class, cbuf, sizeof(cbuf)));
		atype->as.tagged.tag.class = __asn1_v2_get_tag_class_by_name(jtag_class->as.cstr);

		t_json_v2 *jtag_number = NULL;
		json_v2_query_nonnull(__JQ_TYPE_TAG_NUMBER, jtype, &jtag_number);
		assert(NULL != jtag_number && jtag_number->type == JSON_V2_TYPE_NUMBER);
		SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TYPE_TAG_NUMBER, json_v2_dumpb(jtag_number, cbuf, sizeof(cbuf)));
		atype->as.tagged.tag.number = (uint32_t)bnum_to_dig_u(&jtag_number->as.number);

		t_json_v2 *jbase_type = NULL;
		json_v2_query_nonnull(__JQ_TYPE_BASE_TYPE, jtype, &jbase_type);
		assert(NULL != jbase_type && jbase_type->type == JSON_V2_TYPE_OBJECT);
		SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TYPE_BASE_TYPE, json_v2_dumpb(jbase_type, cbuf, sizeof(cbuf)));
		if (SSL_OK != __asn1_v2_schema_parse_type(asn1_module, &atype->as.tagged.base_type, jbase_type)) {
			SSL_LOG(ERROR, "failed to parse `%s` from asn1 type: %s", __JQ_TYPE_BASE_TYPE, json_v2_dumpb(jbase_type, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		break;
	case ASN_V2_TYPE_KIND_REF:
		;;
		t_json_v2 *jref_name = NULL;
		json_v2_query_nonnull(__JQ_TYPE_REF_NAME, jtype, &jref_name);
		assert(NULL != jref_name && jref_name->type == JSON_V2_TYPE_STRING);
		SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TYPE_REF_NAME, json_v2_dumpb(jref_name, cbuf, sizeof(cbuf)));
		atype->as.ref.name = ft_strdup(jref_name->as.cstr);
		break;
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INT:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		break;
	default:
		UNREACHABLE("__asn1_v2_schema_parse_type");
	}

	*asn1_type = atype;
	return (SSL_OK);

label_error:
	__asn1_v2_type_delete(atype);
	return (SSL_ERR);
}

static int	__asn1_v2_schema_parse_typedef(const t_asn_v2_module *asn1_module, t_asn_v2_typedef **asn1_typedef, t_json_v2 *jtypedef)
{
	assert(jtypedef->type == JSON_V2_TYPE_OBJECT);
	*asn1_typedef = NULL;

	t_asn_v2_typedef *atypedef = __asn1_v2_typedef_create();
	char cbuf[1024] = {0};

	t_json_v2 *jid = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPEDEF_ID, jtypedef, &jid)) {
		assert(jid->type == JSON_V2_TYPE_STRING);
		SSL_LOG(TRACE, "asn1 typedef: `%s`: %s", __JQ_TYPEDEF_ID, json_v2_dumpb(jid, cbuf, sizeof(cbuf)));
		atypedef->id = ft_strdup(jid->as.cstr);
	}

	t_json_v2 *jtype = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPEDEF_TYPE, jtypedef, &jtype)) {
		assert(jtype->type == JSON_V2_TYPE_OBJECT);
		SSL_LOG(TRACE, "asn1 typedef: `%s`: %s", __JQ_TYPEDEF_TYPE, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
		if (SSL_OK != __asn1_v2_schema_parse_type(asn1_module, &atypedef->type, jtype)) {
			SSL_LOG(ERROR, "failed to parse `%s` in asn1 typedef: %s", __JQ_TYPEDEF_TYPE, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			goto label_error;
		}
	}

	t_json_v2 *joptional = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPEDEF_OPTIONAL, jtypedef, &joptional)) {
		assert(joptional->type == JSON_V2_TYPE_BOOL);
		SSL_LOG(TRACE, "asn1 typedef: `%s`: %s", __JQ_TYPEDEF_OPTIONAL, json_v2_dumpb(joptional, cbuf, sizeof(cbuf)));
		atypedef->optional = joptional->as.boolean;
	}

	t_json_v2 *jdefault = NULL;
	if (JSON_V2_OK == json_v2_query(__JQ_TYPEDEF_DEFAULT, jtypedef, &jdefault)) {
		SSL_LOG(TRACE, "asn1 typedef: `%s`: %s", __JQ_TYPEDEF_DEFAULT, json_v2_dumpb(jdefault, cbuf, sizeof(cbuf)));
		if (SSL_OK != __asn1_v2_schema_parse_value(&atypedef->default_value, jdefault)) {
			SSL_LOG(ERROR, "failed to parse `%s` in asn1 typedef: %s", __JQ_TYPEDEF_DEFAULT, json_v2_dumpb(jdefault, cbuf, sizeof(cbuf)));
			goto label_error;
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

	t_json_v2 *jtagmode = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_SCHEMA_TAG_MODE, jschema, &jtagmode)) {
		amodule->tag_mode = __asn1_v2_get_tag_mode_by_name(jtagmode->as.cstr);
	}

	t_json_v2 *jtypedefs = NULL;
	json_v2_query_nonnull(__JQ_SCHEMA_TYPEDEFS, jschema, &jtypedefs);
	assert(NULL != jtypedefs && jtypedefs->type == JSON_V2_TYPE_OBJECT);

	const char *key = NULL;
	void *value = NULL;
	t_htbl_v2_next next = {0};
	while (ft_htbl_v2_next(&jtypedefs->as.htable, &next, &key, &value)) {
		t_asn_v2_typedef *atypedef = NULL;
		if (SSL_OK != __asn1_v2_schema_parse_typedef(amodule, &atypedef, value)) {
			SSL_LOG(ERROR, "failed to parse typedef: `%s`", key);
			goto label_error;
		}
		if (!ft_htbl_v2_set(&amodule->typedefs, key, atypedef)) {
			SSL_LOG(ERROR, "failed to set typedef: `%s`, duplicate key?", key);
			goto label_error;
		}
	}

	next = (t_htbl_v2_next){0};
	while (ft_htbl_v2_next(&amodule->typedefs, &next, &key, &value)) {
		if (SSL_OK != __asn1_v2_typedef_resolve_refs(amodule, value)) {
			SSL_LOG(ERROR, "failed to resolve refs for asn1 typedef: `%s`", key);
			goto label_error;
		}
	}

	if (ASN_V2_TAG_MODE_AUTOMATIC == amodule->tag_mode) {
		next = (t_htbl_v2_next){0};
		while (ft_htbl_v2_next(&amodule->typedefs, &next, &key, &value)) {
			if (SSL_OK != __asn1_v2_typedef_set_automatic_tags(amodule, value)) {
				SSL_LOG(ERROR, "failed to set automatic tags for asn1 typedef: `%s`", key);
				goto label_error;
			}
		}
	}

	*asn1_module = amodule;
	return (SSL_OK);

label_error:
	__asn1_v2_module_delete(amodule);
	return (SSL_ERR);
}

/****************************************************************************/

static t_der_v2_type *__der_v2_type_create(void);
static void __der_v2_type_delete(t_der_v2_type *der_type);

static inline void __der_v2_type_delete_adapter(void *p) { __der_v2_type_delete((t_der_v2_type *)p); };

static t_der_v2_type *__der_v2_type_create(void)
{
	t_der_v2_type *der_type = NULL;
	SSL_ALLOC(der_type, sizeof(t_der_v2_type));
	*der_type = (t_der_v2_type){0};
	return (der_type);
}

static void __der_v2_type_delete(t_der_v2_type *der_type)
{
	if (NULL != der_type->inner_tag) __asn1_v2_tag_delete(der_type->inner_tag);
	if (NULL != der_type->default_value) __asn1_v2_value_delete(der_type->default_value);
	ft_list_clear_all_content(&der_type->outer_tags, __asn1_v2_tag_delete_adapter);
	ft_list_clear_all_content(&der_type->alternative_tags, __asn1_v2_tag_delete_adapter);

	switch (der_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SET:
	case ASN_V2_TYPE_KIND_CHOICE:
		ft_list_clear_all_content(&der_type->as.constructed.elements, __der_v2_type_delete_adapter);
		break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		__der_v2_type_delete(der_type->as.collection.element_type);
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
	case ASN_V2_TYPE_KIND_REF:
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INT:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		break;
	}
}

static int	__asn1_v2_type_compile(t_der_v2_type **der_type, const t_asn_v2_type *asn1_type)
{
	char cbuf[1024] = {0};

	SSL_LOG(TRACE, "compiling `%s` asn1 type", asn1_v2_get_type_name(asn1_type->kind));

	*der_type = NULL;
	t_der_v2_type *dtype = __der_v2_type_create();

	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SET:
		;;
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&asn1_type->as.constructed.elements, &next, &content)) {
			t_der_v2_type *element_type = NULL;
			if (SSL_OK != asn1_v2_typedef_compile(&element_type, content)) {
				SSL_LOG(ERROR, "failed to compile `%s` asn1 type: %s", asn1_v2_get_type_name(asn1_type->kind), __asn1_v2_type_dumpb(content, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			ft_list_append_content(&dtype->as.constructed.elements, element_type);
		}
		dtype->kind = asn1_type->kind;
		dtype->inner_tag = __asn1_v2_tag_create();
		*dtype->inner_tag = asn1_v2_get_universal_tag(asn1_type->kind);
		break;
	case ASN_V2_TYPE_KIND_CHOICE:
		next = (t_list_next){0};
		while (ft_list_next_content(&asn1_type->as.constructed.elements, &next, &content)) {
			t_der_v2_type *element_type = NULL;
			if (SSL_OK != asn1_v2_typedef_compile(&element_type, content)) {
				SSL_LOG(ERROR, "failed to compile `%s` asn1 type: %s", asn1_v2_get_type_name(asn1_type->kind), __asn1_v2_type_dumpb(content, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			ft_list_append_content(&dtype->as.constructed.elements, element_type);
			t_asn_v2_tag *current_tag = __asn1_v2_tag_create();
			*current_tag = asn1_v2_get_universal_tag(element_type->kind);
			ft_list_append_content(&dtype->alternative_tags, current_tag);
		}
		dtype->kind = asn1_type->kind;
		// CHOICE has no universal tag of its own. Its inner tag is a choice from a set of its alternatives.
		dtype->inner_tag = NULL;
		break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		;;
		if (SSL_OK != __asn1_v2_type_compile(&dtype->as.collection.element_type, asn1_type->as.collection.element_type)) {
			SSL_LOG(ERROR, "failed to compile `%s` asn1 type: %s", asn1_v2_get_type_name(asn1_type->kind), __asn1_v2_type_dumpb(asn1_type, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		dtype->kind = asn1_type->kind;
		dtype->inner_tag = __asn1_v2_tag_create();
		*dtype->inner_tag = asn1_v2_get_universal_tag(asn1_type->kind);
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
		;;
		switch (asn1_type->as.tagged.tag_mode) {
		case ASN_V2_TAG_MODE_EXPLICIT:
			;;
			// Explicit tag wraps the tag of its base type. Flatten nested structure of base types to a single type.
			t_der_v2_type *wrapped_dtype = NULL;
			if (SSL_OK != __asn1_v2_type_compile(&wrapped_dtype, asn1_type->as.tagged.base_type)) {
				SSL_LOG(ERROR, "failed to compile `%s` asn1 type: %s", asn1_v2_get_type_name(asn1_type->kind), __asn1_v2_type_dumpb(asn1_type, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			dtype->kind = wrapped_dtype->kind;
			// Get current tag from this type.
			t_asn_v2_tag *current_tag = __asn1_v2_tag_create();
			__asn1_v2_tag_copy(&asn1_type->as.tagged.tag, current_tag);
			// Copy current tag as outermost tag: this tag will be the first tag in the list.
			ft_list_append_content(&dtype->outer_tags, current_tag);
			// Copy outer tags from the base type: these tags will be placed after current/outermost tag.
			// Resulting outer_tags list: [ tag1 (current tag), tag2 (from base), tag3 (from base of the base), ... ]
			ft_list_copy_all_content(&wrapped_dtype->outer_tags, &dtype->outer_tags, __asn1_v2_tag_copy_adapter);
			// Inner tag is propagated from the bottom of the base type chain.
			dtype->inner_tag = __asn1_v2_tag_create();
			*dtype->inner_tag = asn1_v2_get_universal_tag(wrapped_dtype->kind);
			// Copy alternative tags from the base type (for types such as CHOICE).
			ft_list_copy_all_content(&wrapped_dtype->alternative_tags, &dtype->alternative_tags, __asn1_v2_tag_copy_adapter);
			__der_v2_type_delete(wrapped_dtype);
			break;
		case ASN_V2_TAG_MODE_IMPLICIT:
			;;
			// Implicit tag overwrites the tags of its base type. Flatten nested structure of base types to a single type.
			t_der_v2_type *overwritten_dtype = NULL;
			if (SSL_OK != __asn1_v2_type_compile(&overwritten_dtype, asn1_type->as.tagged.base_type)) {
				SSL_LOG(ERROR, "failed to compile `%s` asn1 type: %s", asn1_v2_get_type_name(asn1_type->kind), __asn1_v2_type_dumpb(asn1_type, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			dtype->kind = overwritten_dtype->kind;
			__der_v2_type_delete(overwritten_dtype);
			if (ASN_V2_TYPE_KIND_CHOICE == dtype->kind) {
				SSL_LOG(ERROR, "cannot use implicit tag for `%s` asn1 type", asn1_v2_get_type_name(ASN_V2_TYPE_KIND_CHOICE));
				goto label_error;
			}
			// There is no outer tags for a type with implicit tag.
			assert(dtype->outer_tags.size == 0);
			// Implicit tag becomes the inner tag.
			dtype->inner_tag = __asn1_v2_tag_create();
			*dtype->inner_tag = asn1_type->as.tagged.tag;
			break;
		case ASN_V2_TAG_MODE_AUTOMATIC:
			UNREACHABLE("__asn1_v2_type_compile: ASN_V2_TAG_MODE_AUTOMATIC");
		}
		break;
	case ASN_V2_TYPE_KIND_REF:
		SSL_LOG(ERROR, "cannot compile asn1 type: unresolved typedef reference");
		goto label_error;
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INT:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		dtype->kind = asn1_type->kind;
		dtype->inner_tag = __asn1_v2_tag_create();
		*dtype->inner_tag = asn1_v2_get_universal_tag(asn1_type->kind);
	}

	*der_type = dtype;
	return (SSL_OK);

label_error:
	__der_v2_type_delete(dtype);
	return (SSL_ERR);
}

int	asn1_v2_typedef_compile(t_der_v2_type **der_type, const t_asn_v2_typedef *asn1_typedef)
{
	if (NULL == der_type || NULL == asn1_typedef) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	char cbuf[1024] = {0};

	if (NULL != asn1_typedef->id) {
		SSL_LOG(TRACE, "compiling `%s` asn1 typedef", asn1_typedef->id);
	} else {
		SSL_LOG(TRACE, "compiling asn1 typedef: %s", __asn1_v2_typedef_dumpb(asn1_typedef, cbuf, sizeof(cbuf)));
	}

	t_der_v2_type *dtype = NULL;
	if (SSL_OK != __asn1_v2_type_compile(&dtype, asn1_typedef->type)) {
		SSL_LOG(ERROR, "failed to compile asn1 typedef");
		return (SSL_ERR);
	}
	if (NULL != asn1_typedef->default_value) {
		__asn1_v2_value_copy(asn1_typedef->default_value, dtype->default_value );
	}
	dtype->optional = asn1_typedef->optional;

	*der_type = dtype;
	return (SSL_OK);
}
