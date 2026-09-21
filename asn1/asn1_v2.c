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

#define ASN_V2_TAG_CLASS_NAME_UNIVERSAL     "UNIVERSAL"
#define ASN_V2_TAG_CLASS_NAME_APPLICATION   "APPLICATION"
#define ASN_V2_TAG_CLASS_NAME_CONTEXT       "CONTEXT-SPECIFIC"
#define ASN_V2_TAG_CLASS_NAME_PRIVATE       "PRIVATE"

#define ASN_V2_TAG_MODE_NAME_EXPLICIT       "EXPLICIT"
#define ASN_V2_TAG_MODE_NAME_IMPLICIT       "IMPLICIT"
#define ASN_V2_TAG_MODE_NAME_AUTOMATIC      "AUTOMATIC"

#define ASN_V2_TYPE_NAME_NULL               "NULL"
#define ASN_V2_TYPE_NAME_BOOLEAN            "BOOLEAN"
#define ASN_V2_TYPE_NAME_INTEGER            "INTEGER"
#define ASN_V2_TYPE_NAME_BIT_STRING         "BIT STRING"
#define ASN_V2_TYPE_NAME_OCTET_STRING       "OCTET STRING"
#define ASN_V2_TYPE_NAME_IA5_STRING         "IA5 STRING"
#define ASN_V2_TYPE_NAME_UTF8_STRING        "UTF8 STRING"
#define ASN_V2_TYPE_NAME_PRINTABLE_STRING   "PRINTABLE STRING"
#define ASN_V2_TYPE_NAME_OBJECT_ID          "OBJECT IDENTIFIER"
#define ASN_V2_TYPE_NAME_OBJECT_DESCR       "OBJECT DESCRIPTOR"
#define ASN_V2_TYPE_NAME_SEQUENCE           "SEQUENCE"
#define ASN_V2_TYPE_NAME_SEQUENCE_OF        "SEQUENCE OF"
#define ASN_V2_TYPE_NAME_SET                "SET"
#define ASN_V2_TYPE_NAME_SET_OF             "SET OF"
#define ASN_V2_TYPE_NAME_CHOICE             "CHOICE"
#define ASN_V2_TYPE_NAME_TAGGED             "TAGGED"
#define ASN_V2_TYPE_NAME_ANY                "ANY"

#define ASN_V2_CONSTRAINT_TYPE_NAME_RANGE   "RANGE"
#define ASN_V2_CONSTRAINT_TYPE_NAME_SIZE    "SIZE"

#define ASN_V2_VALUE_TYPE_NAME_NULL         "NULL"
#define ASN_V2_VALUE_TYPE_NAME_BOOLEAN      "BOOLEAN"
#define ASN_V2_VALUE_TYPE_NAME_NUMBER       "NUMBER"
#define ASN_V2_VALUE_TYPE_NAME_CSTRING      "CSTRING"
#define ASN_V2_VALUE_TYPE_NAME_BSTRING      "BSTRING"
#define ASN_V2_VALUE_TYPE_NAME_OSTRING      "OSTRING"
#define ASN_V2_VALUE_TYPE_NAME_LIST         "LIST"
#define ASN_V2_VALUE_TYPE_NAME_CHOICE       "CHOICE"
#define ASN_V2_VALUE_TYPE_NAME_ANY          "ANY"

/****************************************************************************/

#define __SCHEMA_TAG_CLASS_NAME_UNIVERSAL   ASN_V2_TAG_CLASS_NAME_UNIVERSAL
#define __SCHEMA_TAG_CLASS_NAME_APPLICATION ASN_V2_TAG_CLASS_NAME_APPLICATION
#define __SCHEMA_TAG_CLASS_NAME_CONTEXT     ASN_V2_TAG_CLASS_NAME_CONTEXT
#define __SCHEMA_TAG_CLASS_NAME_PRIVATE     ASN_V2_TAG_CLASS_NAME_PRIVATE

#define __SCHEMA_TAG_MODE_NAME_EXPLICIT     ASN_V2_TAG_MODE_NAME_EXPLICIT
#define __SCHEMA_TAG_MODE_NAME_IMPLICIT     ASN_V2_TAG_MODE_NAME_IMPLICIT
#define __SCHEMA_TAG_MODE_NAME_AUTOMATIC    ASN_V2_TAG_MODE_NAME_AUTOMATIC

#define __SCHEMA_TYPE_NAME_NULL             ASN_V2_TYPE_NAME_NULL
#define __SCHEMA_TYPE_NAME_BOOLEAN          ASN_V2_TYPE_NAME_BOOLEAN
#define __SCHEMA_TYPE_NAME_INTEGER          ASN_V2_TYPE_NAME_INTEGER
#define __SCHEMA_TYPE_NAME_BIT_STRING       ASN_V2_TYPE_NAME_BIT_STRING
#define __SCHEMA_TYPE_NAME_OCTET_STRING     ASN_V2_TYPE_NAME_OCTET_STRING
#define __SCHEMA_TYPE_NAME_IA5_STRING       ASN_V2_TYPE_NAME_IA5_STRING
#define __SCHEMA_TYPE_NAME_UTF8_STRING      ASN_V2_TYPE_NAME_UTF8_STRING
#define __SCHEMA_TYPE_NAME_PRINTABLE_STRING ASN_V2_TYPE_NAME_PRINTABLE_STRING
#define __SCHEMA_TYPE_NAME_OBJECT_ID        ASN_V2_TYPE_NAME_OBJECT_ID
#define __SCHEMA_TYPE_NAME_OBJECT_DESCR     ASN_V2_TYPE_NAME_OBJECT_DESCR
#define __SCHEMA_TYPE_NAME_SEQUENCE         ASN_V2_TYPE_NAME_SEQUENCE
#define __SCHEMA_TYPE_NAME_SEQUENCE_OF      ASN_V2_TYPE_NAME_SEQUENCE_OF
#define __SCHEMA_TYPE_NAME_SET              ASN_V2_TYPE_NAME_SET
#define __SCHEMA_TYPE_NAME_SET_OF           ASN_V2_TYPE_NAME_SET_OF
#define __SCHEMA_TYPE_NAME_CHOICE           ASN_V2_TYPE_NAME_CHOICE
#define __SCHEMA_TYPE_NAME_TAGGED           ASN_V2_TYPE_NAME_TAGGED
#define __SCHEMA_TYPE_NAME_ANY              ASN_V2_TYPE_NAME_ANY

#define __SCHEMA_CONSTRAINT_TYPE_NAME_RANGE ASN_V2_CONSTRAINT_TYPE_NAME_RANGE
#define __SCHEMA_CONSTRAINT_TYPE_NAME_SIZE  ASN_V2_CONSTRAINT_TYPE_NAME_SIZE

#define __JQ_SCHEMA_TAG_MODE        ".tagMode"
#define __JQ_SCHEMA_TYPES           ".types"

#define __JQ_COMPONENT_ID 		    ".id"
#define __JQ_COMPONENT_TYPE 	    ".type"
#define __JQ_COMPONENT_OPTIONAL	    ".optional"
#define __JQ_COMPONENT_DEFAULT	    ".default"

#define __JQ_TYPE_KIND			    ".kind"
#define __JQ_TYPE_TAG_MODE		    ".tagMode"
#define __JQ_TYPE_TAG_CLASS		    ".tagClass"
#define __JQ_TYPE_TAG_NUMBER	    ".tagNumber"
#define __JQ_TYPE_BASE_TYPE		    ".baseType"
#define __JQ_TYPE_DEFINED_BY_ID		".definedById"
#define __JQ_TYPE_COMPONENT_TYPE	".componentType"
#define __JQ_TYPE_COMPONENTS		".components"

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
	case ASN_V2_TYPE_KIND_ANY:              return ASN_V2_TYPE_NAME_ANY;
	default:                                return "unknown";
	}
}

const char *asn1_v2_get_value_type_name(t_asn_v2_value_type type)
{
	switch (type) {
	case ASN_V2_VALUE_TYPE_NULL:        return ASN_V2_VALUE_TYPE_NAME_NULL;
	case ASN_V2_VALUE_TYPE_BOOLEAN:     return ASN_V2_VALUE_TYPE_NAME_BOOLEAN;
	case ASN_V2_VALUE_TYPE_NUMBER:      return ASN_V2_VALUE_TYPE_NAME_NUMBER;
	case ASN_V2_VALUE_TYPE_CSTRING:     return ASN_V2_VALUE_TYPE_NAME_CSTRING;
	case ASN_V2_VALUE_TYPE_BSTRING:     return ASN_V2_VALUE_TYPE_NAME_BSTRING;
	case ASN_V2_VALUE_TYPE_OSTRING:     return ASN_V2_VALUE_TYPE_NAME_OSTRING;
	case ASN_V2_VALUE_TYPE_LIST:        return ASN_V2_VALUE_TYPE_NAME_LIST;
	case ASN_V2_VALUE_TYPE_CHOICE:      return ASN_V2_VALUE_TYPE_NAME_CHOICE;
	case ASN_V2_VALUE_TYPE_ANY:         return ASN_V2_VALUE_TYPE_NAME_ANY;
	default:                            return "unknown";
	}
}

const char *asn1_v2_get_tag_class_name(t_asn_v2_tag_class tag_class)
{
	switch (tag_class) {
	case ASN_V2_TAG_CLASS_UNIVERSAL:	        return ASN_V2_TAG_CLASS_NAME_UNIVERSAL;
	case ASN_V2_TAG_CLASS_APPLICATION:	        return ASN_V2_TAG_CLASS_NAME_APPLICATION;
	case ASN_V2_TAG_CLASS_CONTEXT_SPECIFIC:		return ASN_V2_TAG_CLASS_NAME_CONTEXT;
	case ASN_V2_TAG_CLASS_PRIVATE:		        return ASN_V2_TAG_CLASS_NAME_PRIVATE;
	default:							        return "unknown";
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

#define __ASN_V2_UNKNOWN_NAME 9999

static t_asn_v2_constraint_type __asn1_v2_schema_get_constraint_type_by_name(const char *name)
{
	assert(NULL != name);

	if (ft_streq(name, __SCHEMA_CONSTRAINT_TYPE_NAME_RANGE))    return (ASN_V2_CONSTRAINT_TYPE_RANGE);
	if (ft_streq(name, __SCHEMA_CONSTRAINT_TYPE_NAME_SIZE))	    return (ASN_V2_CONSTRAINT_TYPE_SIZE);

	return (__ASN_V2_UNKNOWN_NAME);
}

static t_asn_v2_type_kind __asn1_v2_schema_get_type_by_name(const char *name)
{
	assert(NULL != name);

	if (ft_streq(name, __SCHEMA_TYPE_NAME_INTEGER))			    return (ASN_V2_TYPE_KIND_INT);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_BOOLEAN))			    return (ASN_V2_TYPE_KIND_BOOLEAN);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_BIT_STRING))		    return (ASN_V2_TYPE_KIND_BIT_STRING);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_OCTET_STRING))		return (ASN_V2_TYPE_KIND_OCTET_STRING);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_IA5_STRING))		    return (ASN_V2_TYPE_KIND_IA5_STRING);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_UTF8_STRING))		    return (ASN_V2_TYPE_KIND_UTF8_STRING);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_PRINTABLE_STRING))	return (ASN_V2_TYPE_KIND_PRINTABLE_STRING);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_NULL))				return (ASN_V2_TYPE_KIND_NULL);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_OBJECT_ID))			return (ASN_V2_TYPE_KIND_OBJECT_ID);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_OBJECT_DESCR))		return (ASN_V2_TYPE_KIND_OBJECT_DESCR);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_SEQUENCE))			return (ASN_V2_TYPE_KIND_SEQUENCE);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_SEQUENCE_OF))		    return (ASN_V2_TYPE_KIND_SEQUENCE_OF);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_SET))				    return (ASN_V2_TYPE_KIND_SET);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_SET_OF))			    return (ASN_V2_TYPE_KIND_SET_OF);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_CHOICE))			    return (ASN_V2_TYPE_KIND_CHOICE);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_TAGGED))			    return (ASN_V2_TYPE_KIND_TAGGED);
	if (ft_streq(name, __SCHEMA_TYPE_NAME_ANY))			        return (ASN_V2_TYPE_KIND_ANY);

	return (__ASN_V2_UNKNOWN_NAME);
}

static t_asn_v2_tag_mode __asn1_v2_schema_get_tag_mode_by_name(const char *name)
{
	assert(NULL != name);

	if (ft_streq(name, __SCHEMA_TAG_MODE_NAME_EXPLICIT))		return (ASN_V2_TAG_MODE_EXPLICIT);
	if (ft_streq(name, __SCHEMA_TAG_MODE_NAME_IMPLICIT))		return (ASN_V2_TAG_MODE_IMPLICIT);
	if (ft_streq(name, __SCHEMA_TAG_MODE_NAME_AUTOMATIC))		return (ASN_V2_TAG_MODE_AUTOMATIC);

	return (__ASN_V2_UNKNOWN_NAME);
}

static t_asn_v2_tag_class __asn1_v2_schema_get_tag_class_by_name(const char *name)
{
	assert(NULL != name);

	if (ft_streq(name, __SCHEMA_TAG_CLASS_NAME_UNIVERSAL))	    return (ASN_V2_TAG_CLASS_UNIVERSAL);
	if (ft_streq(name, __SCHEMA_TAG_CLASS_NAME_APPLICATION))	return (ASN_V2_TAG_CLASS_APPLICATION);
	if (ft_streq(name, __SCHEMA_TAG_CLASS_NAME_CONTEXT))		return (ASN_V2_TAG_CLASS_CONTEXT_SPECIFIC);
	if (ft_streq(name, __SCHEMA_TAG_CLASS_NAME_PRIVATE))		return (ASN_V2_TAG_CLASS_PRIVATE);

	return (__ASN_V2_UNKNOWN_NAME);
}

/****************************************************************************/

static t_asn_v2_tag __asn1_v2_get_universal_tag(t_asn_v2_type_kind type)
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
	case ASN_V2_TYPE_KIND_ANY:
	    return (t_asn_v2_tag){0};
	}
}

/****************************************************************************/

int __asn1_v2_schema_validate_constraint_type_name(const char *name)
{
	if (NULL == name) return (SSL_ERR);
	if (__asn1_v2_schema_get_constraint_type_by_name(name) == __ASN_V2_UNKNOWN_NAME) return (SSL_ERR);
	return (SSL_OK);
}

int __asn1_v2_schema_validate_tag_mode_name(const char *name)
{
	if (NULL == name) return (SSL_ERR);
	if (__asn1_v2_schema_get_tag_mode_by_name(name) == __ASN_V2_UNKNOWN_NAME) return (SSL_ERR);
	return (SSL_OK);
}

int __asn1_v2_schema_validate_tag_class_name(const char *name)
{
	if (NULL == name) return (SSL_ERR);
	if (__asn1_v2_schema_get_tag_class_by_name(name) == __ASN_V2_UNKNOWN_NAME) return (SSL_ERR);
	return (SSL_OK);
}

int __asn1_v2_schema_validate_type_name(const char *name)
{
	if (NULL == name) return (SSL_ERR);
	if (__asn1_v2_schema_get_type_by_name(name) == __ASN_V2_UNKNOWN_NAME) return (SSL_ERR);
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
	case ASN_V2_TYPE_KIND_ANY:
	    return (SSL_OK);
	default:
		return (SSL_ERR);
	}
}

static int __asn1_v2_schema_validate_type(const t_json_v2 *jschema, const t_json_v2 *jtype);
static int __asn1_v2_schema_validate_component(const t_json_v2 *jschema, const t_json_v2 *jcomponent);

static int __asn1_v2_schema_validate_type(const t_json_v2 *jschema, const t_json_v2 *jtype)
{
    char cbuf[1024];

	if (jtype->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid asn1 component: expected json object but got json %s: %s", json_v2_get_type_name(jtype->type), json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
		return (SSL_ERR);
	}

	const t_json_v2 *jkind = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_KIND, jtype, &jkind)) {
		SSL_LOG(ERROR, "invalid asn1 type: expected `%s` key: %s", __JQ_TYPE_KIND, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
		return (SSL_ERR);
	}
	else {
		if (jkind->type != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid asn1 type: expected `%s` to be a json string but got json %s: %s", __JQ_TYPE_KIND, json_v2_get_type_name(jkind->type), json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		if (SSL_OK != __asn1_v2_schema_validate_type_name(jkind->as.cstring)) {
		    SSL_LOG(TRACE, "unknown asn1 type: `%s`: %s", jkind->as.cstring, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
		    // Check if the type name refers to a type within a module.
            const t_json_v2 *jref_type = NULL;
            char *query = ft_strjoin_multi(3, __JQ_SCHEMA_TYPES, ".", jkind->as.cstring);
            int ret = json_v2_query_nonnull(query, jschema, &jref_type);
            SSL_FREE(query);
            if (JSON_V2_OK != ret || jref_type->type != JSON_V2_TYPE_OBJECT) {
                SSL_LOG(ERROR, "invalid asn1 type: bad type ref: `%s`: %s", jkind->as.cstring, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			    return (SSL_ERR);
            }
		}
	}
	t_asn_v2_type_kind asn1_type = __asn1_v2_schema_get_type_by_name(jkind->as.cstring);

	switch (asn1_type) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET:
		;;
		const t_json_v2 *jcomponents = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_COMPONENTS, jtype, &jcomponents)) {
			SSL_LOG(ERROR, "asn1 component: no `%s` key specified: %s", __JQ_TYPE_COMPONENTS, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		else {
			void *content = NULL;
			t_list_next next = {0};
			while (ft_list_next_content(&jcomponents->as.list, &next, &content)) {
				if (SSL_OK != __asn1_v2_schema_validate_component(jschema, content)) {
					SSL_LOG(ERROR, "invalid asn1 component: `%s` contains invalid component: %s", __JQ_TYPE_COMPONENTS, json_v2_dumpb(content, cbuf, sizeof(cbuf)));
					return (SSL_ERR);
				}
			}
		}
		break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		;;
		const t_json_v2 *jelement_type = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_COMPONENT_TYPE, jtype, &jelement_type)) {
			SSL_LOG(ERROR, "asn1 type: no `%s` key specified: %s", __JQ_TYPE_COMPONENT_TYPE, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		else {
			if (SSL_OK != __asn1_v2_schema_validate_type(jschema, jelement_type)) {
				SSL_LOG(ERROR, "invalid asn1 type: invalid `%s`: %s", __JQ_TYPE_COMPONENT_TYPE, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
				return (SSL_ERR);
			}
		}
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
		;;
		const t_json_v2 *jtag_mode = NULL;
		if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_TAG_MODE, jtype, &jtag_mode)) {
			if (jtag_mode->type != JSON_V2_TYPE_STRING) {
				SSL_LOG(ERROR, "invalid asn1 component: expected `%s` to be a json string but got json %s: %s", __JQ_TYPE_TAG_MODE, json_v2_get_type_name(jtag_mode->type), json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
				return (SSL_ERR);
			}
			if (SSL_OK != __asn1_v2_schema_validate_tag_mode_name(jtag_mode->as.cstring)) {
				SSL_LOG(ERROR, "invalid asn1 type: bad tag mode `%s`: %s", jtag_mode->as.cstring, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
				return (SSL_ERR);
			}
		}
		const t_json_v2 *jtag_class = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_TAG_CLASS, jtype, &jtag_class)) {
			SSL_LOG(ERROR, "invalid asn1 component: expected `%s` key: %s", __JQ_TYPE_TAG_CLASS, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		else {
			if (jtag_class->type != JSON_V2_TYPE_STRING) {
				SSL_LOG(ERROR, "invalid asn1 component: expected `%s` to be a json string but got json %s: %s", __JQ_TYPE_TAG_CLASS, json_v2_get_type_name(jtag_class->type), json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
				return (SSL_ERR);
			}
			if (SSL_OK != __asn1_v2_schema_validate_tag_class_name(jtag_class->as.cstring)) {
				SSL_LOG(ERROR, "invalid asn1 type: bad tag class: `%s`: %s", jtag_class->as.cstring, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
				return (SSL_ERR);
			}
		}
		const t_json_v2 *jtag_number = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_TAG_NUMBER, jtype, &jtag_number)) {
			SSL_LOG(ERROR, "invalid asn1 component: expected `%s` key: %s", __JQ_TYPE_TAG_NUMBER, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		else {
			if (jtag_number->type != JSON_V2_TYPE_NUMBER) {
				SSL_LOG(ERROR, "invalid asn1 component: expected `%s` to be a json number but got json %s: %s", __JQ_TYPE_TAG_NUMBER, json_v2_get_type_name(jtag_number->type), json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
				return (SSL_ERR);
			}
		}
		const t_json_v2 *jbase_type = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_BASE_TYPE, jtype, &jbase_type)) {
			SSL_LOG(ERROR, "invalid asn1 component: expected `%s` key: %s", __JQ_TYPE_BASE_TYPE, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		else {
			if (SSL_OK != __asn1_v2_schema_validate_type(jschema, jbase_type)) {
				SSL_LOG(ERROR, "invalid asn1 component: bad type: `%s`: %s", __JQ_TYPE_BASE_TYPE, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
				return (SSL_ERR);
			}
		}
		break;
	case ASN_V2_TYPE_KIND_ANY:
    	;;
    	const t_json_v2 *jdefined_by_id = NULL;
    	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_DEFINED_BY_ID, jtype, &jdefined_by_id)) {
    		if (jdefined_by_id->type != JSON_V2_TYPE_STRING) {
    			SSL_LOG(ERROR, "invalid asn1 component: expected `%s` to be a json string but got json %s: %s", __JQ_TYPE_DEFINED_BY_ID, json_v2_get_type_name(jdefined_by_id->type), json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
    			return (SSL_ERR);
    		}
    	}
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

	return (SSL_OK);
}

static int __asn1_v2_schema_validate_component(const t_json_v2 *jschema, const t_json_v2 *jcomponent)
{
    char cbuf[1024];

	if (jcomponent->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid asn1 component: expected a json object but got json %s", json_v2_get_type_name(jcomponent->type));
		return (SSL_ERR);
	}

	const t_json_v2 *jid = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_COMPONENT_ID, jcomponent, &jid)) {
		SSL_LOG(TRACE, "no asn1 `%s` key specified: %s", __JQ_COMPONENT_ID, json_v2_dumpb(jcomponent, cbuf, sizeof(cbuf)));
	}
	else {
		if (jid->type != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid asn1 component: expected `%s` to be a json string but got json %s: %s", __JQ_COMPONENT_ID, json_v2_get_type_name(jid->type), json_v2_dumpb(jcomponent, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
	}

	const t_json_v2 *jtype = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_COMPONENT_TYPE, jcomponent, &jtype)) {
		SSL_LOG(TRACE, "no asn1 `%s` key specified ", __JQ_COMPONENT_TYPE);
	}
	else {
		if (SSL_OK != __asn1_v2_schema_validate_type(jschema, jtype)) {
			SSL_LOG(ERROR, "invalid asn1 component: invalid `%s`: %s", __JQ_COMPONENT_TYPE, json_v2_dumpb(jcomponent, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
	}

	const t_json_v2 *joptional = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_COMPONENT_OPTIONAL, jcomponent, &joptional)) {
		SSL_LOG(TRACE, "asn1 component: no `%s` key specified: %s", __JQ_COMPONENT_OPTIONAL, json_v2_dumpb(jcomponent, cbuf, sizeof(cbuf)));
	}
	else {
		if (joptional->type != JSON_V2_TYPE_BOOL) {
			SSL_LOG(ERROR, "invalid asn1 component: expected `%s` to be a json boolean but got json %s", __JQ_COMPONENT_OPTIONAL, json_v2_get_type_name(joptional->type));
			return (SSL_ERR);
		}
	}

	const t_json_v2 *jdefault = NULL;
	if (JSON_V2_OK == json_v2_query(__JQ_COMPONENT_DEFAULT, jcomponent, &jdefault)) {
		SSL_LOG(TRACE, "asn1 component: `%s` key is set, value type: %s", __JQ_COMPONENT_DEFAULT, json_v2_get_type_name(jdefault->type));
	}

	return (SSL_OK);
}

int	asn1_v2_schema_validate(const t_json_v2 *jschema)
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

	const t_json_v2 *jtagmode = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_SCHEMA_TAG_MODE, jschema, &jtagmode)) {
		SSL_LOG(TRACE, "no default asn1 tagging mode specified");
	}
	else {
		if (jtagmode->type != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid json schema: expected `%s` to be a json string but got %s", __JQ_SCHEMA_TAG_MODE, json_v2_get_type_name(jtagmode->type));
			return (SSL_ERR);
		}
		if (SSL_OK != __asn1_v2_schema_validate_tag_mode_name(jtagmode->as.cstring)) {
			SSL_LOG(ERROR, "invalid `%s`: %s", __JQ_SCHEMA_TAG_MODE, jtagmode->as.cstring);
			return (SSL_ERR);
		}
	}

	const t_json_v2 *jtypes = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_SCHEMA_TYPES, jschema, &jtypes)) {
		SSL_LOG(ERROR, "invalid json schema: expected `%s` key", __JQ_SCHEMA_TYPES);
		return (SSL_ERR);
	}
	if (jtypes->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid json schema: expected `%s` to be a json object but got json %s", __JQ_SCHEMA_TYPES, json_v2_get_type_name(jtypes->type));
		return (SSL_ERR);
	}

	const char *key = NULL;
	void *value = NULL;
	t_htbl_v2_next next = {0};
	while (ft_htbl_v2_next(&jtypes->as.htable, &next, &key, &value)) {
		if (SSL_OK != __asn1_v2_schema_validate_type(jschema, value)) {
		SSL_LOG(ERROR, "invalid json schema: invalid asn1 type: `%s`", key);
			return (SSL_ERR);
		}
	}
	return (SSL_OK);
}

/****************************************************************************/

static char *__asn1_v2_tag_dumps(const t_asn_v2_tag *asn1_tag);
static char *__asn1_v2_constraint_dumps(const t_asn_v2_constraint *asn1_constraint);
static char *__asn1_v2_value_dumps(const t_asn_v2_value *asn1_value);
static char *__asn1_v2_type_dumps(const t_asn_v2_type *asn1_type);
static char *__asn1_v2_component_dumps(const t_asn_v2_component *asn1_component);

static char *__asn1_v2_tag_dumpb(const t_asn_v2_tag *asn1_tag, char *buf, size_t size);
static char *__asn1_v2_value_dumpb(const t_asn_v2_value *asn1_value, char *buf, size_t size);
static char *__asn1_v2_type_dumpb(const t_asn_v2_type *asn1_type, char *buf, size_t size);
static char *__asn1_v2_component_dumpb(const t_asn_v2_component *asn1_component, char *buf, size_t size);

static char *__asn1_v2_tag_dumps(const t_asn_v2_tag *asn1_tag)
{
	if (NULL == asn1_tag) return ft_strdup("null");

	char *dumps = NULL;
	ft_sprintf(&dumps, "{\"class\":\"%s\",\"number\":%d,\"constructed\":%s}",
		asn1_v2_get_tag_class_name(asn1_tag->class),
		asn1_tag->number,
		asn1_tag->constructed ? "true" : "false"
	);
	return (dumps);
}

static char *__asn1_v2_constraint_dumps(const t_asn_v2_constraint *asn1_constraint)
{
    if (NULL == asn1_constraint) return ft_strdup("null");

	t_ostring ostring = {0};

	switch (asn1_constraint->type) {
	case ASN_V2_CONSTRAINT_TYPE_RANGE:
	    ft_ostr_append_cstr(&ostring, "{\"range:\"{");
	    if (NULL != asn1_constraint->as.range.min) {
	        char *min_dumps = bnum_to_dec(asn1_constraint->as.range.min);
			ft_ostr_appendf(&ostring, "\"min\":%s", min_dumps);
			SSL_FREE(min_dumps);
		} else {
		    ft_ostr_append_cstr(&ostring, "\"min\":null");
		}
	    if (NULL != asn1_constraint->as.range.max) {
			char *max_dumps = bnum_to_dec(asn1_constraint->as.range.max);
			ft_ostr_appendf(&ostring, ",\"max\":%s", max_dumps);
			SSL_FREE(max_dumps);
		} else {
		    ft_ostr_append_cstr(&ostring, ",\"max\":null");
		}
	    ft_ostr_append_cstr(&ostring, "}}");
		break;
	case ASN_V2_CONSTRAINT_TYPE_SIZE:
	    ft_ostr_appendf(&ostring, "{\"size:\"{\"min\":%zu,\"min\":%zu}}", asn1_constraint->as.size.min, asn1_constraint->as.size.max);
		break;
	}
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

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
		return ft_strjoin_multi(3, "\"", asn1_value->as.cstring, "\"");
	case ASN_V2_VALUE_TYPE_OSTRING:
	case ASN_V2_VALUE_TYPE_BSTRING:
		;;
		char *ostring_dumps = NULL;
		ft_sprintf(&ostring_dumps, "{\"octets\":\"<%p,size=%zu>\"}", &asn1_value->as.ostring, asn1_value->as.ostring.size);
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
	case ASN_V2_VALUE_TYPE_CHOICE:
    	;;
    	char *choice_dumps = NULL;
        char *alt_dumps = __asn1_v2_value_dumps(asn1_value->as.choice.value);
        if (NULL != asn1_value->as.choice.id) {
           	ft_sprintf(&choice_dumps, "{\"choice\":{\"id\":\"%s\",\"value\":%s}}", asn1_value->as.choice.id, alt_dumps);
        } else {
           	ft_sprintf(&choice_dumps, "{\"choice\":{\"id\":null,\"value\":%s}}", alt_dumps);
        }
        SSL_FREE(alt_dumps);
    	return (choice_dumps);
	case ASN_V2_VALUE_TYPE_ANY:
	    ;;
    	char *any_dumps = NULL;
        if (NULL != asn1_value->as.any.defined_by_id) {
           	ft_sprintf(&any_dumps, "{\"any\":{\"defined_by_id\":\"%s\",\"octets\":\"<%p,size=%zu>\"}}", asn1_value->as.any.defined_by_id, &asn1_value->as.any.octets, asn1_value->as.any.octets.size);
        } else {
           	ft_sprintf(&any_dumps, "{\"any\":{\"defined_by_id\":null,\"octets\":\"<%p,size=%zu>\"}}", &asn1_value->as.any.octets, asn1_value->as.any.octets.size);
        }
    	return (any_dumps);
	}
}

static char *__asn1_v2_type_dumps(const t_asn_v2_type *asn1_type)
{
	if (NULL == asn1_type) return ft_strdup("null");

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);

	ft_ostr_appendf(&ostring, "{\"kind\":\"%s\"", asn1_v2_get_type_name(asn1_type->kind));

	ft_ostr_append_cstr(&ostring, ",\"constraints\":[");
    t_list_next next = {0};
    void *content = NULL;
    size_t commas = 0;
    while (ft_list_next_content(&asn1_type->constraints, &next, &content)) {
        if (commas++) ft_ostr_append_cstr(&ostring, ",");
        char *constraint_dumps = __asn1_v2_constraint_dumps(content);
        ft_ostr_append_cstr(&ostring, constraint_dumps);
        SSL_FREE(constraint_dumps);
    }
    ft_ostr_append_cstr(&ostring, "]");

	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET:
		next = (t_list_next){0};
		content = NULL;
		commas = 0;
		ft_ostr_append_cstr(&ostring, ",\"components\":[");
		while (ft_list_next_content(&asn1_type->as.composite.elements, &next, &content)) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = __asn1_v2_component_dumps(content);
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
		dumps = __asn1_v2_tag_dumps(&asn1_type->as.tagged.tag);
		ft_ostr_appendf(&ostring, ",\"tag\":%s", dumps);
		SSL_FREE(dumps);
		dumps = __asn1_v2_type_dumps(asn1_type->as.tagged.base_type);
		ft_ostr_appendf(&ostring, ",\"base_type\":%s", dumps);
		SSL_FREE(dumps);
		break;
	case ASN_V2_TYPE_KIND_ANY:
        if (NULL != asn1_type->as.any.defined_by_id) {
           	ft_ostr_appendf(&ostring, ",\"defined_by_id\":\"%s\"", asn1_type->as.any.defined_by_id);
        } else {
           	ft_ostr_append_cstr(&ostring, ",\"defined_by_id\":null");
        }
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

	ft_ostr_append_cstr(&ostring, "}");
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

static char *__asn1_v2_component_dumps(const t_asn_v2_component *asn1_component)
{
	if (NULL == asn1_component) return ft_strdup("null");

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);

	if (NULL != asn1_component->id) {
		ft_ostr_appendf(&ostring, "{\"id\":\"%s\",", asn1_component->id);
	} else {
		ft_ostr_append_cstr(&ostring, "{\"id\":null,");
	}
	char *dumps = __asn1_v2_type_dumps(asn1_component->type);
	ft_ostr_appendf(&ostring, "\"type\":%s,", dumps);
	SSL_FREE(dumps);

	dumps = __asn1_v2_value_dumps(asn1_component->default_value);
	ft_ostr_appendf(&ostring, "\"default_value\":%s,", dumps);
	SSL_FREE(dumps);

	ft_ostr_appendf(&ostring, "\"optional\":%s", asn1_component->optional ? "true" : "false");
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

	ft_ostr_appendf(&ostring, "{\"tag_mode\":\"%s\",\"types\":{", asn1_v2_get_tag_mode_name(asn1_module->tag_mode));

	t_htbl_v2_next next = {0};
	const char *key = NULL;
	void *value = NULL;
	size_t commas = 0;
	while (ft_htbl_v2_next(&asn1_module->types, &next, &key, &value)) {
		if (commas++) ft_ostr_append_cstr(&ostring, ",");
		char *dumps = __asn1_v2_type_dumps(value);
		ft_ostr_appendf(&ostring, "\"%s\":%s", key, dumps);
		SSL_FREE(dumps);
	}
	ft_ostr_append_cstr(&ostring, "}}");
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);

	return (dumps);
}

char *asn1_v2_module_pretty_dumps(const t_asn_v2_module *asn1_module, int depth, size_t width, bool colored)
{
    char *dumps = asn1_v2_module_dumps(asn1_module);
    const t_json_v2 *json = NULL;
    int status = json_v2_parse(dumps, &json);
    if (JSON_V2_OK == status) {
        SSL_FREE(dumps);
        return (json_v2_pretty_dumps(json, depth, width, colored));
    } else {
        return (dumps);
    }
}

static char *__asn1_v2_tag_dumpb(const t_asn_v2_tag *asn1_tag, char *buf, size_t size)
{
	assert(NULL != buf);
	if (size == 0) return (NULL);

	char *dumps = __asn1_v2_tag_dumps(asn1_tag);
	size_t len = ft_strlen(dumps);
	if (len >= size) len = size-1;
	ft_strncpy(buf, dumps, len);
	buf[len] = '\0';
	SSL_FREE(dumps);
	return (buf);
}

static char *__asn1_v2_value_dumpb(const t_asn_v2_value *asn1_value, char *buf, size_t size)
{
	assert(NULL != buf);
	if (size == 0) return (NULL);

	char *dumps = __asn1_v2_value_dumps(asn1_value);
	size_t len = ft_strlen(dumps);
	if (len >= size) len = size-1;
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
	if (len >= size) len = size-1;
	ft_strncpy(buf, dumps, len);
	buf[len] = '\0';
	SSL_FREE(dumps);
	return (buf);
}

static char *__asn1_v2_component_dumpb(const t_asn_v2_component *asn1_component, char *buf, size_t size)
{
	assert(NULL != buf);
	if (size == 0) return (NULL);

	char *dumps = __asn1_v2_component_dumps(asn1_component);
	size_t len = ft_strlen(dumps);
	if (len >= size) len = size-1;
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
	if (len >= size) len = size-1;
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

static t_asn_v2_component *__asn1_v2_component_create(void);
static void __asn1_v2_component_clear(t_asn_v2_component *asn1_component);
static void __asn1_v2_component_delete(t_asn_v2_component *asn1_component);
static void __asn1_v2_component_copy(const t_asn_v2_component *src, t_asn_v2_component *dst);

static t_asn_v2_constraint	*__asn1_v2_constraint_create(void);
static void __asn1_v2_constraint_clear(t_asn_v2_constraint *asn1_constraint);
static void __asn1_v2_constraint_delete(t_asn_v2_constraint *asn1_constraint);
static void	__asn1_v2_constraint_copy(const t_asn_v2_constraint *src, t_asn_v2_constraint *dst);

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
static inline void __asn1_v2_constraint_delete_adapter(void *p)	 {__asn1_v2_constraint_delete((t_asn_v2_constraint *)p);}
static inline void __asn1_v2_type_delete_adapter(void *p)	 {__asn1_v2_type_delete((t_asn_v2_type *)p);}
static inline void __asn1_v2_component_delete_adapter(void *p) {__asn1_v2_component_delete((t_asn_v2_component *)p);}
static inline void __asn1_v2_module_delete_adapter(void *p)	 {__asn1_v2_module_delete((t_asn_v2_module *)p);}

static inline void *__asn1_v2_tag_copy_adapter(const void *p)
{
	t_asn_v2_tag *dst = __asn1_v2_tag_create();
	__asn1_v2_tag_copy((t_asn_v2_tag *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_constraint_copy_adapter(const void *p)
{
	t_asn_v2_constraint *dst = __asn1_v2_constraint_create();
	__asn1_v2_constraint_copy((const t_asn_v2_constraint *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_value_copy_adapter(const void *p)
{
	t_asn_v2_value *dst = __asn1_v2_value_create();
	__asn1_v2_value_copy((const t_asn_v2_value *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_type_copy_adapter(const void *p)
{
	t_asn_v2_type *dst = __asn1_v2_type_create();
	__asn1_v2_type_copy((const t_asn_v2_type *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_component_copy_adapter(const void *p)
{
	t_asn_v2_component *dst = __asn1_v2_component_create();
	__asn1_v2_component_copy((const t_asn_v2_component *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_module_copy_adapter(const void *p)
{
	t_asn_v2_module *dst = __asn1_v2_module_create();
	__asn1_v2_module_copy((const t_asn_v2_module *)p, dst);
	return (dst);
}

static t_asn_v2_tag	*__asn1_v2_tag_create(void)
{
	t_asn_v2_tag *tag = NULL;
	SSL_ALLOC(tag, sizeof(t_asn_v2_tag));
	*tag = (t_asn_v2_tag){0};
	return (tag);
}

static t_asn_v2_tag	*__asn1_v2_tag_universal_create(t_asn_v2_type_kind kind)
{
	t_asn_v2_tag *tag = NULL;
	SSL_ALLOC(tag, sizeof(t_asn_v2_tag));
	*tag = __asn1_v2_get_universal_tag(kind);
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
		SSL_FREE(asn1_value->as.cstring);
		break;
	case ASN_V2_VALUE_TYPE_BSTRING:
	case ASN_V2_VALUE_TYPE_OSTRING:
		ft_ostr_clear(&asn1_value->as.ostring);
		break;
	case ASN_V2_VALUE_TYPE_LIST:
		ft_list_clear_all_content(&asn1_value->as.list, __asn1_v2_value_delete_adapter);
		break;
	case ASN_V2_VALUE_TYPE_CHOICE:
		if (NULL != asn1_value->as.choice.id) SSL_FREE(asn1_value->as.choice.id);
		if (NULL != asn1_value->as.choice.value) __asn1_v2_value_delete(asn1_value->as.choice.value);
		break;
	case ASN_V2_VALUE_TYPE_ANY:
	    if (NULL != asn1_value->as.any.defined_by_id) SSL_FREE(asn1_value->as.any.defined_by_id);
		ft_ostr_clear(&asn1_value->as.any.octets);
		break;
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
	case ASN_V2_VALUE_TYPE_BSTRING:
	case ASN_V2_VALUE_TYPE_CSTRING:
		dst->as.cstring = ft_strdup(src->as.cstring);
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
		break;
	case ASN_V2_VALUE_TYPE_CHOICE:
		if (NULL != src->as.choice.id) dst->as.choice.id = ft_strdup(src->as.choice.id);
		if (NULL != src->as.choice.value) {
		    dst->as.choice.value = __asn1_v2_value_create();
		    __asn1_v2_value_copy(src->as.choice.value, dst->as.choice.value);
		}
		break;
	case ASN_V2_VALUE_TYPE_ANY:
	    if (NULL != src->as.any.defined_by_id) dst->as.any.defined_by_id = ft_strdup(src->as.any.defined_by_id);
		ft_ostr_copy(&src->as.any.octets, &dst->as.any.octets);
		break;
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
		ft_list_clear_all_content(&asn1_type->as.composite.elements, __asn1_v2_component_delete_adapter);
		break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		if (NULL != asn1_type->as.collection.element_type) __asn1_v2_type_delete(asn1_type->as.collection.element_type);
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
		if (NULL != asn1_type->as.tagged.base_type) __asn1_v2_type_delete(asn1_type->as.tagged.base_type);
		break;
	case ASN_V2_TYPE_KIND_ANY:
	    if (NULL != asn1_type->as.any.defined_by_id) SSL_FREE(asn1_type->as.any.defined_by_id);
		asn1_type->as.any.defined_by_id = NULL;
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
		while (ft_list_next_content(&src->as.composite.elements, &next, &content)) {
			t_asn_v2_component *dst_component = __asn1_v2_component_create();
			__asn1_v2_component_copy(content, dst_component);
			ft_list_append_content(&dst->as.composite.elements, dst_component);
		}
		break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		dst->as.collection.element_type = __asn1_v2_type_create();
		__asn1_v2_type_copy(src->as.collection.element_type, dst->as.collection.element_type);
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
	    dst->as.tagged.tag = src->as.tagged.tag;
		dst->as.tagged.tag_mode = src->as.tagged.tag_mode;
		dst->as.tagged.base_type = __asn1_v2_type_create();
		__asn1_v2_type_copy(src->as.tagged.base_type, dst->as.tagged.base_type);
		break;
	case ASN_V2_TYPE_KIND_ANY:
        if (NULL != src->as.any.defined_by_id) dst->as.any.defined_by_id = ft_strdup(src->as.any.defined_by_id);
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
	}
}

static t_asn_v2_component *__asn1_v2_component_create(void)
{
	t_asn_v2_component *asn1_component = NULL;
	SSL_ALLOC(asn1_component, sizeof(t_asn_v2_component));
	*asn1_component = (t_asn_v2_component){0};
	return (asn1_component);
}

static void __asn1_v2_component_clear(t_asn_v2_component *asn1_component)
{
	if (NULL != asn1_component->id) SSL_FREE(asn1_component->id);
	if (NULL != asn1_component->type) __asn1_v2_type_delete(asn1_component->type);
	if (NULL != asn1_component->default_value) __asn1_v2_value_delete(asn1_component->default_value);
}

static void __asn1_v2_component_delete(t_asn_v2_component *asn1_component)
{
	__asn1_v2_component_clear(asn1_component);
	SSL_FREE(asn1_component);
}

static void	__asn1_v2_component_copy(const t_asn_v2_component *src, t_asn_v2_component *dst)
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
	ft_htbl_v2_init(&asn1_module->types, 256);
	return (asn1_module);
}

static void __asn1_v2_module_delete(t_asn_v2_module *asn1_module)
{
	__asn1_v2_module_clear(asn1_module);
	SSL_FREE(asn1_module);
}

static void __asn1_v2_module_clear(t_asn_v2_module *asn1_module)
{
	ft_htbl_v2_clear(&asn1_module->types, __asn1_v2_type_delete_adapter);
}

static void __asn1_v2_module_copy(const t_asn_v2_module *src, t_asn_v2_module *dst)
{
	dst->tag_mode = src->tag_mode;
	ft_htbl_v2_init(&dst->types, src->types.size);

	const char *key = NULL;
	void *value = NULL;
	t_htbl_v2_next next = {0};
	while (ft_htbl_v2_next(&src->types, &next, &key, &value)) {
		t_asn_v2_type *copy_type = __asn1_v2_type_create();
		__asn1_v2_type_copy(value, copy_type);
		int ok = ft_htbl_v2_set(&dst->types, key, copy_type);
		assert(ok);
	}
}

/****************************************************************************/

static int	__asn1_v2_schema_parse_component(const t_asn_v2_module *asn1_module, t_asn_v2_component **asn1_component, const t_json_v2 *jschema, const t_json_v2 *jcomponent);
static int	__asn1_v2_schema_parse_type(const t_asn_v2_module *asn1_module, t_asn_v2_type **asn1_type, const t_json_v2 *jschema, const t_json_v2 *jtype);
static int	__asn1_v2_schema_parse_value(t_asn_v2_value **asn1_value, const t_json_v2 *jvalue);

static int	__asn1_v2_schema_parse_value(t_asn_v2_value **asn1_value, const t_json_v2 *jvalue)
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
				SSL_LOG(ERROR, "failed to parse component value: %s", json_v2_dumpb(content, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			ft_list_append_content(&avalue->as.list, elem_avalue);
		}
		break;
	case JSON_V2_TYPE_STRING:
		avalue->type = ASN_V2_VALUE_TYPE_CSTRING;
		avalue->as.cstring = ft_strdup(jvalue->as.cstring);
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
	    break;
	}
	*asn1_value = avalue;
	return (SSL_OK);

label_error:
	__asn1_v2_value_delete(avalue);
	return (SSL_ERR);
}

static int	__asn1_v2_schema_parse_type(const t_asn_v2_module *asn1_module, t_asn_v2_type **asn1_type, const t_json_v2 *jschema, const t_json_v2 *jtype)
{
	assert(jtype->type == JSON_V2_TYPE_OBJECT);
	*asn1_type = NULL;

	char cbuf[1024] = {0};

	const t_json_v2 *jkind = NULL;
	int ret = json_v2_query_nonnull(__JQ_TYPE_KIND, jtype, &jkind);
	assert(JSON_V2_OK == ret && jkind->type == JSON_V2_TYPE_STRING);
	SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TYPE_KIND, json_v2_dumpb(jkind, cbuf, sizeof(cbuf)));

	t_asn_v2_type *atype = __asn1_v2_type_create();
	atype->kind = __asn1_v2_schema_get_type_by_name(jkind->as.cstring);

	switch (atype->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET:
		;;
		const t_json_v2 *jcomponents = NULL;
		if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_COMPONENTS, jtype, &jcomponents)) {
			assert(jcomponents->type == JSON_V2_TYPE_ARRAY);
			void *content = NULL;
			t_list_next next = {0};
			while (ft_list_next_content(&jcomponents->as.list, &next, &content)) {
				t_asn_v2_component *component = NULL;
				if (SSL_OK != __asn1_v2_schema_parse_component(asn1_module, &component, jschema, content)) {
					SSL_LOG(ERROR, "failed to parse `%s` in asn1 type: %s", __JQ_TYPE_COMPONENTS, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
					goto label_error;
				}
				if (NULL != component->default_value) {

				}
				ft_list_append_content(&atype->as.composite.elements, component);
			}
		}
		break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		;;
		const t_json_v2 *jelement_type = NULL;
		if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_COMPONENT_TYPE, jtype, &jelement_type)) {
			assert(jelement_type->type == JSON_V2_TYPE_OBJECT);
			SSL_LOG(TRACE, "asn1 component: `%s`: %s", __JQ_TYPE_COMPONENT_TYPE, json_v2_dumpb(jelement_type, cbuf, sizeof(cbuf)));
			if (SSL_OK != __asn1_v2_schema_parse_type(asn1_module, &atype->as.collection.element_type, jschema, jelement_type)) {
			SSL_LOG(ERROR, "failed to parse `%s` in asn1 type: %s", __JQ_TYPE_COMPONENT_TYPE, json_v2_dumpb(jelement_type, cbuf, sizeof(cbuf)));
				goto label_error;
			}
		}
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
		;;
		const t_json_v2 *jtag_mode = NULL;
		if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_TAG_MODE, jtype, &jtag_mode)) {
			assert(jtag_mode->type == JSON_V2_TYPE_STRING);
			SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TYPE_TAG_MODE, json_v2_dumpb(jtag_mode, cbuf, sizeof(cbuf)));
			atype->as.tagged.tag_mode = __asn1_v2_schema_get_tag_mode_by_name(jtag_mode->as.cstring);
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

		const t_json_v2 *jtag_class = NULL;
		ret = json_v2_query_nonnull(__JQ_TYPE_TAG_CLASS, jtype, &jtag_class);
		assert(JSON_V2_OK == ret && jtag_class->type == JSON_V2_TYPE_STRING);
		SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TYPE_TAG_CLASS, json_v2_dumpb(jtag_class, cbuf, sizeof(cbuf)));
		atype->as.tagged.tag.class = __asn1_v2_schema_get_tag_class_by_name(jtag_class->as.cstring);

		const t_json_v2 *jtag_number = NULL;
		ret = json_v2_query_nonnull(__JQ_TYPE_TAG_NUMBER, jtype, &jtag_number);
		assert(JSON_V2_OK == ret && jtag_number->type == JSON_V2_TYPE_NUMBER);
		SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TYPE_TAG_NUMBER, json_v2_dumpb(jtag_number, cbuf, sizeof(cbuf)));
		atype->as.tagged.tag.number = (uint32_t)bnum_to_dig_u(&jtag_number->as.number);

		const t_json_v2 *jbase_type = NULL;
		ret = json_v2_query_nonnull(__JQ_TYPE_BASE_TYPE, jtype, &jbase_type);
		assert(JSON_V2_OK == ret && jbase_type->type == JSON_V2_TYPE_OBJECT);
		SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TYPE_BASE_TYPE, json_v2_dumpb(jbase_type, cbuf, sizeof(cbuf)));
		if (SSL_OK != __asn1_v2_schema_parse_type(asn1_module, &atype->as.tagged.base_type, jschema, jbase_type)) {
			SSL_LOG(ERROR, "failed to parse `%s` from asn1 type: %s", __JQ_TYPE_BASE_TYPE, json_v2_dumpb(jbase_type, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		break;
	case ASN_V2_TYPE_KIND_ANY:
	    ;;
    	const t_json_v2 *jdefined_by_id = NULL;
    	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_DEFINED_BY_ID, jtype, &jdefined_by_id)) {
    		assert(jdefined_by_id->type == JSON_V2_TYPE_STRING);
    		SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TYPE_DEFINED_BY_ID, json_v2_dumpb(jdefined_by_id, cbuf, sizeof(cbuf)));
    		atype->as.any.defined_by_id = ft_strdup(jdefined_by_id->as.cstring);
    	} else {
       		atype->as.any.defined_by_id = NULL;
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
        // Not a known type but could be a reference to another type in the module.
	    SSL_LOG(TRACE, "asn1 type: `%s`: not a standard asn1 type: maybe type in the module?", jkind->as.cstring, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
        const t_json_v2 *jref_type = NULL;
        char *query = ft_strjoin_multi(3, __JQ_SCHEMA_TYPES, ".", jkind->as.cstring);
        ret = json_v2_query_nonnull(query, jschema, &jref_type);
        SSL_FREE(query);
        assert(JSON_V2_OK == ret && jref_type->type == JSON_V2_TYPE_OBJECT);
        t_asn_v2_type *ref_atype = NULL;
        if (SSL_OK != __asn1_v2_schema_parse_type(asn1_module, &ref_atype, jschema, jref_type)) {
            SSL_LOG(ERROR, "failed to parse `%s` asn1 type reference: %s", jkind->as.cstring, json_v2_dumpb(jref_type, cbuf, sizeof(cbuf)));
            goto label_error;
        }
        __asn1_v2_type_copy(ref_atype, atype);
        __asn1_v2_type_delete(ref_atype);
	}

	*asn1_type = atype;
	return (SSL_OK);

label_error:
	__asn1_v2_type_delete(atype);
	return (SSL_ERR);
}

static int	__asn1_v2_schema_parse_component(const t_asn_v2_module *asn1_module, t_asn_v2_component **asn1_component, const t_json_v2 *jschema, const t_json_v2 *jcomponent)
{
	assert(jcomponent->type == JSON_V2_TYPE_OBJECT);
	*asn1_component = NULL;

	t_asn_v2_component *acomponent = __asn1_v2_component_create();
	char cbuf[1024] = {0};

	const t_json_v2 *jid = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_COMPONENT_ID, jcomponent, &jid)) {
		assert(jid->type == JSON_V2_TYPE_STRING);
		SSL_LOG(TRACE, "asn1 component: `%s`: %s", __JQ_COMPONENT_ID, json_v2_dumpb(jid, cbuf, sizeof(cbuf)));
		acomponent->id = ft_strdup(jid->as.cstring);
	}

	const t_json_v2 *jtype = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_COMPONENT_TYPE, jcomponent, &jtype)) {
		assert(jtype->type == JSON_V2_TYPE_OBJECT);
		SSL_LOG(TRACE, "asn1 component: `%s`: %s", __JQ_COMPONENT_TYPE, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
		if (SSL_OK != __asn1_v2_schema_parse_type(asn1_module, &acomponent->type, jschema, jtype)) {
			SSL_LOG(ERROR, "failed to parse `%s` in asn1 component: %s", __JQ_COMPONENT_TYPE, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			goto label_error;
		}
	}

	const t_json_v2 *joptional = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_COMPONENT_OPTIONAL, jcomponent, &joptional)) {
		assert(joptional->type == JSON_V2_TYPE_BOOL);
		SSL_LOG(TRACE, "asn1 component: `%s`: %s", __JQ_COMPONENT_OPTIONAL, json_v2_dumpb(joptional, cbuf, sizeof(cbuf)));
		acomponent->optional = joptional->as.boolean;
	}

	const t_json_v2 *jdefault = NULL;
	if (JSON_V2_OK == json_v2_query(__JQ_COMPONENT_DEFAULT, jcomponent, &jdefault)) {
		SSL_LOG(TRACE, "asn1 component: `%s`: %s", __JQ_COMPONENT_DEFAULT, json_v2_dumpb(jdefault, cbuf, sizeof(cbuf)));
		if (SSL_OK != __asn1_v2_schema_parse_value(&acomponent->default_value, jdefault)) {
			SSL_LOG(ERROR, "failed to parse `%s` in asn1 component: %s", __JQ_COMPONENT_DEFAULT, json_v2_dumpb(jdefault, cbuf, sizeof(cbuf)));
			goto label_error;
		}
	}

	*asn1_component = acomponent;
	return (SSL_OK);

label_error:
	__asn1_v2_component_delete(acomponent);
	return (SSL_ERR);
}

int	asn1_v2_schema_parse(t_asn_v2_module **asn1_module, const t_json_v2 *jschema)
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

	const t_json_v2 *jtagmode = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_SCHEMA_TAG_MODE, jschema, &jtagmode)) {
		amodule->tag_mode = __asn1_v2_schema_get_tag_mode_by_name(jtagmode->as.cstring);
	}

	const t_json_v2 *jtypes = NULL;
	int ret = json_v2_query_nonnull(__JQ_SCHEMA_TYPES, jschema, &jtypes);
	assert(JSON_V2_OK == ret && jtypes->type == JSON_V2_TYPE_OBJECT);

	const char *key = NULL;
	void *value = NULL;
	t_htbl_v2_next next = {0};
	while (ft_htbl_v2_next(&jtypes->as.htable, &next, &key, &value)) {
		t_asn_v2_type *atype = NULL;
		if (SSL_OK != __asn1_v2_schema_parse_type(amodule, &atype, jschema, value)) {
			SSL_LOG(ERROR, "failed to parse type: `%s`", key);
			goto label_error;
		}
		if (!ft_htbl_v2_set(&amodule->types, key, atype)) {
			SSL_LOG(ERROR, "failed to set type name: `%s`, duplicate type name?", key);
			goto label_error;
		}
	}

	if (ASN_V2_TAG_MODE_AUTOMATIC == amodule->tag_mode) {
    	if (SSL_OK != asn1_v2_module_compile_automatic_tags(asn1_module, amodule)) {
           	SSL_LOG(ERROR, "failed to compile automatic tags in the parsed module");
           	goto label_error;
    	}
	}
	else {
	    *asn1_module = amodule;
	}

	return (SSL_OK);

label_error:
	__asn1_v2_module_delete(amodule);
	return (SSL_ERR);
}

/****************************************************************************/

static int	__asn1_v2_type_compile_automatic_tags(const t_asn_v2_module *asn1_module, t_asn_v2_type *asn1_type);
static int	__asn1_v2_component_compile_automatic_tags(const t_asn_v2_module *asn1_module, t_asn_v2_component *asn1_component);

static int	__asn1_v2_type_compile_automatic_tags(const t_asn_v2_module *asn1_module, t_asn_v2_type *asn1_type)
{
	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET:
		;;
		bool has_tagged_component = false;
		void *content = NULL;
		t_list_next next = {0};
		while (ft_list_next_content(&asn1_type->as.composite.elements, &next, &content)) {
			t_asn_v2_component *component = content;
			assert(NULL != component->type);
			if (ASN_V2_TYPE_KIND_TAGGED == component->type->kind) has_tagged_component = true;
			if (SSL_OK != __asn1_v2_component_compile_automatic_tags(asn1_module, component)) {
				return (SSL_ERR);
			}
		}
		if (ASN_V2_TAG_MODE_AUTOMATIC == asn1_module->tag_mode && !has_tagged_component) {
			// We must tag types for all components: implicit for SEQUENCE/SET or explicit for CHOICE.
			uint32_t tag_number = 0;
			next = (t_list_next){0};
			while (ft_list_next_content(&asn1_type->as.composite.elements, &next, &content)) {
				t_asn_v2_component *component = content;
				t_asn_v2_type *element_type = component->type;
				t_asn_v2_type *wrapper_type = __asn1_v2_type_create();
				wrapper_type->kind = ASN_V2_TYPE_KIND_TAGGED;
				if (asn1_type->kind == ASN_V2_TYPE_KIND_CHOICE) {
					wrapper_type->as.tagged.tag_mode = ASN_V2_TAG_MODE_EXPLICIT;
				} else {
					wrapper_type->as.tagged.tag_mode = ASN_V2_TAG_MODE_IMPLICIT;
				}
				wrapper_type->as.tagged.tag.class = ASN_V2_TAG_CLASS_CONTEXT_SPECIFIC;
				wrapper_type->as.tagged.tag.number = tag_number;
				wrapper_type->as.tagged.base_type = element_type;
				component->type = wrapper_type;
				tag_number++;
			}
		}
		return (SSL_OK);
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		return (__asn1_v2_type_compile_automatic_tags(asn1_module, asn1_type->as.collection.element_type));
	case ASN_V2_TYPE_KIND_TAGGED:
		return (__asn1_v2_type_compile_automatic_tags(asn1_module, asn1_type->as.tagged.base_type));
	case ASN_V2_TYPE_KIND_ANY:
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
	}
}

static int	__asn1_v2_component_compile_automatic_tags(const t_asn_v2_module *asn1_module, t_asn_v2_component *asn1_component)
{
	return (__asn1_v2_type_compile_automatic_tags(asn1_module, asn1_component->type));
}

int	asn1_v2_module_compile_automatic_tags(t_asn_v2_module **asn1_module_compiled, const t_asn_v2_module *asn1_module)
{
    if (NULL == asn1_module_compiled || NULL == asn1_module) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	*asn1_module_compiled = NULL;

	SSL_LOG(TRACE, "compiling automatic tags for asn1 module");

	t_asn_v2_module *amodule = __asn1_v2_module_create();
	__asn1_v2_module_copy(asn1_module, amodule);

	const char *key = NULL;
	void *value = NULL;
	t_htbl_v2_next next = {0};
	while (ft_htbl_v2_next(&amodule->types, &next, &key, &value)) {
	    SSL_LOG(TRACE, "compiling automatic tags for asn1 type: %s", key);
		if (SSL_OK != __asn1_v2_type_compile_automatic_tags(amodule, value)) {
			SSL_LOG(ERROR, "failed to compile automatic tags for asn1 type: `%s`", key);
			__asn1_v2_module_delete(amodule);
			return (SSL_ERR);
		}
	}
	*asn1_module_compiled = amodule;
	return (SSL_OK);
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

static t_der_v2_value *__der_v2_value_create(void);
static void	__der_v2_value_copy(t_der_v2_value *src, t_der_v2_value *dst);
static void __der_v2_value_clear(t_der_v2_value *der_value);
static void __der_v2_value_delete(t_der_v2_value *der_value);

static t_der_v2_type *__der_v2_type_create(void);
static void __der_v2_type_delete(t_der_v2_type *der_type);

static t_der_v2_component *__der_v2_component_create(void);
static void __der_v2_component_delete(t_der_v2_component *der_component);

static inline void __der_v2_value_delete_adapter(void *p) { __der_v2_value_delete((t_der_v2_value *)p); };
static inline void __der_v2_type_delete_adapter(void *p) { __der_v2_type_delete((t_der_v2_type *)p); };
static inline void __der_v2_component_delete_adapter(void *p) { __der_v2_component_delete((t_der_v2_component *)p); };

static t_der_v2_value *__der_v2_value_create(void)
{
    t_der_v2_value *der_value = NULL;
    SSL_ALLOC(der_value, sizeof(t_der_v2_value));
    *der_value = (t_der_v2_value){0};
    return (der_value);
}

static void	__der_v2_value_copy(t_der_v2_value *src, t_der_v2_value *dst)
{
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
	    if (NULL != src->as.cstring) dst->as.cstring = ft_strdup(src->as.cstring);
        break;
	case ASN_V2_VALUE_TYPE_OSTRING:
	case ASN_V2_VALUE_TYPE_BSTRING:
	    ft_ostr_copy(&src->as.ostring, &dst->as.ostring);
        break;
	case ASN_V2_VALUE_TYPE_LIST:
		if (src->as.array.count > 0) {
		    SSL_ALLOC(dst->as.array.items, src->as.array.count * sizeof(t_der_v2_value));
			dst->as.array.count = src->as.array.count;
    		for (size_t idx = 0; idx < src->as.array.count; idx++) {
                __der_v2_value_copy(src->as.array.items + idx, dst->as.array.items + idx);
    		}
		}
        break;
	case ASN_V2_VALUE_TYPE_CHOICE:
        if (NULL != src->as.choice.id) dst->as.choice.id = ft_strdup(src->as.choice.id);
        __der_v2_value_copy(src->as.choice.value, dst->as.choice.value);
        break;
	case ASN_V2_VALUE_TYPE_ANY:
	    if (NULL != src->as.any.defined_by_id) dst->as.any.defined_by_id = ft_strdup(src->as.any.defined_by_id);
        ft_ostr_copy(&src->as.any.octets, &dst->as.any.octets);
        break;
	}
	dst->type = src->type;
}

static void __der_v2_value_clear(t_der_v2_value *der_value)
{
    switch (der_value->type) {
	case ASN_V2_VALUE_TYPE_NULL:
	    return;
	case ASN_V2_VALUE_TYPE_BOOLEAN:
		return;
	case ASN_V2_VALUE_TYPE_NUMBER:
		bnum_clear(&der_value->as.number);
		return;
	case ASN_V2_VALUE_TYPE_CSTRING:
	    if (NULL != der_value->as.cstring) SSL_FREE(der_value->as.cstring);
		der_value->as.cstring = NULL;
		return;
	case ASN_V2_VALUE_TYPE_OSTRING:
	case ASN_V2_VALUE_TYPE_BSTRING:
	    ft_ostr_clear(&der_value->as.ostring);
		return;
	case ASN_V2_VALUE_TYPE_LIST:
	    for (size_t idx = 0; idx < der_value->as.array.count; idx++) {
			__der_v2_value_clear(der_value->as.array.items + idx);
		}
		der_value->as.array.count = 0;
		SSL_FREE(der_value->as.array.items);
		der_value->as.array.items = NULL;
		return;
	case ASN_V2_VALUE_TYPE_CHOICE:
	    if (NULL != der_value->as.choice.id) SSL_FREE(der_value->as.choice.id);
		der_value->as.choice.id = NULL;
	    __der_v2_value_delete(der_value->as.choice.value);
		der_value->as.choice.value = NULL;
		return;
	case ASN_V2_VALUE_TYPE_ANY:
	    if (NULL != der_value->as.any.defined_by_id) SSL_FREE(der_value->as.any.defined_by_id);
		der_value->as.any.defined_by_id = NULL;
        ft_ostr_clear(&der_value->as.any.octets);
	    return;
	}
}

static void __der_v2_value_delete(t_der_v2_value *der_value)
{
    __der_v2_value_clear(der_value);
    SSL_FREE(der_value);
}

static t_der_v2_component *__der_v2_component_create(void)
{
	t_der_v2_component *der_component = NULL;
	SSL_ALLOC(der_component, sizeof(t_der_v2_component));
	*der_component = (t_der_v2_component){0};
	return (der_component);
}

static void __der_v2_component_delete(t_der_v2_component *der_component)
{
    if (NULL == der_component) return;

    if (NULL != der_component->id) SSL_FREE(der_component->id);
    if (NULL != der_component->default_value) __der_v2_value_delete(der_component->default_value);
    SSL_FREE(der_component);
}

static t_der_v2_type *__der_v2_type_create(void)
{
	t_der_v2_type *der_type = NULL;
	SSL_ALLOC(der_type, sizeof(t_der_v2_type));
	*der_type = (t_der_v2_type){0};
	return (der_type);
}

static void __der_v2_type_delete(t_der_v2_type *der_type)
{
    if (NULL == der_type) return;

    ft_list_clear_all_content(&der_type->constraints, __asn1_v2_constraint_delete_adapter);

	switch (der_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SET:
	case ASN_V2_TYPE_KIND_CHOICE:
		ft_list_clear_all_content(&der_type->as.composite.elements, __der_v2_type_delete_adapter);
		break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		__der_v2_type_delete(der_type->as.collection.element_type);
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
	case ASN_V2_TYPE_KIND_ANY:
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
	SSL_FREE(der_type);
}

static int	__asn1_v2_value_compile(t_der_v2_value **der_value, const t_asn_v2_value *asn1_value)
{
    assert(NULL != asn1_value && NULL != der_value);

    char cbuf[1024];

    t_der_v2_value *compiled = __der_v2_value_create();

	switch (asn1_value->type) {
	case ASN_V2_VALUE_TYPE_NULL:
	    compiled->type = asn1_value->type;
	    break;
	case ASN_V2_VALUE_TYPE_BOOLEAN:
	    compiled->type = asn1_value->type;
		compiled->as.boolean = asn1_value->as.boolean;
	    break;
	case ASN_V2_VALUE_TYPE_NUMBER:
	    compiled->type = asn1_value->type;
		bnum_copy(&asn1_value->as.number, &compiled->as.number);
	    break;
	case ASN_V2_VALUE_TYPE_CSTRING:
	    compiled->type = asn1_value->type;
	    if (NULL != asn1_value->as.cstring) compiled->as.cstring = ft_strdup(asn1_value->as.cstring);
	    break;
	case ASN_V2_VALUE_TYPE_OSTRING:
	case ASN_V2_VALUE_TYPE_BSTRING:
	    compiled->type = asn1_value->type;
	    ft_ostr_copy(&asn1_value->as.ostring, &compiled->as.ostring);
	    break;
	case ASN_V2_VALUE_TYPE_LIST:
	    compiled->type = asn1_value->type;
		if (asn1_value->as.list.size > 0) {
		    SSL_ALLOC(compiled->as.array.items, asn1_value->as.list.size * sizeof(t_der_v2_value));
			compiled->as.array.count = asn1_value->as.list.size;
    		t_list_next next = {0};
    		void *content;
    		for (size_t idx = 0; ft_list_next_content(&asn1_value->as.list, &next, &content); idx++) {
                assert(idx < compiled->as.array.count);
                t_der_v2_value *compiled_element = NULL;
                if (SSL_OK != __asn1_v2_value_compile(&compiled_element, content)) {
                    goto label_error;
                }
                __der_v2_value_copy(compiled_element, compiled->as.array.items + idx);
                __der_v2_value_delete(compiled_element);
    		}
		}
	    break;
	case ASN_V2_VALUE_TYPE_CHOICE:
	    compiled->type = asn1_value->type;
	    if (NULL != asn1_value->as.choice.id) compiled->as.choice.id = ft_strdup(asn1_value->as.choice.id);
		if (SSL_OK != __asn1_v2_value_compile(&compiled->as.choice.value, asn1_value->as.choice.value)) {
            goto label_error;
		}
	    break;
	case ASN_V2_VALUE_TYPE_ANY:
	    compiled->type = asn1_value->type;
	    if (NULL != asn1_value->as.any.defined_by_id) compiled->as.any.defined_by_id = ft_strdup(asn1_value->as.any.defined_by_id);
        ft_ostr_copy(&asn1_value->as.any.octets, &compiled->as.any.octets);
	    break;
	}
	*der_value = compiled;
	return (SSL_OK);

label_error:
    SSL_LOG(ERROR, "failed to compile `%s` asn1 value: %s", asn1_v2_get_value_type_name(asn1_value->type), __asn1_v2_value_dumpb(asn1_value, cbuf, sizeof(cbuf)));
    __der_v2_value_delete(compiled);
    return (SSL_ERR);
}

static int __asn1_v2_component_compile(t_der_v2_component **der_component, const t_asn_v2_component *asn1_component)
{
	char cbuf[1024] = {0};

	if (NULL != asn1_component->id) {
		SSL_LOG(TRACE, "compiling `%s` asn1 component", asn1_component->id);
	} else {
		SSL_LOG(TRACE, "compiling asn1 component: %s", __asn1_v2_component_dumpb(asn1_component, cbuf, sizeof(cbuf)));
	}

	t_der_v2_type *compiled = NULL;
	if (SSL_OK != asn1_v2_type_compile(&compiled, asn1_component->type)) {
		SSL_LOG(ERROR, "failed to compile asn1 component type: %s", __asn1_v2_type_dumpb(asn1_component->type, cbuf, sizeof(cbuf)));
		return (SSL_ERR);
	}

	t_der_v2_component *dcomponent = __der_v2_component_create();

	if (NULL != asn1_component->default_value) {
	    dcomponent->default_value = NULL;
		if (SSL_OK != __asn1_v2_value_compile(&dcomponent->default_value, asn1_component->default_value)) {
		    SSL_LOG(ERROR, "failed to compile asn1 component default value: %s", __asn1_v2_value_dumpb(asn1_component->default_value, cbuf, sizeof(cbuf)));
			SSL_FREE(dcomponent);
			return (SSL_ERR);
		}
	}
	if (NULL != asn1_component->id) {
	    dcomponent->id = ft_strdup(asn1_component->id);
	}
	dcomponent->optional = asn1_component->optional;
	dcomponent->type = compiled;

	*der_component = dcomponent;
	return (SSL_OK);
}

int	asn1_v2_type_compile(t_der_v2_type **der_type, const t_asn_v2_type *asn1_type)
{
    if (NULL == der_type || NULL == asn1_type) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	char cbuf[1024] = {0};
	*der_type = NULL;

	SSL_LOG(TRACE, "compiling `%s` asn1 type", asn1_v2_get_type_name(asn1_type->kind));

	t_der_v2_type *compiled = __der_v2_type_create();

	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SET:
		;;
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&asn1_type->as.composite.elements, &next, &content)) {
			t_der_v2_component *compiled_element = NULL;
			if (SSL_OK != __asn1_v2_component_compile(&compiled_element, content)) {
				SSL_LOG(ERROR, "failed to compile `%s` asn1 type: %s", asn1_v2_get_type_name(asn1_type->kind), __asn1_v2_type_dumpb(content, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			ft_list_append_content(&compiled->as.composite.elements, compiled_element);
		}
		compiled->kind = asn1_type->kind;
		compiled->implicit_tag = __asn1_v2_tag_universal_create(asn1_type->kind);
		break;
	case ASN_V2_TYPE_KIND_CHOICE:
        next = (t_list_next){0};
    	content = NULL;
    	while (ft_list_next_content(&asn1_type->as.composite.elements, &next, &content)) {
    		t_der_v2_component *compiled_element = NULL;
    		if (SSL_OK != __asn1_v2_component_compile(&compiled_element, content)) {
    			SSL_LOG(ERROR, "failed to compile `%s` asn1 type: %s", asn1_v2_get_type_name(asn1_type->kind), __asn1_v2_type_dumpb(content, cbuf, sizeof(cbuf)));
    			goto label_error;
    		}
            if (NULL == compiled_element->id) {
     			SSL_LOG(ERROR, "failed to compile `%s` asn1 type: missing id for choice element", asn1_v2_get_type_name(asn1_type->kind));
     			goto label_error;
            }
            if (NULL != compiled_element->default_value) {
     			SSL_LOG(ERROR, "failed to compile `%s` asn1 type: unexpected default value for choice element", asn1_v2_get_type_name(asn1_type->kind));
     			goto label_error;
            }
    		ft_list_append_content(&compiled->as.composite.elements, compiled_element);
    	}
        // Choice does not have an implicit tag, its tag is a choice from a set of its alternatives.
        compiled->kind = asn1_type->kind;
    	break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		if (SSL_OK != asn1_v2_type_compile(&compiled->as.collection.element_type, asn1_type->as.collection.element_type)) {
			SSL_LOG(ERROR, "failed to compile `%s` asn1 type: %s", asn1_v2_get_type_name(asn1_type->kind), __asn1_v2_type_dumpb(asn1_type, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		compiled->kind = asn1_type->kind;
		compiled->implicit_tag = __asn1_v2_tag_universal_create(asn1_type->kind);
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
	    ;;
	    t_der_v2_type *compiled_tagged = NULL;
        if (SSL_OK != asn1_v2_type_compile(&compiled_tagged, asn1_type->as.tagged.base_type)) {
            SSL_LOG(ERROR, "failed to compile `%s` asn1 type: %s", asn1_v2_get_type_name(asn1_type->kind), __asn1_v2_type_dumpb(asn1_type, cbuf, sizeof(cbuf)));
            goto label_error;
        }
	    switch (asn1_type->as.tagged.tag_mode) {
    	case ASN_V2_TAG_MODE_AUTOMATIC:
    	    SSL_LOG(ERROR, "failed to compile `%s` asn1 type: unexpected automatic tag mode", asn1_v2_get_type_name(asn1_type->kind));
    		goto label_error;
        case ASN_V2_TAG_MODE_EXPLICIT:
            ;;
		    t_asn_v2_tag *etag = __asn1_v2_tag_create();
			*etag = asn1_type->as.tagged.tag;
		    ft_list_append_content(&compiled->explicit_tags, etag);
		    ft_list_copy_all_content(&compiled->explicit_tags, &compiled_tagged->explicit_tags, __asn1_v2_tag_copy_adapter);
			if (NULL != compiled_tagged->implicit_tag) {
			    compiled->implicit_tag = __asn1_v2_tag_copy_adapter(compiled_tagged->implicit_tag);
			}
			break;
       	case ASN_V2_TAG_MODE_IMPLICIT:
            compiled->implicit_tag = __asn1_v2_tag_create();
			*compiled->implicit_tag = asn1_type->as.tagged.tag;
			break;
		}
		compiled->kind = compiled_tagged->kind;
		__der_v2_type_delete(compiled_tagged);
		break;
	case ASN_V2_TYPE_KIND_ANY:
        compiled->kind = asn1_type->kind;
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
	    compiled->kind = asn1_type->kind;
	    compiled->implicit_tag = __asn1_v2_tag_universal_create(asn1_type->kind);
	    break;
	}

	ft_list_copy_all_content(&asn1_type->constraints, &compiled->constraints, __asn1_v2_constraint_copy_adapter);

	*der_type = compiled;
	return (SSL_OK);

label_error:
	__der_v2_type_delete(compiled);
	return (SSL_ERR);
}

/****************************************************************************/

static char *__der_v2_value_dumps(const t_der_v2_value *der_value)
{
	if (NULL == der_value) return ft_strdup("null");

	switch (der_value->type) {
	case ASN_V2_VALUE_TYPE_NULL:
		return ft_strdup("null");
	case ASN_V2_VALUE_TYPE_BOOLEAN:
		return (der_value->as.boolean) ? ft_strdup("true") : ft_strdup("false");
	case ASN_V2_VALUE_TYPE_NUMBER:
		return bnum_to_dec(&der_value->as.number);
	case ASN_V2_VALUE_TYPE_CSTRING:
		return ft_strjoin_multi(3, "\"", der_value->as.cstring, "\"");
	case ASN_V2_VALUE_TYPE_OSTRING:
	case ASN_V2_VALUE_TYPE_BSTRING:
		;;
		char *ostring_dumps = NULL;
		ft_sprintf(&ostring_dumps, "{\"octets\":\"<%p,size=%zu>\"}", &der_value->as.ostring, der_value->as.ostring.size);
		return ostring_dumps;
	case ASN_V2_VALUE_TYPE_LIST:
		;;
		t_ostring ostring = {0};
		ft_ostr_init_with_capacity(&ostring, 1024);
		ft_ostr_append_cstr(&ostring, "[");
		size_t commas = 0;
		for (size_t idx = 0; idx < der_value->as.array.count; idx++) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = __der_v2_value_dumps(der_value->as.array.items + idx);
			ft_ostr_append_cstr(&ostring, dumps);
			SSL_FREE(dumps);
		}
		ft_ostr_append_cstr(&ostring, "]");
		char *dumps = ft_ostr_to_cstr(&der_value->as.ostring, 0, der_value->as.ostring.size);
		ft_ostr_clear(&ostring);
		return (dumps);
	case ASN_V2_VALUE_TYPE_CHOICE:
    	;;
    	char *choice_dumps = NULL;
        char *alt_dumps = __der_v2_value_dumps(der_value->as.choice.value);
        if (NULL != der_value->as.choice.id) {
           	ft_sprintf(&choice_dumps, "{\"choice\":{\"id\":\"%s\",\"value\":%s}}", der_value->as.choice.id, alt_dumps);
        } else {
           	ft_sprintf(&choice_dumps, "{\"choice\":{\"id\":null,\"value\":%s}}", alt_dumps);
        }
        SSL_FREE(alt_dumps);
    	return (choice_dumps);
	case ASN_V2_VALUE_TYPE_ANY:
	    ;;
    	char *any_dumps = NULL;
        if (NULL != der_value->as.any.defined_by_id) {
           	ft_sprintf(&any_dumps, "{\"any\":{\"defined_by_id\":\"%s\",\"octets\":\"<%p,size=%zu>\"}}", der_value->as.any.defined_by_id, &der_value->as.any.octets, der_value->as.any.octets.size);
        } else {
           	ft_sprintf(&any_dumps, "{\"any\":{\"defined_by_id\":null,\"octets\":\"<%p,size=%zu>\"}}", &der_value->as.any.octets, der_value->as.any.octets.size);
        }
    	return (any_dumps);
	}
}

static char *__der_v2_component_dumps(const t_der_v2_component *der_component)
{
    if (NULL == der_component) return ft_strdup("null");

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);

	if (NULL != der_component->id) {
		ft_ostr_appendf(&ostring, "{\"id\":\"%s\",", der_component->id);
	} else {
		ft_ostr_append_cstr(&ostring, "{\"id\":null,");
	}
	char *dumps = der_v2_type_dumps(der_component->type);
	ft_ostr_appendf(&ostring, "\"type\":%s,", dumps);
	SSL_FREE(dumps);

	dumps = __der_v2_value_dumps(der_component->default_value);
	ft_ostr_appendf(&ostring, "\"default_value\":%s,", dumps);
	SSL_FREE(dumps);

	ft_ostr_appendf(&ostring, "\"optional\":%s", der_component->optional ? "true" : "false");
	ft_ostr_append_cstr(&ostring, "}");

	dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);

	return (dumps);
}

char *der_v2_type_dumps(const t_der_v2_type *der_type)
{
    if (NULL == der_type) return ft_strdup("null");

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);

	ft_ostr_appendf(&ostring, "{\"kind\":\"%s\"", asn1_v2_get_type_name(der_type->kind));

	if (NULL != der_type->implicit_tag) {
	    char *tag_dumps = __asn1_v2_tag_dumps(der_type->implicit_tag);
	    ft_ostr_appendf(&ostring, ",\"implicit_tag\":%s", tag_dumps);
		SSL_FREE(tag_dumps);
	} else {
        ft_ostr_append_cstr(&ostring, ",\"implicit_tag\":null");
	}

	if (der_type->explicit_tags.size > 0) {
        ft_ostr_append_cstr(&ostring, ",\"explicit_tags\":[");
    	t_list_next next = {0};
    	void *content = NULL;
    	size_t commas = 0;
    	while (ft_list_next_content(&der_type->explicit_tags, &next, &content)) {
    	    if (commas++) ft_ostr_append_cstr(&ostring, ",");
    	    char *tag_dumps = __asn1_v2_tag_dumps(content);
    	    ft_ostr_append_cstr(&ostring, tag_dumps);
    		SSL_FREE(tag_dumps);
    	}
    	ft_ostr_append_cstr(&ostring, "]");
	}

	if (der_type->constraints.size > 0) {
        ft_ostr_append_cstr(&ostring, ",\"constraints\":[");
       	t_list_next next = {0};
       	void *content = NULL;
       	size_t commas = 0;
    	while (ft_list_next_content(&der_type->constraints, &next, &content)) {
    	    if (commas++) ft_ostr_append_cstr(&ostring, ",");
    	    char *constraint_dumps = __asn1_v2_constraint_dumps(content);
    	    ft_ostr_append_cstr(&ostring, constraint_dumps);
    		SSL_FREE(constraint_dumps);
    	}
    	ft_ostr_append_cstr(&ostring, "]");
	}

	switch (der_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET:
		;;
		t_list_next next = {0};
		void *content;
		size_t commas = 0;
		ft_ostr_append_cstr(&ostring, ",\"components\":[");
		while (ft_list_next_content(&der_type->as.composite.elements, &next, &content)) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = __der_v2_component_dumps(content);
			ft_ostr_append_cstr(&ostring, dumps);
			SSL_FREE(dumps);
		}
		ft_ostr_append_cstr(&ostring, "]");
		break;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		;;
		char *dumps = der_v2_type_dumps(der_type->as.collection.element_type);
		ft_ostr_appendf(&ostring, ",\"element_type\":%s", dumps);
		SSL_FREE(dumps);
		break;
	case ASN_V2_TYPE_KIND_TAGGED:
	case ASN_V2_TYPE_KIND_ANY:
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

	ft_ostr_append_cstr(&ostring, "}");
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

char *der_v2_type_pretty_dumps(const t_der_v2_type *der_type, int depth, size_t width, bool colored)
{
    char *dumps = der_v2_type_dumps(der_type);
    const t_json_v2 *json = NULL;
    int status = json_v2_parse(dumps, &json);
    if (JSON_V2_OK == status) {
        SSL_FREE(dumps);
        return (json_v2_pretty_dumps(json, depth, width, colored));
    } else {
        return (dumps);
    }
}

char *der_v2_type_dumpb(const t_der_v2_type *der_type, char *buf, size_t size)
{
	assert(NULL != buf);
	if (size == 0) return (NULL);

	char *dumps = der_v2_type_dumps(der_type);
	size_t len = ft_strlen(dumps);
	if (len >= size) len = size-1;
	ft_strncpy(buf, dumps, len);
	buf[len] = '\0';
	SSL_FREE(dumps);
	return (buf);
}
