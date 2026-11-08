#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <der.h>
#include <libft.h>
#include <bnum.h>

#include "__asn1_private.h"

/****************************************************************************/

#define ASN_V2_TAG_CLASS_NAME_UNIVERSAL     "UNIVERSAL"
#define ASN_V2_TAG_CLASS_NAME_APPLICATION   "APPLICATION"
#define ASN_V2_TAG_CLASS_NAME_CONTEXT       "CONTEXT-SPECIFIC"
#define ASN_V2_TAG_CLASS_NAME_PRIVATE       "PRIVATE"

#define ASN_V2_TAG_MODE_NAME_DEFAULT		"DEFAULT"
#define ASN_V2_TAG_MODE_NAME_EXPLICIT		"EXPLICIT"
#define ASN_V2_TAG_MODE_NAME_IMPLICIT		"IMPLICIT"
#define ASN_V2_TAG_MODE_NAME_AUTOMATIC		"AUTOMATIC"

#define ASN_V2_TYPE_NAME_NULL               "NULL"
#define ASN_V2_TYPE_NAME_BOOLEAN            "BOOLEAN"
#define ASN_V2_TYPE_NAME_INTEGER            "INTEGER"
#define ASN_V2_TYPE_NAME_ENUMERATED         "ENUMERATED"
#define ASN_V2_TYPE_NAME_BIT_STRING         "BIT STRING"
#define ASN_V2_TYPE_NAME_OCTET_STRING       "OCTET STRING"
#define ASN_V2_TYPE_NAME_IA5_STRING         "IA5 STRING"
#define ASN_V2_TYPE_NAME_UTF8_STRING        "UTF8 STRING"
#define ASN_V2_TYPE_NAME_PRINTABLE_STRING   "PRINTABLE STRING"
#define ASN_V2_TYPE_NAME_NUMERIC_STRING		"NUMERIC STRING"
#define ASN_V2_TYPE_NAME_VISIBLE_STRING  	"VISIBLE STRING"
#define ASN_V2_TYPE_NAME_UTC_TIME  			"UTC TIME"
#define ASN_V2_TYPE_NAME_OBJECT_ID          "OBJECT IDENTIFIER"
#define ASN_V2_TYPE_NAME_OBJECT_DESCR       "OBJECT DESCRIPTOR"
#define ASN_V2_TYPE_NAME_RELATIVE_OBJECT_ID "RELATIVE OBJECT IDENTIFIER"
#define ASN_V2_TYPE_NAME_SEQUENCE           "SEQUENCE"
#define ASN_V2_TYPE_NAME_SEQUENCE_OF        "SEQUENCE OF"
#define ASN_V2_TYPE_NAME_SET                "SET"
#define ASN_V2_TYPE_NAME_SET_OF             "SET OF"
#define ASN_V2_TYPE_NAME_CHOICE             "CHOICE"
#define ASN_V2_TYPE_NAME_ANY                "ANY"
#define ASN_V2_TYPE_NAME_GENERALIZED_TIME   "GENERALIZED TIME"

#define ASN_V2_CONSTRAINT_NAME_RANGE		"RANGE"
#define ASN_V2_CONSTRAINT_NAME_SIZE 		"SIZE"

const char *asn1_v2_get_type_name(t_asn_v2_type_kind type)
{
	switch (type) {
	case ASN_V2_TYPE_KIND_INTEGER:          	return ASN_V2_TYPE_NAME_INTEGER;
	case ASN_V2_TYPE_KIND_ENUMERATED:       	return ASN_V2_TYPE_NAME_ENUMERATED;
	case ASN_V2_TYPE_KIND_BOOLEAN:          	return ASN_V2_TYPE_NAME_BOOLEAN;
	case ASN_V2_TYPE_KIND_BIT_STRING:       	return ASN_V2_TYPE_NAME_BIT_STRING;
	case ASN_V2_TYPE_KIND_OCTET_STRING:     	return ASN_V2_TYPE_NAME_OCTET_STRING;
	case ASN_V2_TYPE_KIND_IA5_STRING:       	return ASN_V2_TYPE_NAME_IA5_STRING;
	case ASN_V2_TYPE_KIND_UTF8_STRING:      	return ASN_V2_TYPE_NAME_UTF8_STRING;
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING: 	return ASN_V2_TYPE_NAME_PRINTABLE_STRING;
	case ASN_V2_TYPE_KIND_NUMERIC_STRING: 		return ASN_V2_TYPE_NAME_NUMERIC_STRING;
	case ASN_V2_TYPE_KIND_VISIBLE_STRING: 		return ASN_V2_TYPE_NAME_VISIBLE_STRING;
	case ASN_V2_TYPE_KIND_UTC_TIME: 			return ASN_V2_TYPE_NAME_UTC_TIME;
	case ASN_V2_TYPE_KIND_NULL:             	return ASN_V2_TYPE_NAME_NULL;
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:	return ASN_V2_TYPE_NAME_RELATIVE_OBJECT_ID;
	case ASN_V2_TYPE_KIND_OBJECT_ID:        	return ASN_V2_TYPE_NAME_OBJECT_ID;
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:     	return ASN_V2_TYPE_NAME_OBJECT_DESCR;
	case ASN_V2_TYPE_KIND_SEQUENCE:         	return ASN_V2_TYPE_NAME_SEQUENCE;
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:      	return ASN_V2_TYPE_NAME_SEQUENCE_OF;
	case ASN_V2_TYPE_KIND_SET:              	return ASN_V2_TYPE_NAME_SET;
	case ASN_V2_TYPE_KIND_SET_OF:           	return ASN_V2_TYPE_NAME_SET_OF;
	case ASN_V2_TYPE_KIND_CHOICE:           	return ASN_V2_TYPE_NAME_CHOICE;
	case ASN_V2_TYPE_KIND_ANY:              	return ASN_V2_TYPE_NAME_ANY;
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:  	return ASN_V2_TYPE_NAME_GENERALIZED_TIME;
	default:                                	return "unknown";
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
	case ASN_V2_TAG_MODE_DEFAULT:    return ASN_V2_TAG_MODE_NAME_DEFAULT;
	case ASN_V2_TAG_MODE_EXPLICIT:   return ASN_V2_TAG_MODE_NAME_EXPLICIT;
	case ASN_V2_TAG_MODE_IMPLICIT:   return ASN_V2_TAG_MODE_NAME_IMPLICIT;
	case ASN_V2_TAG_MODE_AUTOMATIC:  return ASN_V2_TAG_MODE_NAME_AUTOMATIC;
	default:                         return "unknown";
	}
}

const char *asn1_v2_get_constraint_name(t_asn_v2_constraint_kind type)
{
	switch (type) {
	case ASN_V2_CONSTRAINT_KIND_RANGE:  return ASN_V2_CONSTRAINT_NAME_RANGE;
	case ASN_V2_CONSTRAINT_KIND_SIZE:   return ASN_V2_CONSTRAINT_NAME_SIZE;
	default:                            return "unknown";
	}
}

/****************************************************************************/

t_asn_v2_constraint_kind asn1_v2_get_constraint_type_by_name(const char *name)
{
	assert(NULL != name);

	if (ft_streq(name, ASN_V2_CONSTRAINT_NAME_RANGE))  return (ASN_V2_CONSTRAINT_KIND_RANGE);
	if (ft_streq(name, ASN_V2_CONSTRAINT_NAME_SIZE))   return (ASN_V2_CONSTRAINT_KIND_SIZE);

	return (-1);
}

t_asn_v2_type_kind asn1_v2_get_type_by_name(const char *name)
{
	assert(NULL != name);

	if (ft_streq(name, ASN_V2_TYPE_NAME_INTEGER))            return (ASN_V2_TYPE_KIND_INTEGER);
	if (ft_streq(name, ASN_V2_TYPE_NAME_ENUMERATED))         return (ASN_V2_TYPE_KIND_ENUMERATED);
	if (ft_streq(name, ASN_V2_TYPE_NAME_BOOLEAN))            return (ASN_V2_TYPE_KIND_BOOLEAN);
	if (ft_streq(name, ASN_V2_TYPE_NAME_BIT_STRING))         return (ASN_V2_TYPE_KIND_BIT_STRING);
	if (ft_streq(name, ASN_V2_TYPE_NAME_OCTET_STRING))       return (ASN_V2_TYPE_KIND_OCTET_STRING);
	if (ft_streq(name, ASN_V2_TYPE_NAME_IA5_STRING))         return (ASN_V2_TYPE_KIND_IA5_STRING);
	if (ft_streq(name, ASN_V2_TYPE_NAME_UTF8_STRING))        return (ASN_V2_TYPE_KIND_UTF8_STRING);
	if (ft_streq(name, ASN_V2_TYPE_NAME_PRINTABLE_STRING))   return (ASN_V2_TYPE_KIND_PRINTABLE_STRING);
	if (ft_streq(name, ASN_V2_TYPE_NAME_NUMERIC_STRING))     return (ASN_V2_TYPE_KIND_NUMERIC_STRING);
	if (ft_streq(name, ASN_V2_TYPE_NAME_VISIBLE_STRING))     return (ASN_V2_TYPE_KIND_VISIBLE_STRING);
	if (ft_streq(name, ASN_V2_TYPE_NAME_UTC_TIME))           return (ASN_V2_TYPE_KIND_UTC_TIME);
	if (ft_streq(name, ASN_V2_TYPE_NAME_GENERALIZED_TIME))   return (ASN_V2_TYPE_KIND_GENERALIZED_TIME);
	if (ft_streq(name, ASN_V2_TYPE_NAME_NULL))               return (ASN_V2_TYPE_KIND_NULL);
	if (ft_streq(name, ASN_V2_TYPE_NAME_RELATIVE_OBJECT_ID)) return (ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID);
	if (ft_streq(name, ASN_V2_TYPE_NAME_OBJECT_ID))          return (ASN_V2_TYPE_KIND_OBJECT_ID);
	if (ft_streq(name, ASN_V2_TYPE_NAME_OBJECT_DESCR))       return (ASN_V2_TYPE_KIND_OBJECT_DESCR);
	if (ft_streq(name, ASN_V2_TYPE_NAME_SEQUENCE))           return (ASN_V2_TYPE_KIND_SEQUENCE);
	if (ft_streq(name, ASN_V2_TYPE_NAME_SEQUENCE_OF))        return (ASN_V2_TYPE_KIND_SEQUENCE_OF);
	if (ft_streq(name, ASN_V2_TYPE_NAME_SET))                return (ASN_V2_TYPE_KIND_SET);
	if (ft_streq(name, ASN_V2_TYPE_NAME_SET_OF))             return (ASN_V2_TYPE_KIND_SET_OF);
	if (ft_streq(name, ASN_V2_TYPE_NAME_CHOICE))             return (ASN_V2_TYPE_KIND_CHOICE);
	if (ft_streq(name, ASN_V2_TYPE_NAME_ANY))                return (ASN_V2_TYPE_KIND_ANY);

	return (-1);
}

t_asn_v2_tag_mode asn1_v2_get_tag_mode_by_name(const char *name)
{
	assert(NULL != name);

	if (ft_streq(name, ASN_V2_TAG_MODE_NAME_EXPLICIT))     return (ASN_V2_TAG_MODE_EXPLICIT);
	if (ft_streq(name, ASN_V2_TAG_MODE_NAME_IMPLICIT))     return (ASN_V2_TAG_MODE_IMPLICIT);
	if (ft_streq(name, ASN_V2_TAG_MODE_NAME_AUTOMATIC))    return (ASN_V2_TAG_MODE_AUTOMATIC);

	return (-1);
}

t_asn_v2_tag_class asn1_v2_get_tag_class_by_name(const char *name)
{
	assert(NULL != name);

	if (ft_streq(name, ASN_V2_TAG_CLASS_NAME_UNIVERSAL))   return (ASN_V2_TAG_CLASS_UNIVERSAL);
	if (ft_streq(name, ASN_V2_TAG_CLASS_NAME_APPLICATION)) return (ASN_V2_TAG_CLASS_APPLICATION);
	if (ft_streq(name, ASN_V2_TAG_CLASS_NAME_CONTEXT))     return (ASN_V2_TAG_CLASS_CONTEXT_SPECIFIC);
	if (ft_streq(name, ASN_V2_TAG_CLASS_NAME_PRIVATE))     return (ASN_V2_TAG_CLASS_PRIVATE);

	return (-1);
}

/****************************************************************************/

t_asn_v2_symbol *asn1_v2_symbol_create(void)
{
	t_asn_v2_symbol *symbol = NULL;
	SSL_ALLOC(symbol, sizeof(t_asn_v2_symbol));
	*symbol = (t_asn_v2_symbol){0};
	return (symbol);
}

void asn1_v2_symbol_clear(t_asn_v2_symbol *asn1_symbol)
{
	switch (asn1_symbol->kind) {
	case ASN_V2_SYMBOL_KIND_TYPE_REF: {
		if (NULL != asn1_symbol->resolved.type_ref.type) asn1_v2_type_delete(asn1_symbol->resolved.type_ref.type);
		asn1_symbol->resolved.type_ref.type = NULL;
		break;
	}
	case ASN_V2_SYMBOL_KIND_VALUE_REF: {
		if (NULL != asn1_symbol->resolved.value_ref.type) asn1_v2_type_delete(asn1_symbol->resolved.value_ref.type);
		if (NULL != asn1_symbol->resolved.value_ref.value) asn1_v2_value_delete(asn1_symbol->resolved.value_ref.value);
		asn1_symbol->resolved.value_ref.type = NULL;
		asn1_symbol->resolved.value_ref.value = NULL;
		break;
	}}
	*asn1_symbol = (t_asn_v2_symbol){0};
}

void asn1_v2_symbol_delete(t_asn_v2_symbol *asn1_symbol)
{
	asn1_v2_symbol_clear(asn1_symbol);
	SSL_FREE(asn1_symbol);
}

void asn1_v2_symbol_copy(const t_asn_v2_symbol *src, t_asn_v2_symbol *dst)
{
	*dst = (t_asn_v2_symbol){0};
	dst->kind = src->kind;
	dst->state = src->state;
	dst->source = src->source;

	switch (src->kind) {
	case ASN_V2_SYMBOL_KIND_TYPE_REF: {
		if (NULL != src->resolved.type_ref.type) dst->resolved.type_ref.type = __asn1_v2_type_copy_adapter(src->resolved.type_ref.type);
		break;
	}
	case ASN_V2_SYMBOL_KIND_VALUE_REF: {
		if (NULL != src->resolved.value_ref.type) dst->resolved.value_ref.type = __asn1_v2_type_copy_adapter(src->resolved.value_ref.type);
		if (NULL != src->resolved.value_ref.value) dst->resolved.value_ref.value = __asn1_v2_value_copy_adapter(src->resolved.value_ref.value);
		break;
	}}
}

int	asn1_v2_symbol_register_type(t_asn_v2_symbol_table *symbol_table, const char *symbol_name, const t_json_v2 *source)
{
	t_asn_v2_symbol *symbol = asn1_v2_symbol_create();
	symbol->kind = ASN_V2_SYMBOL_KIND_TYPE_REF;
	symbol->source = source;
	if (!ft_htbl_v2_set(&symbol_table->symbols, symbol_name, symbol)) {
		SSL_LOG(ERROR, "redefintion of the symbol: %s", symbol_name);
		asn1_v2_symbol_delete(symbol);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

int	asn1_v2_symbol_register_value(t_asn_v2_symbol_table *symbol_table, const char *symbol_name, const t_json_v2 *source)
{
	t_asn_v2_symbol *symbol = asn1_v2_symbol_create();
	symbol->kind = ASN_V2_SYMBOL_KIND_VALUE_REF;
	symbol->source = source;
	if (!ft_htbl_v2_set(&symbol_table->symbols, symbol_name, symbol)) {
		SSL_LOG(ERROR, "redefintion of the symbol: %s", symbol_name);
		asn1_v2_symbol_delete(symbol);
		return (SSL_ERR);
	}

	return (SSL_OK);
}

/****************************************************************************/

t_asn_v2_tag *asn1_v2_tag_create(void)
{
	t_asn_v2_tag *tag = NULL;
	SSL_ALLOC(tag, sizeof(t_asn_v2_tag));
	*tag = (t_asn_v2_tag){0};
	return (tag);
}

void asn1_v2_tag_clear(t_asn_v2_tag *asn1_tag)
{
	*asn1_tag = (t_asn_v2_tag){0};
}

void asn1_v2_tag_delete(t_asn_v2_tag *asn1_tag)
{
	asn1_v2_tag_clear(asn1_tag);
	SSL_FREE(asn1_tag);
}

void	asn1_v2_tag_copy(const t_asn_v2_tag *src, t_asn_v2_tag *dst)
{
	*dst = *src;
}

t_asn_v2_enum *asn1_v2_enum_create(void)
{
	t_asn_v2_enum *item = NULL;
	SSL_ALLOC(item, sizeof(t_asn_v2_enum));
	*item = (t_asn_v2_enum){0};
	return (item);
}

void asn1_v2_enum_clear(t_asn_v2_enum *asn1_enum_item)
{
	SSL_FREE(asn1_enum_item->name);
	bnum_clear(&asn1_enum_item->number);
}

void asn1_v2_enum_delete(t_asn_v2_enum *asn1_enum_item)
{
	asn1_v2_enum_clear(asn1_enum_item);
	SSL_FREE(asn1_enum_item);
}

void	asn1_v2_enum_copy(const t_asn_v2_enum *src, t_asn_v2_enum *dst)
{
	if (NULL != src->name) dst->name = ft_strdup(src->name);
	bnum_copy(&src->number, &dst->number);
}

t_asn_v2_value *asn1_v2_value_create(void)
{
	t_asn_v2_value *asn1_value = NULL;
	SSL_ALLOC(asn1_value, sizeof(t_asn_v2_value));
	*asn1_value = (t_asn_v2_value){0};
	return (asn1_value);
}

void asn1_v2_value_clear(t_asn_v2_value *asn1_value)
{
	switch (asn1_value->kind) {
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_BOOLEAN: {
		break;
	}
	case ASN_V2_TYPE_KIND_INTEGER: {
		bnum_clear(&asn1_value->as.number);
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		asn1_v2_enum_clear(&asn1_value->as.enumerated);
		break;
	}
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_NUMERIC_STRING:
	case ASN_V2_TYPE_KIND_UTC_TIME:
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
	case ASN_V2_TYPE_KIND_VISIBLE_STRING: {
		SSL_FREE(asn1_value->as.cstring);
		asn1_value->as.cstring = NULL;
		break;
	}
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_ID: {
		ft_list_clear_all_content(&asn1_value->as.object_id.arcs, __asn1_v2_oid_arc_delete_adapter);
		break;
	}
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_OCTET_STRING: {
		ft_ostr_clear(&asn1_value->as.ostring);
		break;
	}
	case ASN_V2_TYPE_KIND_SET:
	case ASN_V2_TYPE_KIND_SET_OF:
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SEQUENCE_OF: {
		ft_list_clear_all_content(&asn1_value->as.composite.elements, __asn1_v2_value_delete_adapter);
		break;
	}
	case ASN_V2_TYPE_KIND_CHOICE: {
		if (NULL != asn1_value->as.choice.id) SSL_FREE(asn1_value->as.choice.id);
		if (NULL != asn1_value->as.choice.value) asn1_v2_value_delete(asn1_value->as.choice.value);
		asn1_value->as.choice.id = NULL;
		asn1_value->as.choice.value = NULL;
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		ft_ostr_clear(&asn1_value->as.any.octets);
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 value type: %d", asn1_value->kind);
	}}
}

void asn1_v2_value_delete(t_asn_v2_value *asn1_value)
{
	asn1_v2_value_clear(asn1_value);
	SSL_FREE(asn1_value);
}

void	asn1_v2_value_copy(const t_asn_v2_value *src, t_asn_v2_value *dst)
{
	dst->kind = src->kind;

	switch (src->kind) {
	case ASN_V2_TYPE_KIND_NULL: {
		break;
	}
	case ASN_V2_TYPE_KIND_BOOLEAN: {
		dst->as.boolean = src->as.boolean;
		break;
	}
	case ASN_V2_TYPE_KIND_INTEGER: {
		bnum_copy(&src->as.number, &dst->as.number);
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		asn1_v2_enum_copy(&src->as.enumerated, &dst->as.enumerated);
		break;
	}
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NUMERIC_STRING:
	case ASN_V2_TYPE_KIND_VISIBLE_STRING:
	case ASN_V2_TYPE_KIND_UTC_TIME:
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
	case ASN_V2_TYPE_KIND_IA5_STRING: {
		dst->as.cstring = ft_strdup(src->as.cstring);
		break;
	}
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_ID: {
		ft_list_copy_all_content(&src->as.object_id.arcs, &dst->as.object_id.arcs, __asn1_v2_oid_arc_copy_adapter);
		break;
	}
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_OCTET_STRING: {
		ft_ostr_copy(&src->as.ostring, &dst->as.ostring);
		break;
	}
	case ASN_V2_TYPE_KIND_SET:
	case ASN_V2_TYPE_KIND_SET_OF:
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SEQUENCE_OF: {
		void *content = NULL;
		t_list_next next = {0};
		while (ft_list_next_content(&src->as.composite.elements, &next, &content)) {
			t_asn_v2_value *copy_value = asn1_v2_value_create();
			asn1_v2_value_copy(content, copy_value);
			ft_list_append_content(&dst->as.composite.elements, copy_value);
		}
		break;
	}
	case ASN_V2_TYPE_KIND_CHOICE: {
		if (NULL != src->as.choice.id) dst->as.choice.id = ft_strdup(src->as.choice.id);
		if (NULL != src->as.choice.value) {
			dst->as.choice.value = asn1_v2_value_create();
			asn1_v2_value_copy(src->as.choice.value, dst->as.choice.value);
		}
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		ft_ostr_copy(&src->as.any.octets, &dst->as.any.octets);
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 value type: %d", src->kind);
	}}
}

t_asn_v2_constraint *asn1_v2_constraint_create(void)
{
	t_asn_v2_constraint *constraint = NULL;
	SSL_ALLOC(constraint, sizeof(t_asn_v2_constraint));
	*constraint = (t_asn_v2_constraint){0};
	return (constraint);
}

void	asn1_v2_constraint_clear(t_asn_v2_constraint *asn1_constraint)
{
	switch (asn1_constraint->kind) {
	case ASN_V2_CONSTRAINT_KIND_RANGE:
		bnum_del(asn1_constraint->as.range.min);
		bnum_del(asn1_constraint->as.range.max);
		asn1_constraint->as.range.min = NULL;
		asn1_constraint->as.range.max = NULL;
		break;
	case ASN_V2_CONSTRAINT_KIND_SIZE:
		asn1_constraint->as.size.min = 0;
		asn1_constraint->as.size.max = SIZE_MAX;
		break;
	default:
		SSL_LOG(ERROR, "unknown asn1 constrain type: %d", asn1_constraint->kind);
	}
}

void	asn1_v2_constraint_delete(t_asn_v2_constraint *asn1_constraint)
{
	asn1_v2_constraint_clear(asn1_constraint);
	SSL_FREE(asn1_constraint);
}

void	asn1_v2_constraint_copy(const t_asn_v2_constraint *src, t_asn_v2_constraint *dst)
{
	dst->kind = src->kind;
	switch (src->kind) {
	case ASN_V2_CONSTRAINT_KIND_RANGE:
		bnum_copy(src->as.range.min, dst->as.range.min);
		bnum_copy(src->as.range.max, dst->as.range.max);
		break;
	case ASN_V2_CONSTRAINT_KIND_SIZE:
		dst->as.size.min = src->as.size.min;
		dst->as.size.max = src->as.size.max;
		break;
	default:
		SSL_LOG(ERROR, "unknown asn1 constrain type: %d", src->kind);
	}
}

t_asn_v2_type *asn1_v2_type_create(void)
{
	t_asn_v2_type *asn1_type = NULL;
	SSL_ALLOC(asn1_type, sizeof(t_asn_v2_type));
	*asn1_type = (t_asn_v2_type){0};
	return (asn1_type);
}

void asn1_v2_type_clear(t_asn_v2_type *asn1_type)
{
	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET: {
		ft_list_clear_all_content(&asn1_type->as.composite.elements, __asn1_v2_component_delete_adapter);
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF: {
		if (NULL != asn1_type->as.collection.element_type) asn1_v2_type_delete(asn1_type->as.collection.element_type);
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		ft_list_clear_all_content(&asn1_type->as.enumeration.items, __asn1_v2_enum_delete_adapter);
		break;
	}
	case ASN_V2_TYPE_KIND_INTEGER:
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NUMERIC_STRING:
	case ASN_V2_TYPE_KIND_VISIBLE_STRING:
	case ASN_V2_TYPE_KIND_UTC_TIME:
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR: {
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 type: %d", asn1_type->kind);
	}}
	ft_list_clear_all_content(&asn1_type->tags, __asn1_v2_tag_delete_adapter);
	ft_list_clear_all_content(&asn1_type->constraints, __asn1_v2_constraint_delete_adapter);
}

void asn1_v2_type_delete(t_asn_v2_type *asn1_type)
{
	asn1_v2_type_clear(asn1_type);
	SSL_FREE(asn1_type);
}

void	asn1_v2_type_copy(const t_asn_v2_type *src, t_asn_v2_type *dst)
{
	*dst = (t_asn_v2_type){0};
	dst->kind = src->kind;

	void *content = NULL;
	t_list_next next = {0};
	while (ft_list_next_content(&src->constraints, &next, &content)) {
		t_asn_v2_constraint *dst_constraint = asn1_v2_constraint_create();
		asn1_v2_constraint_copy(content, dst_constraint);
		ft_list_append_content(&dst->constraints, dst_constraint);
	}

	switch (src->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET: {
		void *content = NULL;
		t_list_next next = {0};
		while (ft_list_next_content(&src->as.composite.elements, &next, &content)) {
			t_asn_v2_component *dst_component = asn1_v2_component_create();
			asn1_v2_component_copy(content, dst_component);
			ft_list_append_content(&dst->as.composite.elements, dst_component);
		}
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF: {
		dst->as.collection.element_type = asn1_v2_type_create();
		asn1_v2_type_copy(src->as.collection.element_type, dst->as.collection.element_type);
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		ft_list_copy_all_content(&src->as.enumeration.items, &dst->as.enumeration.items, __asn1_v2_enum_copy_adapter);
		dst->as.enumeration.extendable = src->as.enumeration.extendable;
		break;
	}
	case ASN_V2_TYPE_KIND_INTEGER:
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NUMERIC_STRING:
	case ASN_V2_TYPE_KIND_VISIBLE_STRING:
	case ASN_V2_TYPE_KIND_UTC_TIME:
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR: {
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 type: %d", src->kind);
	}}
}

bool	asn1_v2_type_eq(const t_asn_v2_type *a, const t_asn_v2_type *b)
{
	assert(NULL != a && NULL != b);

	if (a->kind != b->kind) return false;

	switch (a->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET: {
		void *a_content = NULL;
		void *b_content = NULL;
		t_list_next a_next = {0};
		t_list_next b_next = {0};
		while (true) {
			bool a_has_next = ft_list_next_content(&a->as.composite.elements, &a_next, &a_content);
			bool b_has_next = ft_list_next_content(&b->as.composite.elements, &b_next, &b_content);
			if (a_has_next != b_has_next) return false;
			if (!a_has_next || !b_has_next) break;
			t_asn_v2_component *a_component = a_content;
			t_asn_v2_component *b_component = b_content;
			if (!asn1_v2_type_eq(a_component->type, b_component->type)) return false;
		}
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF: {
		if (!asn1_v2_type_eq(a->as.collection.element_type, b->as.collection.element_type)) return false;
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		break;
	}
	case ASN_V2_TYPE_KIND_INTEGER:
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NUMERIC_STRING:
	case ASN_V2_TYPE_KIND_VISIBLE_STRING:
	case ASN_V2_TYPE_KIND_UTC_TIME:
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR: {
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 type: %d", a->kind);
		return false;
	}}
	return true;
}

t_asn_v2_component *asn1_v2_component_create(void)
{
	t_asn_v2_component *asn1_component = NULL;
	SSL_ALLOC(asn1_component, sizeof(t_asn_v2_component));
	*asn1_component = (t_asn_v2_component){0};
	return (asn1_component);
}

void asn1_v2_component_clear(t_asn_v2_component *asn1_component)
{
	if (NULL != asn1_component->id) SSL_FREE(asn1_component->id);
	if (NULL != asn1_component->type) asn1_v2_type_delete(asn1_component->type);
	if (NULL != asn1_component->default_value) asn1_v2_value_delete(asn1_component->default_value);
}

void asn1_v2_component_delete(t_asn_v2_component *asn1_component)
{
	asn1_v2_component_clear(asn1_component);
	SSL_FREE(asn1_component);
}

void	asn1_v2_component_copy(const t_asn_v2_component *src, t_asn_v2_component *dst)
{
	dst->id = ft_strdup(src->id);
	dst->type = asn1_v2_type_create();
	asn1_v2_type_copy(src->type, dst->type);
	if (NULL != src->default_value) {
		dst->default_value = asn1_v2_value_create();
		asn1_v2_value_copy(src->default_value, dst->default_value);
	}
	dst->optional = src->optional;
}

t_asn_v2_component *asn1_v2_component_find_by_id(t_list components, const char *name)
{
	assert(NULL != name);

	void *content = NULL;
	t_list_next next = {0};
	while (ft_list_next_content(&components, &next, &content)) {
		t_asn_v2_component *component = content;
		if (ft_streq(component->id, name)) {
			return (component);
		}
	}
	return (NULL);
}

t_asn_v2_module *asn1_v2_module_create(void)
{
	t_asn_v2_module *asn1_module = NULL;
	SSL_ALLOC(asn1_module, sizeof(t_asn_v2_module));
	*asn1_module = (t_asn_v2_module){0};
	ft_htbl_v2_init(&asn1_module->symbol_table.symbols, 512);
	return (asn1_module);
}

void asn1_v2_module_delete(t_asn_v2_module *asn1_module)
{
	asn1_v2_module_clear(asn1_module);
	SSL_FREE(asn1_module);
}

void asn1_v2_module_clear(t_asn_v2_module *asn1_module)
{
	ft_htbl_v2_clear(&asn1_module->symbol_table.symbols, __asn1_v2_symbol_delete_adapter);
}

void asn1_v2_module_copy(const t_asn_v2_module *src, t_asn_v2_module *dst)
{
	dst->tag_mode = src->tag_mode;
	{
		const char *key = NULL;
		void *value = NULL;
		t_htbl_v2_next next = {0};
		while (ft_htbl_v2_next(&src->symbol_table.symbols, &next, &key, &value)) {
			t_asn_v2_symbol *copy_symbol = __asn1_v2_symbol_copy_adapter(value);
			bool ok = ft_htbl_v2_set(&dst->symbol_table.symbols, key, copy_symbol);
			assert(ok);
		}
	}
}

t_asn_v2_type *asn1_v2_module_get_type(const t_asn_v2_module *asn1_module, const char *name)
{
	t_asn_v2_symbol *symbol = ft_htbl_v2_get(&asn1_module->symbol_table.symbols, name);
	if (NULL != symbol && symbol->kind == ASN_V2_SYMBOL_KIND_TYPE_REF) {
		return (symbol->resolved.type_ref.type);
	}
	return (NULL);
}

t_asn_v2_value *asn1_v2_module_get_value(const t_asn_v2_module *asn1_module, const char *name)
{
	t_asn_v2_symbol *symbol = ft_htbl_v2_get(&asn1_module->symbol_table.symbols, name);
	if (NULL != symbol && symbol->kind == ASN_V2_SYMBOL_KIND_VALUE_REF) {
		return (symbol->resolved.value_ref.value);
	}
	return (NULL);
}

/****************************************************************************/

char *asn1_v2_tag_dumps(const t_asn_v2_tag *asn1_tag)
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

char *asn1_v2_constraint_dumps(const t_asn_v2_constraint *asn1_constraint)
{
	if (NULL == asn1_constraint) return ft_strdup("null");

	t_ostring ostring = {0};

	switch (asn1_constraint->kind) {
	case ASN_V2_CONSTRAINT_KIND_RANGE: {
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
	}
	case ASN_V2_CONSTRAINT_KIND_SIZE: {
		ft_ostr_appendf(&ostring, "{\"type\":\"size\",\"min\":%zu,\"min\":%zu}", asn1_constraint->as.size.min, asn1_constraint->as.size.max);
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 constraint type: %d", asn1_constraint->kind);
	}}
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

char *asn1_v2_value_dumps(const t_asn_v2_value *asn1_value)
{
	if (NULL == asn1_value) return ft_strdup("null");

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);

	const char *type_name = asn1_v2_get_type_name(asn1_value->kind);
	ft_ostr_appendf(&ostring, "{\"%s\":", type_name);

	switch (asn1_value->kind) {
	case ASN_V2_TYPE_KIND_NULL: {
		ft_ostr_append_cstr(&ostring, "null");
		break;
	}
	case ASN_V2_TYPE_KIND_BOOLEAN: {
		if (asn1_value->as.boolean) {
			ft_ostr_append_cstr(&ostring, "true");
		} else {
			ft_ostr_append_cstr(&ostring, "false");
		}
		break;
	}
	case ASN_V2_TYPE_KIND_INTEGER: {
		char *dumps = bnum_to_dec(&asn1_value->as.number);
		ft_ostr_append_cstr(&ostring, dumps);
		SSL_FREE(dumps);
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		char *dumps = bnum_to_dec(&asn1_value->as.enumerated.number);
		ft_ostr_appendf(&ostring, "[\"%s\",%s]", asn1_value->as.enumerated.name, dumps);
		SSL_FREE(dumps);
		break;
	}
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NUMERIC_STRING:
	case ASN_V2_TYPE_KIND_VISIBLE_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING: {
		t_ostring tmp = {0};
		ft_ostr_appendf(&tmp, "\"%s\"", asn1_value->as.cstring);
		char *escaped = json_v2_escape_string((const char *)tmp.content, tmp.size);
		ft_ostr_clear(&tmp);
		ft_ostr_appendf(&ostring, "\"%s\"", escaped);
		SSL_FREE(escaped);
		break;
	}
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
	case ASN_V2_TYPE_KIND_UTC_TIME: {
		ft_ostr_appendf(&ostring, "\"%s\"", asn1_value->as.cstring);
		break;
	}
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_ID: {
		ft_ostr_append_cstr(&ostring, "[");
		t_list_next next = {0};
		void *content = NULL;
		size_t commas = 0;
		while (ft_list_next_content(&asn1_value->as.object_id.arcs, &next, &content)) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			t_asn_v2_oid_arc *arc = content;
			switch (arc->kind) {
			case ASN_V2_OID_ARC_KIND_NAMED: {
				assert(NULL != arc->as.name);
				ft_ostr_appendf(&ostring, "\"%s\"", arc->as.name);
				break;
			}
			case ASN_V2_OID_ARC_KIND_NUMERIC: {
				ft_ostr_appendf(&ostring, "%zu", arc->as.number);
				break;
			}
			case ASN_V2_OID_ARC_KIND_NAMED_AND_NUMERIC: {
				assert(NULL != arc->as.name);
				ft_ostr_appendf(&ostring, "\"%s(%zu)\"", arc->as.name, arc->as.number);
				break;
			}}
		}
		ft_ostr_append_cstr(&ostring, "]");
		break;
	}
	case ASN_V2_TYPE_KIND_UTF8_STRING: {
		char *dumps = NULL;
		t_ostring tmp = {0};
		ft_ostr_init_with_capacity(&tmp, asn1_value->as.ostring.size+2);
		ft_ostr_append_cstr(&tmp, "\"");
		ft_ostr_append(&tmp, asn1_value->as.ostring.content, asn1_value->as.ostring.size);
		ft_ostr_append_cstr(&tmp, "\"");
		dumps = json_v2_escape_string((const char *)tmp.content, tmp.size);
		ft_ostr_clear(&tmp);
		ft_ostr_appendf(&ostring, "\"%s\"", dumps);
		SSL_FREE(dumps);
		break;
	}
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_BIT_STRING: {
		ft_ostr_append_cstr(&ostring, "\"'");
		char *hexs = ft_bytes_dumps_hex_upper(asn1_value->as.ostring.content, asn1_value->as.ostring.size);
		ft_ostr_append_cstr(&ostring, hexs);
		SSL_FREE(hexs);
		ft_ostr_append_cstr(&ostring, "'H\"");
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET:
	case ASN_V2_TYPE_KIND_SET_OF: {
		t_list_next next = {0};
		void *content;
		size_t commas = 0;
		ft_ostr_append_cstr(&ostring, "[");
		while (ft_list_next_content(&asn1_value->as.composite.elements, &next, &content)) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = asn1_v2_value_dumps(content);
			ft_ostr_append_cstr(&ostring, dumps);
			SSL_FREE(dumps);
		}
		ft_ostr_append_cstr(&ostring, "]");
		break;
	}
	case ASN_V2_TYPE_KIND_CHOICE: {
		char *dumps = asn1_v2_value_dumps(asn1_value->as.choice.value);
		if (NULL != asn1_value->as.choice.id) {
			ft_ostr_appendf(&ostring, "{\"%s\":%s}", asn1_value->as.choice.id, dumps);
		} else {
			ft_ostr_append_cstr(&ostring, dumps);
		}
		SSL_FREE(dumps);
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		char *dumps = ft_bytes_dumps_hex_upper(asn1_value->as.any.octets.content, asn1_value->as.any.octets.size);
		ft_ostr_appendf(&ostring, "\"'%s'H\"", dumps);
		SSL_FREE(dumps);
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown der value type: %d", asn1_value->kind);
		ft_ostr_append_cstr(&ostring, "\"???\"");
		break;
	}}
	ft_ostr_append_cstr(&ostring, "}");
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

char *asn1_v2_type_dumps(const t_asn_v2_type *asn1_type)
{
	if (NULL == asn1_type) return ft_strdup("null");

	SSL_LOG(TRACE, "dumping asn1 type: %s", asn1_v2_get_type_name(asn1_type->kind));

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);

	ft_ostr_appendf(&ostring, "{\"kind\":\"%s\"", asn1_v2_get_type_name(asn1_type->kind));

	if (asn1_type->tags.size > 0) {
		ft_ostr_append_cstr(&ostring, ",\"tags\":[");
		t_list_next next = {0};
		void *content = NULL;
		size_t commas = 0;
		while (ft_list_next_content(&asn1_type->tags, &next, &content)) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *constraint_dumps = asn1_v2_tag_dumps(content);
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
			char *constraint_dumps = asn1_v2_constraint_dumps(content);
			ft_ostr_append_cstr(&ostring, constraint_dumps);
			SSL_FREE(constraint_dumps);
		}
		ft_ostr_append_cstr(&ostring, "]");
	}

	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET: {
		t_list_next next = {0};
		void *content = NULL;
		size_t commas = 0;
		ft_ostr_append_cstr(&ostring, ",\"components\":[");
		while (ft_list_next_content(&asn1_type->as.composite.elements, &next, &content)) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = asn1_v2_component_dumps(content);
			ft_ostr_append_cstr(&ostring, dumps);
			SSL_FREE(dumps);
		}
		ft_ostr_append_cstr(&ostring, "]");
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF: {
		char *dumps = asn1_v2_type_dumps(asn1_type->as.collection.element_type);
		ft_ostr_appendf(&ostring, ",\"element_type\":%s", dumps);
		SSL_FREE(dumps);
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		t_list_next next = {0};
		void *content = NULL;
		size_t commas = 0;
		ft_ostr_append_cstr(&ostring, ",\"enumeration\":{\"items\":[");
		while (ft_list_next_content(&asn1_type->as.enumeration.items, &next, &content)) {
			t_asn_v2_enum *item = content;
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = bnum_to_dec(&item->number);
			ft_ostr_appendf(&ostring, "[\"%s\",%s]", item->name, dumps);
			SSL_FREE(dumps);
		}
		ft_ostr_appendf(&ostring, "],\"extendable\":%s}", asn1_type->as.enumeration.extendable ? "true" : "false");
		break;
	}
	case ASN_V2_TYPE_KIND_INTEGER:
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NUMERIC_STRING:
	case ASN_V2_TYPE_KIND_VISIBLE_STRING:
	case ASN_V2_TYPE_KIND_UTC_TIME:
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR: {
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 type: %d", asn1_type->kind);
	}}

	ft_ostr_append_cstr(&ostring, "}");
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

char *asn1_v2_component_dumps(const t_asn_v2_component *asn1_component)
{
	if (NULL == asn1_component) return ft_strdup("null");

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);

	if (NULL != asn1_component->id) {
		ft_ostr_appendf(&ostring, "{\"id\":\"%s\"", asn1_component->id);
	} else {
		ft_ostr_append_cstr(&ostring, "{\"id\":null");
	}
	char *dumps = asn1_v2_type_dumps(asn1_component->type);
	ft_ostr_appendf(&ostring, ",\"type\":%s", dumps);
	SSL_FREE(dumps);

	if (NULL != asn1_component->defined_by) {
		ft_ostr_appendf(&ostring, ",\"defined_by\":\"%s\"", asn1_component->defined_by);
	}

	if (NULL != asn1_component->default_value) {
		dumps = asn1_v2_value_dumps(asn1_component->default_value);
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
	while (ft_htbl_v2_next(&asn1_module->symbol_table.symbols, &next, &key, &value)) {
		// SSL_LOG(TRACE, "dumping asn1 type: %s", key);
		t_asn_v2_symbol *symbol = value;
		if (symbol->kind == ASN_V2_SYMBOL_KIND_TYPE_REF) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = asn1_v2_type_dumps(symbol->resolved.type_ref.type);
			ft_ostr_appendf(&ostring, "\"%s\":%s", key, dumps);
			SSL_FREE(dumps);
		}
	}
	ft_ostr_append_cstr(&ostring, "},\"values\":{");
	next = (t_htbl_v2_next){0};
	commas = 0;
	while (ft_htbl_v2_next(&asn1_module->symbol_table.symbols, &next, &key, &value)) {
		// SSL_LOG(TRACE, "dumping asn1 value: %s", key);
		t_asn_v2_symbol *symbol = value;
		if (symbol->kind == ASN_V2_SYMBOL_KIND_VALUE_REF) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = asn1_v2_value_dumps(symbol->resolved.value_ref.value);
			ft_ostr_appendf(&ostring, "\"%s\":%s", key, dumps);
			SSL_FREE(dumps);
		}
	}
	ft_ostr_append_cstr(&ostring, "}}");
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);

	return (dumps);
}

char *asn1_v2_module_pretty_dumps(const t_asn_v2_module *asn1_module, bool colored)
{
	char *dumps = asn1_v2_module_dumps(asn1_module);
	const t_json_v2 *json = NULL;
	int status = json_v2_parse(dumps, &json);
	if (JSON_V2_OK == status) {
		SSL_FREE(dumps);
		return (json_v2_pretty_dumps_special(json, colored));
	} else {
		SSL_LOG(ERROR, "failed to pretty format dump string");
		return (dumps);
	}
}

char *asn1_v2_tag_dumpb(const t_asn_v2_tag *asn1_tag, char *buf, size_t size)
{
	assert(NULL != buf);
	if (size == 0) return (NULL);

	char *dumps = asn1_v2_tag_dumps(asn1_tag);
	size_t len = ft_strlen(dumps);
	if (len >= size) len = size-1;
	ft_strncpy(buf, dumps, len);
	buf[len] = '\0';
	SSL_FREE(dumps);
	return (buf);
}

char *asn1_v2_value_dumpb(const t_asn_v2_value *asn1_value, char *buf, size_t size)
{
	assert(NULL != buf);
	if (size == 0) return (NULL);

	char *dumps = asn1_v2_value_dumps(asn1_value);
	size_t len = ft_strlen(dumps);
	if (len >= size) len = size-1;
	ft_strncpy(buf, dumps, len);
	buf[len] = '\0';
	SSL_FREE(dumps);
	return (buf);
}

char *asn1_v2_type_dumpb(const t_asn_v2_type *asn1_type, char *buf, size_t size)
{
	assert(NULL != buf);
	if (size == 0) return (NULL);

	char *dumps = asn1_v2_type_dumps(asn1_type);
	size_t len = ft_strlen(dumps);
	if (len >= size) len = size-1;
	ft_strncpy(buf, dumps, len);
	buf[len] = '\0';
	SSL_FREE(dumps);
	return (buf);
}

char *asn1_v2_component_dumpb(const t_asn_v2_component *asn1_component, char *buf, size_t size)
{
	assert(NULL != buf);
	if (size == 0) return (NULL);

	char *dumps = asn1_v2_component_dumps(asn1_component);
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

t_der_v2_tag *der_v2_tag_create(void)
{
	t_der_v2_tag *der_tag = NULL;
	SSL_ALLOC(der_tag, sizeof(t_der_v2_tag));
	*der_tag = (t_der_v2_tag){0};
	return (der_tag);
}

inline void der_v2_tag_delete(t_der_v2_tag *der_tag)
{
	SSL_FREE(der_tag);
}

void der_v2_tag_copy(const t_der_v2_tag *src, t_der_v2_tag *dst)
{
	*dst = *src;
}

bool der_v2_tag_eq(const t_der_v2_tag *a, const t_der_v2_tag *b)
{
	// return (a->class == b->class && a->number == b->number && a->constructed == b->constructed);
	return (ft_memcmp(a, b, sizeof(t_der_v2_tag)) == 0);
}

t_der_v2_tag der_v2_get_tag_universal(t_asn_v2_type_kind type)
{
	t_asn_v2_tag_class uclass = ASN_V2_TAG_CLASS_UNIVERSAL;
	switch (type) {
	case ASN_V2_TYPE_KIND_INTEGER:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_INTEGER, };
	case ASN_V2_TYPE_KIND_ENUMERATED:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_ENUMERATED, };
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
	case ASN_V2_TYPE_KIND_NUMERIC_STRING:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_NUMERIC_STRING, };
	case ASN_V2_TYPE_KIND_VISIBLE_STRING:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_VISIBLE_STRING, };
	case ASN_V2_TYPE_KIND_UTC_TIME:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_UTC_TIME, };
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_GENERALIZED_TIME, };
	case ASN_V2_TYPE_KIND_NULL:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_NULL, };
	case ASN_V2_TYPE_KIND_OBJECT_ID:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_OBJECT_ID, };
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
		return (t_der_v2_tag){ .class = uclass, .constructed = false, .number = ASN_V2_TAG_NUMBER_RELATIVE_OBJECT_ID, };
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
	default:
		SSL_LOG(ERROR, "unknown asn1 type: %d", type);
		return (t_der_v2_tag){0};
	}
}

t_der_v2_value *der_v2_value_create(void)
{
	t_der_v2_value *der_value = NULL;
	SSL_ALLOC(der_value, sizeof(t_der_v2_value));
	*der_value = (t_der_v2_value){0};
	return (der_value);
}

void	der_v2_value_copy(t_der_v2_value *src, t_der_v2_value *dst)
{
	switch (src->kind) {
	case ASN_V2_TYPE_KIND_NULL: {
		break;
	}
	case ASN_V2_TYPE_KIND_BOOLEAN: {
		dst->as.boolean = src->as.boolean;
		break;
	}
	case ASN_V2_TYPE_KIND_INTEGER: {
		bnum_copy(&src->as.number, &dst->as.number);
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		asn1_v2_enum_copy(&src->as.enumerated, &dst->as.enumerated);
		break;
	}
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NUMERIC_STRING:
	case ASN_V2_TYPE_KIND_VISIBLE_STRING:
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
	case ASN_V2_TYPE_KIND_UTC_TIME:
	case ASN_V2_TYPE_KIND_IA5_STRING: {
		if (NULL != src->as.cstring) dst->as.cstring = ft_strdup(src->as.cstring);
		break;
	}
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_ID: {
		for (size_t i = 0; i < src->as.object_id.narcs; i++) {
			dst->as.object_id.arc_nums[i] = src->as.object_id.arc_nums[i];
			dst->as.object_id.narcs++;
		}
		break;
	}
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_BIT_STRING: {
		ft_ostr_copy(&src->as.ostring, &dst->as.ostring);
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET:
	case ASN_V2_TYPE_KIND_SET_OF: {
		if (src->as.composite.count > 0) {
			SSL_ALLOC(dst->as.composite.items, src->as.composite.count * sizeof(t_der_v2_value));
			dst->as.composite.count = src->as.composite.count;
			for (size_t idx = 0; idx < src->as.composite.count; idx++) {
				der_v2_value_copy(src->as.composite.items + idx, dst->as.composite.items + idx);
			}
		}
		break;
	}
	case ASN_V2_TYPE_KIND_CHOICE: {
		if (NULL != src->as.choice.id) dst->as.choice.id = ft_strdup(src->as.choice.id);
		dst->as.choice.value = der_v2_value_create();
		der_v2_value_copy(src->as.choice.value, dst->as.choice.value);
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		ft_ostr_copy(&src->as.any.octets, &dst->as.any.octets);
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown der value type: %d", src->kind);
	}}
	dst->kind = src->kind;
	if (src->explicit_tags.count > 0) {
		SSL_ALLOC(dst->explicit_tags.items, src->explicit_tags.count * sizeof(t_der_v2_tag));
		dst->explicit_tags.count = src->explicit_tags.count;
		for (size_t i = 0; i < src->explicit_tags.count; i++) {
			der_v2_tag_copy(src->explicit_tags.items + i, dst->explicit_tags.items + i);
		}
	}
	if (src->implicit_tags.count > 0) {
		SSL_ALLOC(dst->implicit_tags.items, src->implicit_tags.count * sizeof(t_der_v2_tag));
		dst->implicit_tags.count = src->implicit_tags.count;
		for (size_t i = 0; i < src->implicit_tags.count; i++) {
			der_v2_tag_copy(src->implicit_tags.items + i, dst->implicit_tags.items + i);
		}
	}
}

void der_v2_value_clear(t_der_v2_value *der_value)
{
	switch (der_value->kind) {
	case ASN_V2_TYPE_KIND_NULL: {
		return;
	}
	case ASN_V2_TYPE_KIND_BOOLEAN: {
		return;
	}
	case ASN_V2_TYPE_KIND_INTEGER: {
		bnum_clear(&der_value->as.number);
		return;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		asn1_v2_enum_clear(&der_value->as.enumerated);
		return;
	}
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NUMERIC_STRING:
	case ASN_V2_TYPE_KIND_VISIBLE_STRING:
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
	case ASN_V2_TYPE_KIND_UTC_TIME:
	case ASN_V2_TYPE_KIND_IA5_STRING: {
		if (NULL != der_value->as.cstring) SSL_FREE(der_value->as.cstring);
		der_value->as.cstring = NULL;
		return;
	}
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_ID: {
		der_value->as.object_id.narcs = 0;
		return;
	}
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_BIT_STRING: {
		ft_ostr_clear(&der_value->as.ostring);
		return;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET:
	case ASN_V2_TYPE_KIND_SET_OF: {
		for (size_t idx = 0; idx < der_value->as.composite.count; idx++) {
			der_v2_value_clear(der_value->as.composite.items + idx);
		}
		der_value->as.composite.count = 0;
		SSL_FREE(der_value->as.composite.items);
		der_value->as.composite.items = NULL;
		return;
	}
	case ASN_V2_TYPE_KIND_CHOICE: {
		if (NULL != der_value->as.choice.id) SSL_FREE(der_value->as.choice.id);
		der_value->as.choice.id = NULL;
		der_v2_value_delete(der_value->as.choice.value);
		der_value->as.choice.value = NULL;
		return;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		ft_ostr_clear(&der_value->as.any.octets);
		return;
	}
	default: {
		SSL_LOG(ERROR, "unknown der value type: %d", der_value->kind);
	}}
	SSL_FREE(der_value->explicit_tags.items);
	SSL_FREE(der_value->implicit_tags.items);
}

void der_v2_value_delete(t_der_v2_value *der_value)
{
	der_v2_value_clear(der_value);
	SSL_FREE(der_value);
}

t_der_v2_component *der_v2_component_create(void)
{
	t_der_v2_component *der_component = NULL;
	SSL_ALLOC(der_component, sizeof(t_der_v2_component));
	*der_component = (t_der_v2_component){0};
	return (der_component);
}

void der_v2_component_delete(t_der_v2_component *der_component)
{
	if (NULL == der_component) return;

	if (NULL != der_component->id) SSL_FREE(der_component->id);
	if (NULL != der_component->default_value) der_v2_value_delete(der_component->default_value);
	SSL_FREE(der_component);
}

t_der_v2_component *der_v2_component_find_by_id(t_list components, const char *name)
{
	assert(NULL != name);

	void *content = NULL;
	t_list_next next = {0};
	while (ft_list_next_content(&components, &next, &content)) {
		t_der_v2_component *component = content;
		if (ft_streq(component->id, name)) {
			return (component);
		}
	}
	return (NULL);
}

t_der_v2_type *der_v2_type_create(void)
{
	t_der_v2_type *der_type = NULL;
	SSL_ALLOC(der_type, sizeof(t_der_v2_type));
	*der_type = (t_der_v2_type){0};
	return (der_type);
}

void der_v2_type_delete(t_der_v2_type *der_type)
{
	if (NULL == der_type) return;

	ft_list_clear_all_content(&der_type->constraints, __asn1_v2_constraint_delete_adapter);

	switch (der_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SET:
	case ASN_V2_TYPE_KIND_CHOICE: {
		ft_list_clear_all_content(&der_type->as.composite.elements, __der_v2_type_delete_adapter);
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF: {
		der_v2_type_delete(der_type->as.collection.element_type);
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		ft_list_clear_all_content(&der_type->as.enumeration.items, __asn1_v2_enum_delete_adapter);
		break;
	}
	case ASN_V2_TYPE_KIND_ANY:
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INTEGER:
	case ASN_V2_TYPE_KIND_BOOLEAN:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING:
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NUMERIC_STRING:
	case ASN_V2_TYPE_KIND_VISIBLE_STRING:
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
	case ASN_V2_TYPE_KIND_UTC_TIME:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR: {
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown der value type: %d", der_type->kind);
	}}
	SSL_FREE(der_type);
}
