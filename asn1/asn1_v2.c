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
#define __SCHEMA_TYPE_NAME_ANY              ASN_V2_TYPE_NAME_ANY

#define __SCHEMA_CONSTRAINT_TYPE_NAME_RANGE ASN_V2_CONSTRAINT_TYPE_NAME_RANGE
#define __SCHEMA_CONSTRAINT_TYPE_NAME_SIZE  ASN_V2_CONSTRAINT_TYPE_NAME_SIZE

#define __JQ_SCHEMA_TAG_MODE        ".tagMode"
#define __JQ_SCHEMA_TYPES           ".types"

#define __JQ_COMPONENT_ID 		    ".id"
#define __JQ_COMPONENT_TYPE 	    ".type"
#define __JQ_COMPONENT_OPTIONAL	    ".optional"
#define __JQ_COMPONENT_DEFAULT	    ".default"

#define __JQ_TYPE_KIND				".kind"
#define __JQ_TYPE_TAGS	    		".tags"
#define __JQ_TYPE_CONSTRAINTS	    ".constraints"
#define __JQ_TYPE_DEFINED_BY_ID		".definedById"
#define __JQ_TYPE_COMPONENT_TYPE	".componentType"
#define __JQ_TYPE_COMPONENTS		".components"

#define __JQ_TAG_MODE				".mode"
#define __JQ_TAG_CLASS				".class"
#define __JQ_TAG_NUMBER	 			".number"

#define __JQ_CONSTRAINT_TYPE		".type"
#define __JQ_CONSTRAINT_MIN			".min"
#define __JQ_CONSTRAINT_MAX			".max"

const char *asn1_v2_get_type_kind_name(t_asn_v2_type_kind type)
{
	switch (type) {
	case ASN_V2_TYPE_KIND_INTEGER:          return ASN_V2_TYPE_NAME_INTEGER;
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

	if (ft_streq(name, __SCHEMA_TYPE_NAME_INTEGER))			    return (ASN_V2_TYPE_KIND_INTEGER);
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
	case ASN_V2_TYPE_KIND_INTEGER:
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
	case ASN_V2_TYPE_KIND_ANY:
		return (SSL_OK);
	default:
		return (SSL_ERR);
	}
}

static int __asn1_v2_schema_validate_tag(const t_json_v2 *jschema, const t_json_v2 *jtag);
static int __asn1_v2_schema_validate_constraint(const t_json_v2 *jschema, const t_json_v2 *jconstraint);
static int __asn1_v2_schema_validate_type(const t_json_v2 *jschema, const t_json_v2 *jtype);
static int __asn1_v2_schema_validate_component(const t_json_v2 *jschema, const t_json_v2 *jcomponent);

static int __asn1_v2_schema_validate_constraint(const t_json_v2 *jschema, const t_json_v2 *jconstraint)
{
	char cbuf[1024];

	if (jconstraint->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid asn1 constraint: expected json object but got json %s: %s", json_v2_get_type_name(jconstraint->type), json_v2_dumpb(jconstraint, cbuf, sizeof(cbuf)));
		return (SSL_ERR);
	}

	const t_json_v2 *jtype = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_CONSTRAINT_TYPE, jconstraint, &jtype)) {
		SSL_LOG(ERROR, "invalid asn1 constraint: expected `%s` key: %s", __JQ_CONSTRAINT_TYPE, json_v2_dumpb(jconstraint, cbuf, sizeof(cbuf)));
		return (SSL_ERR);
	}
	if (jtype->type != JSON_V2_TYPE_STRING) {
		SSL_LOG(ERROR, "invalid asn1 constraint: expected `%s` to be a json string but got json %s: %s", __JQ_CONSTRAINT_TYPE, json_v2_get_type_name(jtype->type), json_v2_dumpb(jconstraint, cbuf, sizeof(cbuf)));
		return (SSL_ERR);
	}
	if (SSL_OK != __asn1_v2_schema_validate_constraint_type_name(jconstraint->as.cstring)) {
		SSL_LOG(ERROR, "invalid asn1 constraint: bad type `%s`: %s", jconstraint->as.cstring, json_v2_dumpb(jconstraint, cbuf, sizeof(cbuf)));
		return (SSL_ERR);
	}
	const t_json_v2 *jmin = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_CONSTRAINT_MIN, jconstraint, &jmin)) {
		if (jmin->type != JSON_V2_TYPE_NUMBER) {
			SSL_LOG(ERROR, "invalid asn1 constraint: expected `%s` to be a json number but got json %s: %s", __JQ_CONSTRAINT_MIN, json_v2_get_type_name(jmin->type), json_v2_dumpb(jconstraint, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
	}
	const t_json_v2 *jmax = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_CONSTRAINT_MAX, jconstraint, &jmax)) {
		if (jmax->type != JSON_V2_TYPE_NUMBER) {
			SSL_LOG(ERROR, "invalid asn1 constraint: expected `%s` to be a json number but got json %s: %s", __JQ_CONSTRAINT_MAX, json_v2_get_type_name(jmax->type), json_v2_dumpb(jconstraint, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
	}
	if (NULL == jmin && NULL == jmax) {
		SSL_LOG(ERROR, "invalid asn1 constraint: no limits defined: %s", json_v2_dumpb(jconstraint, cbuf, sizeof(cbuf)));
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int __asn1_v2_schema_validate_tag(const t_json_v2 *jschema, const t_json_v2 *jtag)
{
	char cbuf[1024];

	if (jtag->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid asn1 tag: expected json object but got json %s: %s", json_v2_get_type_name(jtag->type), json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
		return (SSL_ERR);
	}

	const t_json_v2 *jmode = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TAG_MODE, jtag, &jmode)) {
		if (jmode->type != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid asn1 tag: expected `%s` to be a json string but got json %s: %s", __JQ_TAG_MODE, json_v2_get_type_name(jmode->type), json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		if (SSL_OK != __asn1_v2_schema_validate_tag_mode_name(jmode->as.cstring)) {
			SSL_LOG(ERROR, "invalid asn1 tag: bad tag mode `%s`: %s", jmode->as.cstring, json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
	}
	const t_json_v2 *jclass = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TAG_CLASS, jtag, &jclass)) {
		SSL_LOG(ERROR, "invalid asn1 tag: expected `%s` key: %s", __JQ_TAG_CLASS, json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
		return (SSL_ERR);
	}
	else {
		if (jclass->type != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid asn1 tag: expected `%s` to be a json string but got json %s: %s", __JQ_TAG_CLASS, json_v2_get_type_name(jclass->type), json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		if (SSL_OK != __asn1_v2_schema_validate_tag_class_name(jclass->as.cstring)) {
			SSL_LOG(ERROR, "invalid asn1 tag: bad tag class: `%s`: %s", jclass->as.cstring, json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
	}
	const t_json_v2 *jnumber = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TAG_NUMBER, jtag, &jnumber)) {
		SSL_LOG(ERROR, "invalid asn1 tag: expected `%s` key: %s", __JQ_TAG_NUMBER, json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
		return (SSL_ERR);
	}
	else {
		if (jnumber->type != JSON_V2_TYPE_NUMBER) {
			SSL_LOG(ERROR, "invalid asn1 tag: expected `%s` to be a json number but got json %s: %s", __JQ_TAG_NUMBER, json_v2_get_type_name(jnumber->type), json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
	}
	return (SSL_OK);
}

static int __asn1_v2_schema_validate_type(const t_json_v2 *jschema, const t_json_v2 *jtype)
{
	char cbuf[1024];

	if (jtype->type != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid asn1 type: expected json object but got json %s: %s", json_v2_get_type_name(jtype->type), json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
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
			SSL_LOG(ERROR, "asn1 type: no `%s` key specified: %s", __JQ_TYPE_COMPONENTS, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		else {
			void *content = NULL;
			t_list_next next = {0};
			while (ft_list_next_content(&jcomponents->as.list, &next, &content)) {
				if (SSL_OK != __asn1_v2_schema_validate_component(jschema, content)) {
					SSL_LOG(ERROR, "invalid asn1 type: `%s` contains invalid component: %s", __JQ_TYPE_COMPONENTS, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
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
	case ASN_V2_TYPE_KIND_ANY:
		;;
		const t_json_v2 *jdefined_by_id = NULL;
		if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_DEFINED_BY_ID, jtype, &jdefined_by_id)) {
			if (jdefined_by_id->type != JSON_V2_TYPE_STRING) {
				SSL_LOG(ERROR, "invalid asn1 type: expected `%s` to be a json string but got json %s: %s", __JQ_TYPE_DEFINED_BY_ID, json_v2_get_type_name(jdefined_by_id->type), json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
				return (SSL_ERR);
			}
		}
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INTEGER:
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

	const t_json_v2 *jtags = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_TAGS, jtype, &jtags)) {
		if (jtags->type != JSON_V2_TYPE_ARRAY) {
			SSL_LOG(ERROR, "invalid asn1 type: expected `%s` to be a json array but got json %s: %s", __JQ_TYPE_TAGS, json_v2_get_type_name(jtags->type), json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&jtags->as.list, &next, &content)) {
			if (SSL_OK != __asn1_v2_schema_validate_tag(jschema, content)) {
				SSL_LOG(ERROR, "invalid asn1 type: `%s` contains invalid tag: %s", __JQ_TYPE_TAGS, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
				return (SSL_ERR);
			}
		}
	}

	const t_json_v2 *jconstraints = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_CONSTRAINTS, jtype, &jconstraints)) {
		if (jtags->type != JSON_V2_TYPE_ARRAY) {
			SSL_LOG(ERROR, "invalid asn1 type: expected `%s` to be a json array but got json %s: %s", __JQ_TYPE_CONSTRAINTS, json_v2_get_type_name(jconstraints->type), json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			return (SSL_ERR);
		}
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&jconstraints->as.list, &next, &content)) {
			if (SSL_OK != __asn1_v2_schema_validate_constraint(jschema, content)) {
				SSL_LOG(ERROR, "invalid asn1 type: `%s` contains invalid constraint: %s", __JQ_TYPE_CONSTRAINTS, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
				return (SSL_ERR);
			}
		}
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
	ft_sprintf(&dumps, "{\"mode\":\"%s\",\"class\":\"%s\",\"number\":%d}",
		asn1_v2_get_tag_mode_name(asn1_tag->mode),
		asn1_v2_get_tag_class_name(asn1_tag->class),
		asn1_tag->number
	);
	return (dumps);
}

static char *__asn1_v2_constraint_dumps(const t_asn_v2_constraint *asn1_constraint)
{
	if (NULL == asn1_constraint) return ft_strdup("null");

	t_ostring ostring = {0};

	switch (asn1_constraint->type) {
	case ASN_V2_CONSTRAINT_TYPE_RANGE:
		ft_ostr_append_cstr(&ostring, "{\"type\":\"range\"");
		if (NULL != asn1_constraint->as.range.min) {
			char *min_dumps = bnum_to_dec(asn1_constraint->as.range.min);
			ft_ostr_appendf(&ostring, ",\"min\":%s", min_dumps);
			SSL_FREE(min_dumps);
		} else {
			ft_ostr_append_cstr(&ostring, ",\"min\":null");
		}
		if (NULL != asn1_constraint->as.range.max) {
			char *max_dumps = bnum_to_dec(asn1_constraint->as.range.max);
			ft_ostr_appendf(&ostring, ",\"max\":%s", max_dumps);
			SSL_FREE(max_dumps);
		} else {
			ft_ostr_append_cstr(&ostring, ",\"max\":null");
		}
		ft_ostr_append_cstr(&ostring, "}");
		break;
	case ASN_V2_CONSTRAINT_TYPE_SIZE:
		ft_ostr_appendf(&ostring, "{\"type\":\"size\",\"min\":%zu,\"min\":%zu}", asn1_constraint->as.size.min, asn1_constraint->as.size.max);
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

	ft_ostr_appendf(&ostring, "{\"kind\":\"%s\"", asn1_v2_get_type_kind_name(asn1_type->kind));

	if (asn1_type->tags.size > 0) {
		ft_ostr_append_cstr(&ostring, ",\"tags\":[");
		t_list_next next = {0};
		void *content = NULL;
		size_t commas = 0;
		while (ft_list_next_content(&asn1_type->tags, &next, &content)) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *constraint_dumps = __asn1_v2_tag_dumps(content);
			ft_ostr_append_cstr(&ostring, constraint_dumps);
			SSL_FREE(constraint_dumps);
		}
		ft_ostr_append_cstr(&ostring, "]");
	}

	if (asn1_type->constraints.size > 0) {
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
	}

	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET:
		;;
		t_list_next next = {0};
		void *content = NULL;
		size_t commas = 0;
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
	case ASN_V2_TYPE_KIND_ANY:
		if (NULL != asn1_type->as.any.defined_by_id) {
			ft_ostr_appendf(&ostring, ",\"defined_by_id\":\"%s\"", asn1_type->as.any.defined_by_id);
		}
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INTEGER:
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
		ft_ostr_appendf(&ostring, "{\"id\":\"%s\"", asn1_component->id);
	} else {
		ft_ostr_append_cstr(&ostring, "{\"id\":null");
	}
	char *dumps = __asn1_v2_type_dumps(asn1_component->type);
	ft_ostr_appendf(&ostring, ",\"type\":%s", dumps);
	SSL_FREE(dumps);

	if (NULL != asn1_component->default_value) {
		dumps = __asn1_v2_value_dumps(asn1_component->default_value);
		ft_ostr_appendf(&ostring, ",\"default_value\":%s", dumps);
		SSL_FREE(dumps);
	}

	ft_ostr_appendf(&ostring, ",\"optional\":%s", asn1_component->optional ? "true" : "false");
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
		bnum_del(asn1_constraint->as.range.min);
		bnum_del(asn1_constraint->as.range.max);
		asn1_constraint->as.range.min = NULL;
		asn1_constraint->as.range.max = NULL;
		break;
	case ASN_V2_CONSTRAINT_TYPE_SIZE:
		asn1_constraint->as.size.min = 0;
		asn1_constraint->as.size.max = SIZE_MAX;
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
	case ASN_V2_TYPE_KIND_ANY:
		if (NULL != asn1_type->as.any.defined_by_id) SSL_FREE(asn1_type->as.any.defined_by_id);
		asn1_type->as.any.defined_by_id = NULL;
		break;
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INTEGER:
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
	ft_list_clear_all_content(&asn1_type->tags, __asn1_v2_tag_delete_adapter);
	ft_list_clear_all_content(&asn1_type->constraints, __asn1_v2_constraint_delete_adapter);
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
	case ASN_V2_TYPE_KIND_ANY:
		if (NULL != src->as.any.defined_by_id) dst->as.any.defined_by_id = ft_strdup(src->as.any.defined_by_id);
		break;
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INTEGER:
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

static int	__asn1_v2_schema_parse_tag(const t_asn_v2_module *asn1_module, t_asn_v2_tag **asn1_tag, const t_json_v2 *jschema, const t_json_v2 *jtag)
{
	assert(jtag->type == JSON_V2_TYPE_OBJECT);
	*asn1_tag = NULL;

	char cbuf[1024] = {0};

	t_asn_v2_tag *atag = __asn1_v2_tag_create();

	const t_json_v2 *jmode = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TAG_MODE, jtag, &jmode)) {
		assert(jmode->type == JSON_V2_TYPE_STRING);
		SSL_LOG(TRACE, "asn1 tag: `%s`: %s", __JQ_TAG_MODE, json_v2_dumpb(jmode, cbuf, sizeof(cbuf)));
		atag->mode = __asn1_v2_schema_get_tag_mode_by_name(jmode->as.cstring);
	}
	else {
		if (ASN_V2_TAG_MODE_AUTOMATIC == asn1_module->tag_mode) {
			// Use explicit tagging mode as default.
			atag->mode = ASN_V2_TAG_MODE_EXPLICIT;
		} else {
			// Use asn1 module's tagging mode as default.
			atag->mode = asn1_module->tag_mode;
		}
	}

	const t_json_v2 *jclass = NULL;
	int ret = json_v2_query_nonnull(__JQ_TAG_CLASS, jtag, &jclass);
	assert(JSON_V2_OK == ret && jclass->type == JSON_V2_TYPE_STRING);
	SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TAG_CLASS, json_v2_dumpb(jclass, cbuf, sizeof(cbuf)));
	atag->class = __asn1_v2_schema_get_tag_class_by_name(jclass->as.cstring);

	const t_json_v2 *jnumber = NULL;
	ret = json_v2_query_nonnull(__JQ_TAG_NUMBER, jtag, &jnumber);
	assert(JSON_V2_OK == ret && jnumber->type == JSON_V2_TYPE_NUMBER);
	SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TAG_NUMBER, json_v2_dumpb(jnumber, cbuf, sizeof(cbuf)));
	atag->number = (uint32_t)bnum_to_dig_u(&jnumber->as.number);

	*asn1_tag = atag;
	return (SSL_OK);
}

static int	__asn1_v2_schema_parse_constraint(const t_asn_v2_module *asn1_module, t_asn_v2_constraint **asn1_constraint, const t_json_v2 *jschema, const t_json_v2 *jconstraint)
{
	assert(jconstraint->type == JSON_V2_TYPE_OBJECT);
	*asn1_constraint = NULL;

	char cbuf[1024] = {0};

	t_asn_v2_constraint *aconstraint = __asn1_v2_constraint_create();

	const t_json_v2 *jtype = NULL;
	int ret = json_v2_query_nonnull(__JQ_CONSTRAINT_TYPE, jconstraint, &jtype);
	assert(jtype->type == JSON_V2_TYPE_STRING);
	SSL_LOG(TRACE, "asn1 constraint: `%s`: %s", __JQ_CONSTRAINT_TYPE, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
	aconstraint->type = __asn1_v2_schema_get_constraint_type_by_name(jtype->as.cstring);

	const t_json_v2 *jmin = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_CONSTRAINT_MIN, jconstraint, &jmin)) {
		assert(JSON_V2_OK == ret && jmin->type == JSON_V2_TYPE_NUMBER);
		SSL_LOG(TRACE, "asn1 constraint: `%s`: %s", __JQ_CONSTRAINT_MIN, json_v2_dumpb(jmin, cbuf, sizeof(cbuf)));
	}

	const t_json_v2 *jmax = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_CONSTRAINT_MAX, jconstraint, &jmax)) {
		assert(JSON_V2_OK == ret && jmax->type == JSON_V2_TYPE_NUMBER);
		SSL_LOG(TRACE, "asn1 constraint: `%s`: %s", __JQ_CONSTRAINT_MAX, json_v2_dumpb(jmax, cbuf, sizeof(cbuf)));
	}

	switch (aconstraint->type) {
	case ASN_V2_CONSTRAINT_TYPE_RANGE:
		if (NULL == jmin) aconstraint->as.range.min = bnum_clone(&jmin->as.number);
		if (NULL == jmax) aconstraint->as.range.max = bnum_clone(&jmax->as.number);
		break;
	case ASN_V2_CONSTRAINT_TYPE_SIZE:
		aconstraint->as.size.min = (NULL == jmin) ? (size_t)bnum_to_dig_u(&jmin->as.number) : 0;
		aconstraint->as.size.max = (NULL == jmax) ? (size_t)bnum_to_dig_u(&jmax->as.number) : SIZE_MAX;
		break;
	}

	*asn1_constraint = aconstraint;
	return (SSL_OK);
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
					SSL_LOG(ERROR, "failed to parse `%s` for asn1 type: %s", __JQ_TYPE_COMPONENTS, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
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
			SSL_LOG(ERROR, "failed to parse `%s` for asn1 type: %s", __JQ_TYPE_COMPONENT_TYPE, json_v2_dumpb(jelement_type, cbuf, sizeof(cbuf)));
				goto label_error;
			}
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
	case ASN_V2_TYPE_KIND_INTEGER:
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
			SSL_LOG(ERROR, "failed to parse `%s` type reference for asn1 type: %s", jkind->as.cstring, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		__asn1_v2_type_copy(ref_atype, atype);
		__asn1_v2_type_delete(ref_atype);
	}

	const t_json_v2 *jtags = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_TAGS, jtype, &jtags)) {
		assert(jtags->type == JSON_V2_TYPE_ARRAY);
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&jtags->as.list, &next, &content)) {
			t_asn_v2_tag *atag = NULL;
			if (SSL_OK != __asn1_v2_schema_parse_tag(asn1_module, &atag, jschema, content)) {
				SSL_LOG(ERROR, "failed to parse `%s` for asn1 type: %s", __JQ_TYPE_TAGS, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			ft_list_append_content(&atype->tags, atag);
		}
	}

	const t_json_v2 *jconstraints = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_CONSTRAINTS, jtype, &jconstraints)) {
		assert(jconstraints->type == JSON_V2_TYPE_ARRAY);
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&jconstraints->as.list, &next, &content)) {
			t_asn_v2_constraint *aconstraint = NULL;
			if (SSL_OK != __asn1_v2_schema_parse_constraint(asn1_module, &aconstraint, jschema, content)) {
				SSL_LOG(ERROR, "failed to parse `%s` for asn1 type: %s", __JQ_TYPE_CONSTRAINTS, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			ft_list_append_content(&atype->constraints, aconstraint);
		}
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
			if (component->type->tags.size > 0) has_tagged_component = true;
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
				t_asn_v2_tag *tag = __asn1_v2_tag_create();
				tag->mode = (asn1_type->kind == ASN_V2_TYPE_KIND_CHOICE) ? ASN_V2_TAG_MODE_IMPLICIT : ASN_V2_TAG_MODE_EXPLICIT;
				tag->class = ASN_V2_TAG_CLASS_CONTEXT_SPECIFIC;
				tag->number = tag_number;
				ft_list_append_content(&component->type->tags, tag);
				tag_number++;
			}
		}
		return (SSL_OK);
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF:
		return (__asn1_v2_type_compile_automatic_tags(asn1_module, asn1_type->as.collection.element_type));
	case ASN_V2_TYPE_KIND_ANY:
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INTEGER:
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

static t_der_v2_tag *__der_v2_tag_create(void);
static void __der_v2_tag_delete(t_der_v2_tag *der_tag);

static t_der_v2_value *__der_v2_value_create(void);
static void	__der_v2_value_copy(t_der_v2_value *src, t_der_v2_value *dst);
static void __der_v2_value_clear(t_der_v2_value *der_value);
static void __der_v2_value_delete(t_der_v2_value *der_value);

static t_der_v2_type *__der_v2_type_create(void);
static void __der_v2_type_delete(t_der_v2_type *der_type);

static t_der_v2_component *__der_v2_component_create(void);
static void __der_v2_component_delete(t_der_v2_component *der_component);

static inline void __der_v2_tag_delete_adapter(void *p) { __der_v2_tag_delete((t_der_v2_tag *)p); };
static inline void __der_v2_value_delete_adapter(void *p) { __der_v2_value_delete((t_der_v2_value *)p); };
static inline void __der_v2_type_delete_adapter(void *p) { __der_v2_type_delete((t_der_v2_type *)p); };
static inline void __der_v2_component_delete_adapter(void *p) { __der_v2_component_delete((t_der_v2_component *)p); };

static t_der_v2_tag *__der_v2_tag_create(void)
{
	t_der_v2_tag *der_tag = NULL;
	SSL_ALLOC(der_tag, sizeof(t_der_v2_tag));
	*der_tag = (t_der_v2_tag){0};
	return (der_tag);
}

static inline void __der_v2_tag_delete(t_der_v2_tag *der_tag)
{
	SSL_FREE(der_tag);
}

static inline bool __der_v2_tag_eq(const t_der_v2_tag *a, const t_der_v2_tag *b)
{
	// return (a->class == b->class && a->number == b->number && a->constructed == b->constructed);
	return (ft_memcmp(a, b, sizeof(t_der_v2_tag)) == 0);
}

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
	case ASN_V2_TYPE_KIND_ANY:
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INTEGER:
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

static t_der_v2_tag __der_v2_get_tag_universal(t_asn_v2_type_kind type)
{
	t_asn_v2_tag_class uclass = ASN_V2_TAG_CLASS_UNIVERSAL;
	switch (type) {
	case ASN_V2_TYPE_KIND_INTEGER:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_INTEGER, };
	case ASN_V2_TYPE_KIND_BOOLEAN:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_BOOLEAN, };
	case ASN_V2_TYPE_KIND_BIT_STRING:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_BIT_STRING, };
	case ASN_V2_TYPE_KIND_OCTET_STRING:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_OCTET_STRING, };
	case ASN_V2_TYPE_KIND_IA5_STRING:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_IA5_STRING, };
	case ASN_V2_TYPE_KIND_UTF8_STRING:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_UTF8_STRING, };
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_PRINTABLE_STRING, };
	case ASN_V2_TYPE_KIND_NULL:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_NULL, };
	case ASN_V2_TYPE_KIND_OBJECT_ID:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_OBJECT_ID, };
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_OBJECT_DESCR, };
	case ASN_V2_TYPE_KIND_SEQUENCE:
		return (t_der_v2_tag){ .class = uclass, .constructed = true, .number = ASN_V2_TAG_NUMBER_SEQUENCE, };
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
		return (t_der_v2_tag){ .class = uclass, .constructed = true, .number = ASN_V2_TAG_NUMBER_SEQUENCE_OF, };
	case ASN_V2_TYPE_KIND_SET:
		return (t_der_v2_tag){ .class = uclass, .constructed = true, .number = ASN_V2_TAG_NUMBER_SET, };
	case ASN_V2_TYPE_KIND_SET_OF:
		return (t_der_v2_tag){ .class = uclass, .constructed = true, .number = ASN_V2_TAG_NUMBER_SET_OF, };
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_ANY:
		return (t_der_v2_tag){0};
	}
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

	SSL_LOG(TRACE, "compiling `%s` asn1 type", asn1_v2_get_type_kind_name(asn1_type->kind));

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
				SSL_LOG(ERROR, "failed to compile `%s` asn1 type: %s", asn1_v2_get_type_kind_name(asn1_type->kind), __asn1_v2_type_dumpb(content, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			ft_list_append_content(&compiled->as.composite.elements, compiled_element);
		}
		compiled->kind = asn1_type->kind;
		break;
	case ASN_V2_TYPE_KIND_CHOICE:
		next = (t_list_next){0};
		content = NULL;
		while (ft_list_next_content(&asn1_type->as.composite.elements, &next, &content)) {
			t_der_v2_component *compiled_element = NULL;
			if (SSL_OK != __asn1_v2_component_compile(&compiled_element, content)) {
				SSL_LOG(ERROR, "failed to compile `%s` asn1 type: %s", asn1_v2_get_type_kind_name(asn1_type->kind), __asn1_v2_type_dumpb(content, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			if (NULL == compiled_element->id) {
				SSL_LOG(ERROR, "failed to compile `%s` asn1 type: missing id for choice element", asn1_v2_get_type_kind_name(asn1_type->kind));
				goto label_error;
			}
			if (NULL != compiled_element->default_value) {
				SSL_LOG(ERROR, "failed to compile `%s` asn1 type: unexpected default value for choice element", asn1_v2_get_type_kind_name(asn1_type->kind));
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
			SSL_LOG(ERROR, "failed to compile `%s` asn1 type: %s", asn1_v2_get_type_kind_name(asn1_type->kind), __asn1_v2_type_dumpb(asn1_type, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		compiled->kind = asn1_type->kind;
		break;
	case ASN_V2_TYPE_KIND_ANY:
		compiled->kind = asn1_type->kind;
		break;
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INTEGER:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		compiled->kind = asn1_type->kind;
		break;
	}

	// CHOICE and ANY do not have their own tag.
	if (ASN_V2_TYPE_KIND_CHOICE != asn1_type->kind && ASN_V2_TYPE_KIND_ANY != asn1_type->kind) {
		// Semantic tag is the one describing which type decoder should expect.
		const t_der_v2_tag semantic_tag = __der_v2_get_tag_universal(asn1_type->kind);
		// The innermost tag shall be the one that wraps the value of a type known to decoder.
		t_der_v2_tag *inner_tag = __der_v2_tag_create();
		*inner_tag = semantic_tag;
		// Tags are ordered from innermost (first element) to outermost (last element).
		ft_list_append_content(&compiled->tags, inner_tag);
	}

	// Compile additional tags that wrap the innermost tag, in order, if any.
	t_list_next next = {0};
	void *content = NULL;
	while (ft_list_next_content(&asn1_type->tags, &next, &content)) {
		t_asn_v2_tag *asn1_tag = content;
		if (ASN_V2_TAG_CLASS_UNIVERSAL == asn1_tag->class) {
			SSL_LOG(ERROR, "failed to compile `%s` asn1 type: cannot use reserved tag: %s", asn1_v2_get_type_kind_name(asn1_type->kind), __asn1_v2_tag_dumpb(asn1_tag, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		t_der_v2_tag *compiled_tag = NULL;
		switch (asn1_tag->mode) {
		case ASN_V2_TAG_MODE_AUTOMATIC:
			SSL_LOG(ERROR, "failed to compile `%s` asn1 type: unexpected automatic tag mode", asn1_v2_get_type_kind_name(asn1_type->kind));
			goto label_error;
		case ASN_V2_TAG_MODE_EXPLICIT:
			compiled_tag = __der_v2_tag_create();
			compiled_tag->class = asn1_tag->class;
			compiled_tag->number = asn1_tag->number;
			compiled_tag->constructed = true;
			ft_list_append_content(&compiled->tags, compiled_tag);
			break;
		case ASN_V2_TAG_MODE_IMPLICIT:
			if (ASN_V2_TYPE_KIND_CHOICE == asn1_type->kind || ASN_V2_TYPE_KIND_ANY == asn1_type->kind) {
				SSL_LOG(ERROR, "failed to compile `%s` asn1 type: implicit tags are not allowed for this type", asn1_v2_get_type_kind_name(asn1_type->kind));
				goto label_error;
			}
			compiled_tag = __der_v2_tag_create();
			compiled_tag->class = asn1_tag->class;
			compiled_tag->number = asn1_tag->number;
			// An implicit tag's constructedness is defined by the type it wraps.
			const t_der_v2_tag semantic_tag = __der_v2_get_tag_universal(asn1_type->kind);
			compiled_tag->constructed = semantic_tag.constructed;
			// An implicit tag replaces all underlying tags, including the innermost tag.
			ft_list_clear_all_content(&compiled->tags, __asn1_v2_tag_delete_adapter);
			ft_list_append_content(&compiled->tags, compiled_tag);
			break;
		}
	}

	ft_list_copy_all_content(&asn1_type->constraints, &compiled->constraints, __asn1_v2_constraint_copy_adapter);

	*der_type = compiled;
	return (SSL_OK);

label_error:
	__der_v2_type_delete(compiled);
	return (SSL_ERR);
}

/****************************************************************************/

static char *__der_v2_tag_dumps(const t_der_v2_tag *der_tag)
{
	if (NULL == der_tag) return ft_strdup("null");

	char *dumps = NULL;
	ft_sprintf(&dumps, "{\"class\":\"%s\",\"number\":%d,\"constructed\":%s}",
		asn1_v2_get_tag_class_name(der_tag->class),
		der_tag->number,
		der_tag->constructed ? "true" : "false"
	);
	return (dumps);
}

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
		;;
		char *ostring_dumps = NULL;
		ft_sprintf(&ostring_dumps, "\"<octet_string=%p,size=%zu>\"", &der_value->as.ostring, der_value->as.ostring.size);
		return ostring_dumps;
	case ASN_V2_VALUE_TYPE_BSTRING:
		;;
		char *bstring_dumps = NULL;
		ft_sprintf(&bstring_dumps, "\"<bit_string=%p,size=%zu>\"", &der_value->as.ostring, der_value->as.ostring.size);
		return bstring_dumps;
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
		char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
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
			ft_sprintf(&any_dumps, "{\"any\":{\"defined_by_id\":\"%s\",\"octets\":\"<octet_string=%p,size=%zu>\"}}", der_value->as.any.defined_by_id, &der_value->as.any.octets, der_value->as.any.octets.size);
		} else {
			ft_sprintf(&any_dumps, "{\"any\":{\"defined_by_id\":null,\"octets\":\"<octet_string=%p,size=%zu>\"}}", &der_value->as.any.octets, der_value->as.any.octets.size);
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
		ft_ostr_appendf(&ostring, "{\"id\":\"%s\"", der_component->id);
	} else {
		ft_ostr_append_cstr(&ostring, "{\"id\":null");
	}
	char *dumps = der_v2_type_dumps(der_component->type);
	ft_ostr_appendf(&ostring, ",\"type\":%s", dumps);
	SSL_FREE(dumps);

	if (NULL != der_component->default_value) {
		dumps = __der_v2_value_dumps(der_component->default_value);
		ft_ostr_appendf(&ostring, ",\"default_value\":%s", dumps);
		SSL_FREE(dumps);
	}

	ft_ostr_appendf(&ostring, ",\"optional\":%s", der_component->optional ? "true" : "false");
	ft_ostr_append_cstr(&ostring, "}");

	dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);

	return (dumps);
}

char *der_v2_value_dumps(const t_der_v2_value *der_value)
{
	return (__der_v2_value_dumps(der_value));
}

char *der_v2_type_dumps(const t_der_v2_type *der_type)
{
	if (NULL == der_type) return ft_strdup("null");

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);

	ft_ostr_appendf(&ostring, "{\"kind\":\"%s\"", asn1_v2_get_type_kind_name(der_type->kind));

	if (der_type->tags.size > 0) {
		ft_ostr_append_cstr(&ostring, ",\"tags\":[");
		t_list_next next = {0};
		void *content = NULL;
		size_t commas = 0;
		while (ft_list_next_content(&der_type->tags, &next, &content)) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *tag_dumps = __der_v2_tag_dumps(content);
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
	case ASN_V2_TYPE_KIND_ANY:
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INTEGER:
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


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

typedef struct s_der_v2_tlv {
	t_der_v2_tag tag;
	size_t length;
	const uint8_t *value;
} t_der_v2_tlv;

static const t_asn_v2_tag_class DER_V2_TAG_CLASS_MAP[] = {
	ASN_V2_TAG_CLASS_UNIVERSAL,
	ASN_V2_TAG_CLASS_APPLICATION,
	ASN_V2_TAG_CLASS_CONTEXT_SPECIFIC,
	ASN_V2_TAG_CLASS_PRIVATE,
};

typedef int (*t_func_der_v2_generic_decode)(t_der_v2_tlv tlv, t_der_v2_value **der_value);

static bool __der_v2_next_tlv(const uint8_t *enc, size_t encsize, t_der_v2_tlv *tlv);

static int __der_v2_generic_decode_value(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int __der_v2_generic_decode_choice(t_der_v2_tlv tlv, t_der_v2_value **der_value);

static int	__der_v2_generic_decode_ostring(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_bitstring(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_boolean(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_sequence(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_null(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_integer(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_object_id(t_der_v2_tlv tlv, t_der_v2_value **der_value);

static bool __der_v2_next_tlv(const uint8_t *enc, size_t encsize, t_der_v2_tlv *tlv)
{
	ssize_t rbytes = 0;
	uint8_t octet = 0;

	// Get next TLV position using pointer arithmetic.
	if (NULL != tlv->value && (tlv->value >= enc && tlv->value <= enc + encsize)) {
		ptrdiff_t diff = tlv->value - enc;
		if (diff + tlv->length < encsize) {
			enc = tlv->value + tlv->length;
			encsize -= (size_t)(diff + tlv->length);
		} else {
			return false;
		}
	}
	SSL_LOG(TRACE, "reading tlv: enc=%p, encsize=%zu", enc, encsize);

	if (encsize == 0) return false;
	octet = enc[rbytes++];

	tlv->tag.class = DER_V2_TAG_CLASS_MAP[octet & 0xC0];
	tlv->tag.constructed = (octet & 0x20) > 0;
	uint32_t number = octet & 0x1F;
	if (number == 0x1F) {
		octet = 0;
		number = 0;
		do {
			if (rbytes >= encsize) { SSL_LOG(ERROR, "bad tag read"); return false; }
			octet = enc[rbytes++];
			number <<= 7;
			number |= (uint32_t)octet & 0x7F;
		} while (octet & 0x80);
	}
	tlv->tag.number = number;

	if (rbytes >= encsize) { SSL_LOG(ERROR, "bad length read"); return false; }
	octet = enc[rbytes++];
	size_t length = octet;
	if (length & 0x80) {
		uint8_t lensize = octet & 0x7F;
		if (lensize > sizeof(length)) {
			SSL_LOG(ERROR, "encoding size exceeds supported max");
			return false;
		}
		length = 0;
		for (size_t i = 0; i < lensize; i++) {
			if (rbytes >= encsize) { SSL_LOG(ERROR, "bad length read"); return false; }
			octet = enc[rbytes++];
			length <<= 8;
			length |= (size_t)octet;
		}
	}
	tlv->length = length;

	if (rbytes + tlv->length > encsize) { SSL_LOG(ERROR, "bad content read"); return false; }
	tlv->value = enc + rbytes;

	SSL_LOG(TRACE, "tlv read: tag={class=%d,number=%d,constructed=%d}, length:%zu, content=%p", tlv->tag.class, tlv->tag.number, tlv->tag.constructed, tlv->length, tlv->value);
	return true;
}

static struct s_der_v2_generic_decode {
	t_func_der_v2_generic_decode f_decode;
	const char *name;
} DER_V2_GENERIC_DECODE_TYPE_MAP[] = {
	/* ASN_V2_TAG_NUMBER_EOC                = 0 */	{ NULL, "EOC" },
	/* ASN_V2_TAG_NUMBER_BOOLEAN            = 1 */	{ __der_v2_generic_decode_boolean, "BOOLEAN" },
	/* ASN_V2_TAG_NUMBER_INTEGER            = 2 */	{ __der_v2_generic_decode_integer, "INTEGER" },
	/* ASN_V2_TAG_NUMBER_BIT_STRING         = 3 */	{ __der_v2_generic_decode_bitstring, "BIT_STRING" },
	/* ASN_V2_TAG_NUMBER_OCTET_STRING       = 4 */	{ __der_v2_generic_decode_ostring, "OCTET_STRING" },
	/* ASN_V2_TAG_NUMBER_NULL               = 5 */	{ __der_v2_generic_decode_null, "NULL" },
	/* ASN_V2_TAG_NUMBER_OBJECT_ID          = 6 */	{ __der_v2_generic_decode_object_id, "OBJECT_ID" },
	/* ASN_V2_TAG_NUMBER_OBJECT_DESCR       = 7 */	{ NULL, "OBJECT_DESCR" },
	/* ASN_V2_TAG_NUMBER_EXTERNAL           = 8 */	{ NULL, "EXTERNAL" },
	/* ASN_V2_TAG_NUMBER_REAL               = 9 */	{ NULL, "REAL" },
	/* ASN_V2_TAG_NUMBER_ENUMERATED         = 10 */	{ NULL, "ENUMERATED" },
	/* ASN_V2_TAG_NUMBER_EMBEDDED_PDV       = 11 */	{ NULL, "EMBEDDED_PDV" },
	/* ASN_V2_TAG_NUMBER_UTF8_STRING        = 12 */	{ NULL, "UTF8_STRING" },
	/* ASN_V2_TAG_NUMBER_RELATIVE_OBJECT_ID = 13 */	{ NULL, "RELATIVE_OBJECT_ID" },
	/* ASN_V2_TAG_NUMBER_TIME               = 14 */	{ NULL, "TIME" },
	/* ASN_V2_TAG_NUMBER_RESERVED_15        = 15 */	{ NULL, "RESERVED_15" },
	/* ASN_V2_TAG_NUMBER_SEQUENCE(_OF)      = 16 */	{ __der_v2_generic_decode_sequence, "SEQUENCE" },
	/* ASN_V2_TAG_NUMBER_SET(_OF)           = 17 */	{ NULL, "SET" },
	/* ASN_V2_TAG_NUMBER_NUMERIC_STRING     = 18 */	{ NULL, "NUMERIC_STRING" },
	/* ASN_V2_TAG_NUMBER_PRINTABLE_STRING   = 19 */	{ NULL, "PRINTABLE_STRING" },
	/* ASN_V2_TAG_NUMBER_TELETEX_STRING     = 20 */	{ NULL, "TELETEX_STRING" },
	/* ASN_V2_TAG_NUMBER_VIDEOTEX_STRING    = 21 */	{ NULL, "VIDEOTEX_STRING" },
	/* ASN_V2_TAG_NUMBER_IA5_STRING         = 22 */	{ NULL, "IA5_STRING" },
	/* ASN_V2_TAG_NUMBER_UTC_TIME           = 23 */	{ NULL, "UTC_TIME" },
	/* ASN_V2_TAG_NUMBER_GENERALIZED_TIME   = 24 */	{ NULL, "GENERALIZED_TIME" },
	/* ASN_V2_TAG_NUMBER_GRAPHIC_STRING     = 25 */	{ NULL, "GRAPHIC_STRING" },
	/* ASN_V2_TAG_NUMBER_VISIBLE_STRING     = 26 */	{ NULL, "VISIBLE_STRING" },
	/* ASN_V2_TAG_NUMBER_GENERAL_STRING     = 27 */	{ NULL, "GENERAL_STRING" },
	/* ASN_V2_TAG_NUMBER_UNIVERSAL_STRING   = 28 */	{ NULL, "UNIVERSAL_STRING" },
	/* ASN_V2_TAG_NUMBER_CHARACTER_STRING   = 29 */	{ NULL, "CHARACTER_STRING" },
	/* ASN_V2_TAG_NUMBER_BMP_STRING         = 30 */	{ NULL, "BMP_STRING" },
	/* ASN_V2_TAG_NUMBER_DATE               = 31 */	{ NULL, "DATE" },
	/* ASN_V2_TAG_NUMBER_TIME_OF_DAY        = 32 */	{ NULL, "TIME_OF_DAY" },
	/* ASN_V2_TAG_NUMBER_DATE_TIME          = 33 */	{ NULL, "DATE_TIME" },
	/* ASN_V2_TAG_NUMBER_DURATION           = 34 */	{ NULL, "DURATION" },
	/* ASN_V2_TAG_NUMBER_OID_IRI            = 35 */	{ NULL, "OID_IRI" },
	/* ASN_V2_TAG_NUMBER_RELATIVE_OID_IRI   = 36 */	{ NULL, "RELATIVE_OID_IRI" },
};
static const size_t DER_V2_GENERIC_DECODE_TYPE_COUNT = sizeof(DER_V2_GENERIC_DECODE_TYPE_MAP)/sizeof(DER_V2_GENERIC_DECODE_TYPE_MAP[0]);

int der_v2_generic_decode(const uint8_t *encoded, size_t encsize, t_der_v2_value **der_value)
{
	if (NULL == encoded || NULL == der_value) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	// Read root TLV.
	t_der_v2_tlv root = {0};
	if (!__der_v2_next_tlv(encoded, encsize, &root)) {
		SSL_LOG(ERROR, "bad nested tlv read");
		return (SSL_ERR);
	}
	if (SSL_OK != __der_v2_generic_decode_value(root, der_value)) {
		SSL_LOG(ERROR, "bad decode");
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int __der_v2_generic_decode_value(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	bool done = false;
	while (!done) {
		SSL_LOG(TRACE, "tlv: tag={class=%d,number=%d,constructed=%d}, length:%zu, content=%p", tlv.tag.class, tlv.tag.number, tlv.tag.constructed, tlv.length, tlv.value);
		switch (tlv.tag.class) {
		case ASN_V2_TAG_CLASS_UNIVERSAL: {
			// This is must be a standard asn1 type and we should know how to decode the value.
			SSL_LOG(TRACE, "universal: standard asn1 type");
			if (tlv.tag.number >= DER_V2_GENERIC_DECODE_TYPE_COUNT) {
				SSL_LOG(ERROR, "universal: bad tag: unknown tag number: %lu", tlv.tag.number);
				goto label_error;
			}
			t_func_der_v2_generic_decode f_decode = DER_V2_GENERIC_DECODE_TYPE_MAP[tlv.tag.number].f_decode;
			if (NULL == f_decode) {
				SSL_LOG(ERROR, "universal: decoding for `%s` asn1 type is not implemented", DER_V2_GENERIC_DECODE_TYPE_MAP[tlv.tag.number].name);
				goto label_error;
			}
			SSL_LOG(TRACE, "universal: decoding standard asn1 type: %s", DER_V2_GENERIC_DECODE_TYPE_MAP[tlv.tag.number].name);
			if (SSL_OK != f_decode(tlv, der_value)) {
				SSL_LOG(ERROR, "universal: bad decode");
				goto label_error;
			}
			done = true;
			continue;
		}
		case ASN_V2_TAG_CLASS_APPLICATION: {
			SSL_LOG(ERROR, "application: no application scoped asn1 tags currently implemented", tlv.tag.number);
			goto label_error;
		}
		case ASN_V2_TAG_CLASS_CONTEXT_SPECIFIC: {
			if (tlv.tag.constructed) {
				// This is in external tag with nested TLV. Unwrap nested TLV.
				SSL_LOG(TRACE, "context-specific: nested tlv");
				t_der_v2_tlv nested_tlv = {0};
				if (!__der_v2_next_tlv(tlv.value, tlv.length, &nested_tlv)) {
					SSL_LOG(ERROR, "context-specific: bad nested tlv read");
					goto label_error;
				}
				tlv.value = nested_tlv.value;
				tlv.length = nested_tlv.length;
				continue;
			}
			else {
				SSL_LOG(TRACE, "context-specific: implicit tag");
				// We don't know how to decode the value and we have no type metadata.
				{
					t_der_v2_value *dvalue = __der_v2_value_create();
					dvalue->type = ASN_V2_VALUE_TYPE_ANY;
					ft_ostr_append(&dvalue->as.any.octets, tlv.value, tlv.length);
					*der_value = dvalue;
				}
				done = true;
				continue;
			}
		}
		case ASN_V2_TAG_CLASS_PRIVATE: {
			SSL_LOG(ERROR, "private: no private scoped asn1 tags currently implemented", tlv.tag.number);
			goto label_error;
		}}
	}
	return (SSL_OK);

label_error:
	return (SSL_ERR);
}

static int	__der_v2_generic_decode_sequence(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "sequence: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	t_der_v2_tlv element_tlv = {0};
	t_list element_values = {0};
	while (__der_v2_next_tlv(tlv.value, tlv.length, &element_tlv)) {
		t_der_v2_value *element_value = NULL;
		if (SSL_OK != __der_v2_generic_decode_value(element_tlv, &element_value)) {
			SSL_LOG(ERROR, "sequence: bad decode element");
			goto label_error;
		}
		assert(element_value != NULL);
		ft_list_append_content(&element_values, element_value);
	}
	// Compile the list of values.
	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_LIST;
	SSL_ALLOC(dvalue->as.array.items, element_values.size * sizeof(t_der_v2_value));
	dvalue->as.array.count = element_values.size;

	t_list_next next = {0};
	void *content = NULL;
	size_t idx = 0;
	while (ft_list_next_content(&element_values, &next, &content)) {
		__der_v2_value_copy(content, dvalue->as.array.items + idx);
		idx++;
	}
	ft_list_clear_all_content(&element_values, __der_v2_value_delete_adapter);

	SSL_LOG(TRACE, "sequence: done");
	*der_value = dvalue;
	return (SSL_OK);

label_error:
	ft_list_clear_all_content(&element_values, __der_v2_value_delete_adapter);
	return (SSL_ERR);
}

static int	__der_v2_generic_decode_ostring(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "octet string: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_OSTRING;
	if (tlv.length > 0) {
		ft_ostr_init_with_capacity(&dvalue->as.ostring, tlv.length);
		ft_ostr_append(&dvalue->as.ostring, tlv.value, tlv.length);
	} else {
		ft_ostr_init(&dvalue->as.ostring);
	}

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_bitstring(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "bit string: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	if (tlv.length < 1) {
		SSL_LOG(ERROR, "bit string: bad length");
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "bit string: unused bits: %u", tlv.value[0]);
	// TODO: check last value octet: unused octet bits must be unset.
	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_OSTRING;
	ft_ostr_init_with_capacity(&dvalue->as.ostring, tlv.length);
	ft_ostr_append(&dvalue->as.ostring, tlv.value, tlv.length);

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_boolean(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "boolean: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	if (tlv.length != 1) {
		SSL_LOG(ERROR, "boolean: bad length");
		return (SSL_ERR);
	}
	bool boolean = false;
	if (tlv.value[0] == 0xFF) {
		boolean = true;
	} else if (tlv.value[0] == 0x0) {
		boolean = false;
	} else {
		SSL_LOG(ERROR, "boolean: bad value");
		return (SSL_ERR);
	}
	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_NULL;
	dvalue->as.boolean = boolean;

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_null(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "null: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_NULL;

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_integer(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "integer: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_NUMBER;
	bnum_from_bytes_u(&dvalue->as.number, tlv.value, tlv.length);

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_object_id(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	if (tlv.length == 0) {
		SSL_LOG(ERROR, "bad length");
		return (SSL_ERR);
	}
	const uint8_t *enc = tlv.value;
	size_t encsize = tlv.length;

	uint32_t sub_ids[encsize + 1];
	int num_sub_ids = 0;

	for (size_t i = 0; i < encsize; ) {
		sub_ids[num_sub_ids] = 0;
		// Get 7-bit blocks, up to the last one
		while (i < encsize && enc[i] & 0x80) {
			sub_ids[num_sub_ids] <<= 7;
			sub_ids[num_sub_ids] |= enc[i] & 0x7F;
			i++;
		}
		// If we've reached the end of the encoded content, then we've got an invalid der encoding
		if (i >= encsize) {
			SSL_LOG(ERROR, "bad value");
			return (SSL_ERR);
		}
		// Get the last block
		sub_ids[num_sub_ids] <<= 7;
		sub_ids[num_sub_ids] |= enc[i] & 0x7F;
		i++;
		num_sub_ids++;
	}
	if (num_sub_ids < 2) {
		SSL_LOG(ERROR, "bad value");
		return (SSL_ERR);
	}
	//	First two ids are concatenated into one single id using following formula:
	//	CONCAT_ID = 40 * ID_0 + ID_1
	char *sub_id_strings[encsize + 1];
	ft_sprintf(&sub_id_strings[0], "%lu.", sub_ids[0] / 40);
	ft_sprintf(&sub_id_strings[1], "%lu.", sub_ids[0] % 40);

	// Get the rest of ids, except the last one
	for (int i = 1; i < num_sub_ids-1; i++) {
		ft_sprintf(&sub_id_strings[i+1], "%lu.", sub_ids[i]);
	}
	// Get the last id
	ft_sprintf(&sub_id_strings[num_sub_ids], "%lu", sub_ids[num_sub_ids-1]);
	// Join all sub-id strings into an object id string
	char *obj_id = ft_2darray_strjoin(sub_id_strings, num_sub_ids + 1, "");
	SSL_LOG(TRACE, "object id: %s", obj_id);

	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_CSTRING;
	dvalue->as.cstring = obj_id;

	*der_value = dvalue;
	return (SSL_OK);
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

typedef int (*t_func_der_v2_decode)(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);

static int __der_v2_decode_value(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int __der_v2_decode_choice(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);

static int	__der_v2_decode_ostring(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_bitstring(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_boolean(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_sequence(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_null(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_integer(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_object_id(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);

static struct s_der_v2_decode {
	t_func_der_v2_decode f_decode;
	const char *name;
} DER_V2_DECODE_TYPE_MAP[] = {
	/* ASN_V2_TAG_NUMBER_EOC                = 0 */	{ NULL, "EOC" },
	/* ASN_V2_TAG_NUMBER_BOOLEAN            = 1 */	{ __der_v2_decode_boolean, "BOOLEAN" },
	/* ASN_V2_TAG_NUMBER_INTEGER            = 2 */	{ __der_v2_decode_integer, "INTEGER" },
	/* ASN_V2_TAG_NUMBER_BIT_STRING         = 3 */	{ __der_v2_decode_bitstring, "BIT_STRING" },
	/* ASN_V2_TAG_NUMBER_OCTET_STRING       = 4 */	{ __der_v2_decode_ostring, "OCTET_STRING" },
	/* ASN_V2_TAG_NUMBER_NULL               = 5 */	{ __der_v2_decode_null, "NULL" },
	/* ASN_V2_TAG_NUMBER_OBJECT_ID          = 6 */	{ __der_v2_decode_object_id, "OBJECT_ID" },
	/* ASN_V2_TAG_NUMBER_OBJECT_DESCR       = 7 */	{ NULL, "OBJECT_DESCR" },
	/* ASN_V2_TAG_NUMBER_EXTERNAL           = 8 */	{ NULL, "EXTERNAL" },
	/* ASN_V2_TAG_NUMBER_REAL               = 9 */	{ NULL, "REAL" },
	/* ASN_V2_TAG_NUMBER_ENUMERATED         = 10 */	{ NULL, "ENUMERATED" },
	/* ASN_V2_TAG_NUMBER_EMBEDDED_PDV       = 11 */	{ NULL, "EMBEDDED_PDV" },
	/* ASN_V2_TAG_NUMBER_UTF8_STRING        = 12 */	{ NULL, "UTF8_STRING" },
	/* ASN_V2_TAG_NUMBER_RELATIVE_OBJECT_ID = 13 */	{ NULL, "RELATIVE_OBJECT_ID" },
	/* ASN_V2_TAG_NUMBER_TIME               = 14 */	{ NULL, "TIME" },
	/* ASN_V2_TAG_NUMBER_RESERVED_15        = 15 */	{ NULL, "RESERVED_15" },
	/* ASN_V2_TAG_NUMBER_SEQUENCE(_OF)      = 16 */	{ __der_v2_decode_sequence, "SEQUENCE" },
	/* ASN_V2_TAG_NUMBER_SET(_OF)           = 17 */	{ NULL, "SET" },
	/* ASN_V2_TAG_NUMBER_NUMERIC_STRING     = 18 */	{ NULL, "NUMERIC_STRING" },
	/* ASN_V2_TAG_NUMBER_PRINTABLE_STRING   = 19 */	{ NULL, "PRINTABLE_STRING" },
	/* ASN_V2_TAG_NUMBER_TELETEX_STRING     = 20 */	{ NULL, "TELETEX_STRING" },
	/* ASN_V2_TAG_NUMBER_VIDEOTEX_STRING    = 21 */	{ NULL, "VIDEOTEX_STRING" },
	/* ASN_V2_TAG_NUMBER_IA5_STRING         = 22 */	{ NULL, "IA5_STRING" },
	/* ASN_V2_TAG_NUMBER_UTC_TIME           = 23 */	{ NULL, "UTC_TIME" },
	/* ASN_V2_TAG_NUMBER_GENERALIZED_TIME   = 24 */	{ NULL, "GENERALIZED_TIME" },
	/* ASN_V2_TAG_NUMBER_GRAPHIC_STRING     = 25 */	{ NULL, "GRAPHIC_STRING" },
	/* ASN_V2_TAG_NUMBER_VISIBLE_STRING     = 26 */	{ NULL, "VISIBLE_STRING" },
	/* ASN_V2_TAG_NUMBER_GENERAL_STRING     = 27 */	{ NULL, "GENERAL_STRING" },
	/* ASN_V2_TAG_NUMBER_UNIVERSAL_STRING   = 28 */	{ NULL, "UNIVERSAL_STRING" },
	/* ASN_V2_TAG_NUMBER_CHARACTER_STRING   = 29 */	{ NULL, "CHARACTER_STRING" },
	/* ASN_V2_TAG_NUMBER_BMP_STRING         = 30 */	{ NULL, "BMP_STRING" },
	/* ASN_V2_TAG_NUMBER_DATE               = 31 */	{ NULL, "DATE" },
	/* ASN_V2_TAG_NUMBER_TIME_OF_DAY        = 32 */	{ NULL, "TIME_OF_DAY" },
	/* ASN_V2_TAG_NUMBER_DATE_TIME          = 33 */	{ NULL, "DATE_TIME" },
	/* ASN_V2_TAG_NUMBER_DURATION           = 34 */	{ NULL, "DURATION" },
	/* ASN_V2_TAG_NUMBER_OID_IRI            = 35 */	{ NULL, "OID_IRI" },
	/* ASN_V2_TAG_NUMBER_RELATIVE_OID_IRI   = 36 */	{ NULL, "RELATIVE_OID_IRI" },
};
static const size_t DER_V2_DECODE_TYPE_COUNT = sizeof(DER_V2_DECODE_TYPE_MAP)/sizeof(DER_V2_DECODE_TYPE_MAP[0]);

int der_v2_decode(const uint8_t *encoded, size_t encsize, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	if (NULL == encoded || NULL == der_type || NULL == der_value) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	// Read root TLV.
	t_der_v2_tlv root = {0};
	if (!__der_v2_next_tlv(encoded, encsize, &root)) {
		SSL_LOG(ERROR, "bad nested tlv read");
		return (SSL_ERR);
	}
	if (SSL_OK != __der_v2_decode_value(root, der_type, der_value)) {
		SSL_LOG(ERROR, "bad decode");
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int __der_v2_decode_value(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	// Choice is a special case: value type can be one of the alternative types.
	if (ASN_V2_TYPE_KIND_CHOICE == der_type->kind) {
		return (__der_v2_decode_choice(tlv, der_type, der_value));
	}
	// ANY is a special case: value type can be of any type. We must use generic decoder.
	if (ASN_V2_TYPE_KIND_ANY == der_type->kind) {
		SSL_LOG(TRACE, "using generic decoder for type ANY");
		return (__der_v2_generic_decode_value(tlv, der_value));
	}

	// Get a list of tag references from metadata in reverse order: outermost to innermost tag.
	t_list tags = {0};
	t_list_next next = {0};
	void *content = NULL;
	while (ft_list_next_content(&der_type->tags, &next, &content)) {
		ft_list_prepend_ref(&tags, content);
	}

	bool done = false;
	while (!done) {
		SSL_LOG(TRACE, "tlv: tag={class=%d,number=%d,constructed=%d}, length:%zu, content=%p", tlv.tag.class, tlv.tag.number, tlv.tag.constructed, tlv.length, tlv.value);
		SSL_LOG(TRACE, "metadata type: kind: %s", asn1_v2_get_type_kind_name(der_type->kind));
		// Use type metadata to validate the TLV tag.
		const t_der_v2_tag *meta_tag = ft_list_pop_ref(&tags);
		if (NULL != meta_tag) {
			SSL_LOG(TRACE, "type metadata: tag={class=%d,number=%d,constructed=%d}", meta_tag->class, meta_tag->number, meta_tag->constructed);
		} else {
			SSL_LOG(ERROR, "type metadata: missing tag");
			goto label_error;
		}
		if (!__der_v2_tag_eq(&tlv.tag, meta_tag)) {
			SSL_LOG(ERROR, "type metadata: tag mismatch");
			goto label_error;
		}
		// Decode the value in TLV.
		switch (tlv.tag.class) {
		case ASN_V2_TAG_CLASS_UNIVERSAL: {
			// This is must be a standard asn1 type and we should know how to decode the value.
			SSL_LOG(TRACE, "universal: standard asn1 type");
			if (tlv.tag.number >= DER_V2_DECODE_TYPE_COUNT) {
				SSL_LOG(ERROR, "universal: bad tag: unknown tag number: %lu", tlv.tag.number);
				goto label_error;
			}
			t_func_der_v2_decode f_decode = DER_V2_DECODE_TYPE_MAP[tlv.tag.number].f_decode;
			if (NULL == f_decode) {
				SSL_LOG(ERROR, "universal: decoding for `%s` asn1 type is not implemented", asn1_v2_get_type_kind_name(der_type->kind));
				goto label_error;
			}
			SSL_LOG(TRACE, "universal: decoding standard asn1 type: %s", DER_V2_DECODE_TYPE_MAP[tlv.tag.number].name);
			if (SSL_OK != f_decode(tlv, der_type, der_value)) {
				SSL_LOG(ERROR, "universal: bad decode");
				goto label_error;
			}
			done = true;
			continue;
		}
		case ASN_V2_TAG_CLASS_APPLICATION: {
			SSL_LOG(ERROR, "application: no application scoped asn1 tags currently implemented", tlv.tag.number);
			goto label_error;
		}
		case ASN_V2_TAG_CLASS_CONTEXT_SPECIFIC: {
			if (tlv.tag.constructed) {
				// This is in external tag with nested TLV. Unwrap nested TLV.
				SSL_LOG(TRACE, "context-specific: nested tlv");
				t_der_v2_tlv nested_tlv = {0};
				if (!__der_v2_next_tlv(tlv.value, tlv.length, &nested_tlv)) {
					SSL_LOG(ERROR, "context-specific: bad nested tlv read");
					goto label_error;
				}
				tlv.value = nested_tlv.value;
				tlv.length = nested_tlv.length;
				continue;
			}
			else {
				SSL_LOG(TRACE, "context-specific: implicit tag");
				// Must be an implicit tag. Certain types cannot have implicit tags.
				assert(ASN_V2_TYPE_KIND_CHOICE != der_type->kind && ASN_V2_TYPE_KIND_ANY != der_type->kind);
				// Context specific implicit tags wrap raw value and we can't infer the actual type from the tag.
				// We must use type metadata to know how to decode the value.
				assert(NULL != meta_tag);
				tlv.tag = *meta_tag;
				continue;
			}
		}
		case ASN_V2_TAG_CLASS_PRIVATE: {
			SSL_LOG(ERROR, "private: no private scoped asn1 tags currently implemented", tlv.tag.number);
			goto label_error;
		}}
	}
	ft_list_clear_all_ref(&tags);
	return (SSL_OK);

label_error:
	ft_list_clear_all_ref(&tags);
	return (SSL_ERR);
}

static int __der_v2_decode_choice(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "choice: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	t_der_v2_value *chosen_value = NULL;
	t_der_v2_component *chosen_element = NULL;
	t_list_next next = {0};
	void *content = NULL;
	while (ft_list_next_content(&der_type->as.composite.elements, &next, &content)) {
		t_der_v2_component *element = content;
		SSL_LOG(TRACE, "choice: checking element with id: %s", element->id);
		// TODO: choice element type cannot be ANY, ensure type compilation handles this before decoding.
		assert(ASN_V2_TYPE_KIND_ANY == element->type->kind);
		// Element type can also be a choice.
		if (ASN_V2_TYPE_KIND_CHOICE == element->type->kind) {
			if (SSL_OK == __der_v2_decode_choice(tlv, element->type, &chosen_value)) {
				chosen_element = element;
				break;
			}
		}
		// Choice element type must have its own tag unless the element is also a choice.
		if (element->type->tags.size > 0) {
			SSL_LOG(ERROR, "choice: bad type metadata");
			return (SSL_ERR);
		}
		// Compare element type's outermost tag with TLV tag.
		if (__der_v2_tag_eq(&tlv.tag, ft_list_last_content(&element->type->tags))) {
			if (SSL_OK == __der_v2_decode_value(tlv, element->type, &chosen_value)) {
				chosen_element = element;
				break;
			} else {
				SSL_LOG(ERROR, "choice: bad decode");
				return (SSL_ERR);
			}
		}
	}
	if (NULL == chosen_element) {
		SSL_LOG(ERROR, "choice: bad tlv");
		return (SSL_ERR);
	}
	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_CHOICE;
	dvalue->as.choice.id = chosen_element->id;
	dvalue->as.choice.value = chosen_value;

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_decode_sequence(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "sequence: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	t_der_v2_tlv element_tlv = {0};
	t_list element_values = {0};
	t_list_next next = {0};
	void *content = NULL;

	if (!__der_v2_next_tlv(tlv.value, tlv.length, &element_tlv)) {
		SSL_LOG(ERROR, "sequence: bad element tlv read");
		goto label_error;
	}
	while (true) {
		// Get element type metadata.
		if (!ft_list_next_content(&der_type->as.composite.elements, &next, &content)) {
			SSL_LOG(ERROR, "sequence: bad element metadata read");
			goto label_error;
		}
		t_der_v2_component *element = content;
		// Decode the element value.
		SSL_LOG(TRACE, "sequence: decoding element with id: %s", element->id);
		t_der_v2_value *element_value = NULL;
		if (SSL_OK != __der_v2_decode_value(element_tlv, element->type, &element_value)) {
			if (element->optional) {
				// Retry with next element type metadata;
				continue;
			} else {
				SSL_LOG(ERROR, "sequence: bad decode element");
				goto label_error;
			}
		}
		assert(element_value != NULL);
		ft_list_append_content(&element_values, element_value);
		// Get next TLV.
		if (!__der_v2_next_tlv(tlv.value, tlv.length, &element_tlv)) {
			break;
		}
	}
	// Compile the list of values.
	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_LIST;
	SSL_ALLOC(dvalue->as.array.items, element_values.size * sizeof(t_der_v2_value));
	dvalue->as.array.count = element_values.size;
	next = (t_list_next){0};
	size_t idx = 0;
	while (ft_list_next_content(&element_values, &next, &content)) {
		__der_v2_value_copy(content, dvalue->as.array.items + idx);
		idx++;
	}
	ft_list_clear_all_content(&element_values, __der_v2_value_delete_adapter);

	SSL_LOG(TRACE, "sequence: done");
	*der_value = dvalue;
	return (SSL_OK);

label_error:
	ft_list_clear_all_content(&element_values, __der_v2_value_delete_adapter);
	return (SSL_ERR);
}

static int	__der_v2_decode_ostring(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "octet string: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	UNUSED(der_type);
	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_OSTRING;
	if (tlv.length > 0) {
		ft_ostr_init_with_capacity(&dvalue->as.ostring, tlv.length);
		ft_ostr_append(&dvalue->as.ostring, tlv.value, tlv.length);
	} else {
		ft_ostr_init(&dvalue->as.ostring);
	}

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_decode_bitstring(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "bit string: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	UNUSED(der_type);
	if (tlv.length < 1) {
		SSL_LOG(ERROR, "bit string: bad length");
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "bit string: unused bits: %u", tlv.value[0]);
	// TODO: check last value octet: unused octet bits must be unset.
	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_OSTRING;
	ft_ostr_init_with_capacity(&dvalue->as.ostring, tlv.length);
	ft_ostr_append(&dvalue->as.ostring, tlv.value, tlv.length);

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_decode_boolean(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "boolean: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	UNUSED(der_type);
	if (tlv.length != 1) {
		SSL_LOG(ERROR, "boolean: bad length");
		return (SSL_ERR);
	}
	bool boolean = false;
	if (tlv.value[0] == 0xFF) {
		boolean = true;
	} else if (tlv.value[0] == 0x0) {
		boolean = false;
	} else {
		SSL_LOG(ERROR, "boolean: bad value");
		return (SSL_ERR);
	}
	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_NULL;
	dvalue->as.boolean = boolean;

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_decode_null(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "null: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	UNUSED(der_type);
	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_NULL;

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_decode_integer(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "integer: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	UNUSED(der_type);
	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_NUMBER;
	bnum_from_bytes_u(&dvalue->as.number, tlv.value, tlv.length);

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_decode_object_id(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	if (tlv.length == 0) {
		SSL_LOG(ERROR, "bad length");
		return (SSL_ERR);
	}
	const uint8_t *enc = tlv.value;
	size_t encsize = tlv.length;

	uint32_t sub_ids[encsize + 1];
	int num_sub_ids = 0;

	for (size_t i = 0; i < encsize; ) {
		sub_ids[num_sub_ids] = 0;
		// Get 7-bit blocks, up to the last one
		while (i < encsize && enc[i] & 0x80) {
			sub_ids[num_sub_ids] <<= 7;
			sub_ids[num_sub_ids] |= enc[i] & 0x7F;
			i++;
		}
		// If we've reached the end of the encoded content, then we've got an invalid der encoding
		if (i >= encsize) {
			SSL_LOG(ERROR, "bad value");
			return (SSL_ERR);
		}
		// Get the last block
		sub_ids[num_sub_ids] <<= 7;
		sub_ids[num_sub_ids] |= enc[i] & 0x7F;
		i++;
		num_sub_ids++;
	}
	if (num_sub_ids < 2) {
		SSL_LOG(ERROR, "bad value");
		return (SSL_ERR);
	}
	//	First two ids are concatenated into one single id using following formula:
	//	CONCAT_ID = 40 * ID_0 + ID_1
	char *sub_id_strings[encsize + 1];
	ft_sprintf(&sub_id_strings[0], "%lu.", sub_ids[0] / 40);
	ft_sprintf(&sub_id_strings[1], "%lu.", sub_ids[0] % 40);

	// Get the rest of ids, except the last one
	for (int i = 1; i < num_sub_ids-1; i++) {
		ft_sprintf(&sub_id_strings[i+1], "%lu.", sub_ids[i]);
	}
	// Get the last id
	ft_sprintf(&sub_id_strings[num_sub_ids], "%lu", sub_ids[num_sub_ids-1]);
	// Join all sub-id strings into an object id string
	char *obj_id = ft_2darray_strjoin(sub_id_strings, num_sub_ids + 1, "");
	SSL_LOG(TRACE, "object id: %s", obj_id);

	t_der_v2_value *dvalue = __der_v2_value_create();
	dvalue->type = ASN_V2_VALUE_TYPE_CSTRING;
	dvalue->as.cstring = obj_id;

	*der_value = dvalue;
	return (SSL_OK);
}
