#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <json.h>
#include <json_v2.h>
#include <libft.h>
#include <bnum.h>
#include <textutil.h>

/****************************************************************************/

#define __SCHEMA_TAG_CLASS_NAME_UNIVERSAL   "UNIVERSAL"
#define __SCHEMA_TAG_CLASS_NAME_APPLICATION "APPLICATION"
#define __SCHEMA_TAG_CLASS_NAME_CONTEXT     "CONTEXT-SPECIFIC"
#define __SCHEMA_TAG_CLASS_NAME_PRIVATE     "PRIVATE"

#define __SCHEMA_TAG_MODE_NAME_EXPLICIT     "EXPLICIT"
#define __SCHEMA_TAG_MODE_NAME_IMPLICIT     "IMPLICIT"
#define __SCHEMA_TAG_MODE_NAME_AUTOMATIC    "AUTOMATIC"

#define __SCHEMA_CONSTRAINT_NAME_RANGE		"RANGE"
#define __SCHEMA_CONSTRAINT_NAME_SIZE		"SIZE"

#define __JQ_SCHEMA_TAG_MODE		"tagMode"
#define __JQ_SCHEMA_TYPES			"types"
#define __JQ_SCHEMA_VALUES			"values"

#define __JQ_VALUE_KIND				"kind"
#define __JQ_VALUE_VALUE			"value"

#define __JQ_TYPE_KIND				"kind"
#define __JQ_TYPE_TAGS	    		"tags"
#define __JQ_TYPE_CONSTRAINTS	    "constraints"
#define __JQ_TYPE_DEFINED_BY		"definedBy"
#define __JQ_TYPE_ENUMERATION		"enumeration"
#define __JQ_TYPE_EXT_ENUMERATION	"extEnumeration"
#define __JQ_TYPE_COMPONENT_TYPE	"componentType"
#define __JQ_TYPE_COMPONENTS		"components"

#define __JQ_COMPONENT_ID 		    "id"
#define __JQ_COMPONENT_TYPE 	    "type"
#define __JQ_COMPONENT_OPTIONAL	    "optional"
#define __JQ_COMPONENT_DEFAULT	    "default"

#define __JQ_TAG_MODE				"mode"
#define __JQ_TAG_CLASS				"class"
#define __JQ_TAG_NUMBER	 			"number"

#define __JQ_CONSTRAINT_TYPE		"type"
#define __JQ_CONSTRAINT_MIN			"min"
#define __JQ_CONSTRAINT_MAX			"max"

#define __JQ_TOKEN_ELLIPSIS			"..."

/****************************************************************************/

static inline void __asn1_v2_oid_arc_delete_adapter(void *vctx) { asn1_v2_oid_arc_delete((t_asn_v2_oid_arc *)vctx); }
static inline void __asn1_v2_symbol_delete_adapter(void *p)		{ asn1_v2_symbol_delete((t_asn_v2_symbol *)p); }
static inline void __asn1_v2_tag_delete_adapter(void *p)		{ asn1_v2_tag_delete((t_asn_v2_tag *)p); }
static inline void __asn1_v2_value_delete_adapter(void *p)		{ asn1_v2_value_delete((t_asn_v2_value *)p); }
static inline void __asn1_v2_constraint_delete_adapter(void *p)	{ asn1_v2_constraint_delete((t_asn_v2_constraint *)p); }
static inline void __asn1_v2_enum_delete_adapter(void *p)		{ asn1_v2_enum_delete((t_asn_v2_enum *)p); }
static inline void __asn1_v2_type_delete_adapter(void *p)	 	{ asn1_v2_type_delete((t_asn_v2_type *)p); }
static inline void __asn1_v2_component_delete_adapter(void *p)	{ asn1_v2_component_delete((t_asn_v2_component *)p); }
static inline void __asn1_v2_module_delete_adapter(void *p)		{ asn1_v2_module_delete((t_asn_v2_module *)p); }

static void *__asn1_v2_oid_arc_copy_adapter(const void *vctx)
{
	t_asn_v2_oid_arc *dst = asn1_v2_oid_arc_create();
	asn1_v2_oid_arc_copy((t_asn_v2_oid_arc *)vctx, dst);
	return (dst);
}
static inline void *__asn1_v2_symbol_copy_adapter(const void *p)
{
	t_asn_v2_symbol *dst = asn1_v2_symbol_create();
	asn1_v2_symbol_copy((t_asn_v2_symbol *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_tag_copy_adapter(const void *p)
{
	t_asn_v2_tag *dst = asn1_v2_tag_create();
	asn1_v2_tag_copy((t_asn_v2_tag *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_constraint_copy_adapter(const void *p)
{
	t_asn_v2_constraint *dst = asn1_v2_constraint_create();
	asn1_v2_constraint_copy((const t_asn_v2_constraint *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_enum_copy_adapter(const void *p)
{
	t_asn_v2_enum *dst = asn1_v2_enum_create();
	asn1_v2_enum_copy((const t_asn_v2_enum *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_value_copy_adapter(const void *p)
{
	t_asn_v2_value *dst = asn1_v2_value_create();
	asn1_v2_value_copy((const t_asn_v2_value *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_type_copy_adapter(const void *p)
{
	t_asn_v2_type *dst = asn1_v2_type_create();
	asn1_v2_type_copy((const t_asn_v2_type *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_component_copy_adapter(const void *p)
{
	t_asn_v2_component *dst = asn1_v2_component_create();
	asn1_v2_component_copy((const t_asn_v2_component *)p, dst);
	return (dst);
}
static inline void *__asn1_v2_module_copy_adapter(const void *p)
{
	t_asn_v2_module *dst = asn1_v2_module_create();
	asn1_v2_module_copy((const t_asn_v2_module *)p, dst);
	return (dst);
}

/****************************************************************************/

static int	__asn1_v2_schema_parse_component(t_asn_v2_symbol_table *symbol_table, t_asn_v2_component **asn1_component, const t_json_v2 *jcomponent);
static int	__asn1_v2_schema_parse_type(t_asn_v2_symbol_table *symbol_table, t_asn_v2_type **asn1_type, const t_json_v2 *jtype);
static int	__asn1_v2_schema_parse_value(t_asn_v2_symbol_table *symbol_table, const t_asn_v2_type *asn1_type, t_asn_v2_value **asn1_value, const t_json_v2 *jvalue);

static bool __asn1_v2_schema_is_symbol_name(const char *s)
{
	if (NULL == s) return false;
	size_t slen = ft_strlen(s);
	if (slen == 0) return false;
	if (!ft_isalpha(s[0])) return false;

	bool is_prev_hyphen = false;
	for (size_t i = 0; i < slen; i++) {
		if (!ft_isalnum_or_hyphen(s[i])) return false;
		if (s[i] == '-' && is_prev_hyphen) return false;
		is_prev_hyphen = (s[i] == '-');
	}
	if (s[slen-1] == '-') return false;
	// TODO: reserved ASN1 keywords.
	return true;
}

static bool __asn1_v2_schema_is_type_ref_symbol(const t_asn_v2_symbol_table *symbol_table, const char *symbol_name)
{
	if (!__asn1_v2_schema_is_symbol_name(symbol_name)) return false;
	if (!ft_isalpha_upper(symbol_name[0])) return false;
	const t_asn_v2_symbol *symbol = ft_htbl_v2_get(&symbol_table->symbols, symbol_name);
	if (NULL == symbol) return false;
	return (symbol->kind == ASN_V2_SYMBOL_KIND_TYPE_REF);
}

static bool __asn1_v2_schema_is_value_ref_symbol(const t_asn_v2_symbol_table *symbol_table, const char *symbol_name)
{
	if (!__asn1_v2_schema_is_symbol_name(symbol_name)) return false;
	if (!ft_isalpha_lower(symbol_name[0])) return false;
	const t_asn_v2_symbol *symbol = ft_htbl_v2_get(&symbol_table->symbols, symbol_name);
	if (NULL == symbol) return false;
	return (symbol->kind == ASN_V2_SYMBOL_KIND_VALUE_REF);
}

static int	__asn1_v2_schema_resolve_symbol(t_asn_v2_symbol_table *symbol_table, const char *symbol_name, const t_asn_v2_symbol **asn1_symbol)
{
	t_asn_v2_symbol *symbol = ft_htbl_v2_get(&symbol_table->symbols, symbol_name);
	if (NULL == symbol) {
		SSL_LOG(ERROR, "no such symbol in the table: %s", symbol_name);
		return (SSL_ERR);
	}

	switch (symbol->state) {
	case ASN_V2_SYMBOL_STATE_UNRESOLVED: {
		SSL_LOG(TRACE, "resolving symbol: %s", symbol_name);
		symbol->state = ASN_V2_SYMBOL_STATE_RESOLVING;
		switch (symbol->kind) {
		case ASN_V2_SYMBOL_KIND_TYPE_REF: {
			if (SSL_OK != __asn1_v2_schema_parse_type(symbol_table, &symbol->resolved.type_ref.type, symbol->source)) {
				SSL_LOG(ERROR, "resolve type ref error: %s", symbol_name);
				goto label_error;
			}
			break;
		}
		case ASN_V2_SYMBOL_KIND_VALUE_REF: {
			const t_json_v2 *jvalue_ref = symbol->source;
			const t_json_v2 *jvalue_kind = NULL;
			const t_json_v2 *jvalue = NULL;
			int ret = json_v2_query(__JQ_VALUE_KIND, jvalue_ref, &jvalue_kind);
			assert(ret == JSON_V2_OK && jvalue_kind->kind == JSON_V2_TYPE_STRING);
			ret = json_v2_query(__JQ_VALUE_VALUE, jvalue_ref, &jvalue);
			assert(ret == JSON_V2_OK);
			t_asn_v2_type_kind kind = asn1_v2_get_type_by_name(jvalue_kind->as.cstring);
			if (kind == -1) {
				const t_asn_v2_symbol *type_symbol = NULL;
				const char *type_name = jvalue_kind->as.cstring;
				if (SSL_OK != __asn1_v2_schema_resolve_symbol(symbol_table, type_name, &type_symbol)) {
					SSL_LOG(ERROR, "resolve value ref error: %s: unknown type: %s", symbol_name, type_name);
					goto label_error;
				}
				if (type_symbol->kind != ASN_V2_SYMBOL_KIND_TYPE_REF) {
					SSL_LOG(ERROR, "failed to resolve type for a value ref: %s: not a type ref: %s", symbol_name, type_name);
					goto label_error;
				}
				if (SSL_OK != __asn1_v2_schema_parse_value(symbol_table, type_symbol->resolved.type_ref.type, &symbol->resolved.value_ref.value, jvalue)) {
					SSL_LOG(ERROR, "resolve value error: %s", symbol_name);
					goto label_error;
				}
				symbol->resolved.value_ref.type = __asn1_v2_type_copy_adapter(type_symbol->resolved.value_ref.type);
			}
			else {
				if (SSL_OK != __asn1_v2_schema_parse_value(symbol_table, &(t_asn_v2_type){.kind = kind}, &symbol->resolved.value_ref.value, jvalue)) {
					SSL_LOG(ERROR, "resolve value error: %s", symbol_name);
					goto label_error;
				}
				symbol->resolved.value_ref.type = __asn1_v2_type_copy_adapter(&(const t_asn_v2_type){.kind = kind});
			}
			break;
		}}
		symbol->state = ASN_V2_SYMBOL_STATE_RESOLVED;
		break;
	}
	case ASN_V2_SYMBOL_STATE_RESOLVING: {
		SSL_LOG(ERROR, "resolve cycle: %s", symbol_name);
		goto label_error;
	}
	case ASN_V2_SYMBOL_STATE_RESOLVED: {
		SSL_LOG(TRACE, "symbol is already resolved: %s", symbol_name);
		switch (symbol->kind) {
		case ASN_V2_SYMBOL_KIND_TYPE_REF: {
			if (NULL == symbol->resolved.type_ref.type) {
				SSL_LOG(ERROR, "resolved type is not defined");
				goto label_error;
			}
			break;
		}
		case ASN_V2_SYMBOL_KIND_VALUE_REF: {
			if (NULL == symbol->resolved.value_ref.value) {
				SSL_LOG(ERROR, "resolved value is not defined");
				goto label_error;
			}
			break;
		}}
		break;
	}
	case ASN_V2_SYMBOL_STATE_ERROR: {
		SSL_LOG(ERROR, "resolve error: %s", symbol_name);
		goto label_error;
	}}

	if (NULL != asn1_symbol) *asn1_symbol = symbol;
	return (SSL_OK);

label_error:
	symbol->state = ASN_V2_SYMBOL_STATE_ERROR;
	if (NULL != asn1_symbol) *asn1_symbol = NULL;
	return (SSL_ERR);
}

static int	__asn1_v2_schema_parse_value(t_asn_v2_symbol_table *symbol_table, const t_asn_v2_type *asn1_type, t_asn_v2_value **asn1_value, const t_json_v2 *jvalue)
{
	char cbuf[1024] = {0};

	SSL_LOG(TRACE, "parsing asn1 value of type: %s", asn1_v2_get_type_name(asn1_type->kind));

	const char *expected_fmt = NULL;
	*asn1_value = NULL;

	// Check if this is a value reference.
	if (jvalue->kind == JSON_V2_TYPE_STRING) {
		const char *name = jvalue->as.cstring;
		if (__asn1_v2_schema_is_value_ref_symbol(symbol_table, name)) {
			const t_asn_v2_symbol *symbol = NULL;
			if (SSL_OK != __asn1_v2_schema_resolve_symbol(symbol_table, name, &symbol)) {
				SSL_LOG(ERROR, "failed to parse value ref: %s", name);
				return (SSL_ERR);
			}
			if (symbol->kind != ASN_V2_SYMBOL_KIND_VALUE_REF) {
				SSL_LOG(ERROR, "expected a reference to a value: %s", name);
				return (SSL_ERR);
			}
			const t_asn_v2_type *type = symbol->resolved.value_ref.type;
			const t_asn_v2_value *value = symbol->resolved.value_ref.value;
			// TODO: need proper type comparing, currently this only checks whether type kinds match between types.
			if (!asn1_v2_type_eq(type, asn1_type)) {
				SSL_LOG(ERROR, "failed to parse value ref: %s: incompatible type: %s", name, asn1_v2_type_dumpb(type, cbuf, sizeof(cbuf)));
				return (SSL_ERR);
			}
			*asn1_value = __asn1_v2_value_copy_adapter(value);
			return (SSL_OK);
		}
	}

	t_asn_v2_value *avalue = asn1_v2_value_create();
	avalue->kind = asn1_type->kind;

	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_NULL: {
		if (jvalue->kind != JSON_V2_TYPE_NULL) goto label_format;
		avalue->as.boolean = jvalue->as.boolean;
		break;
	}
	// TODO: implement a proper parsing for different kinds of strings.
	case ASN_V2_TYPE_KIND_IA5_STRING: {
		expected_fmt = "DOUBLE-QUOTED STRING";
		if (jvalue->kind != JSON_V2_TYPE_STRING) goto label_format;
		const char *s = jvalue->as.cstring;
		size_t slen = ft_strlen(s);
		if (slen < 2) goto label_format;
		if (s[0] != '"' || s[slen-1] != '"') goto label_format;
		for (size_t i = 1; i < slen-1; i++) {
			if (!ft_isascii(s[i])) goto label_format;
		}
		avalue->as.cstring = ft_strndup(s+1, slen-2);
		break;
	}
	case ASN_V2_TYPE_KIND_NUMERIC_STRING: {
		expected_fmt = "DOUBLE-QUOTED STRING";
		if (jvalue->kind != JSON_V2_TYPE_STRING) goto label_format;
		const char *s = jvalue->as.cstring;
		size_t slen = ft_strlen(s);
		if (slen < 2) goto label_format;
		if (s[0] != '"' || s[slen-1] != '"') goto label_format;
		for (size_t i = 1; i < slen-1; i++) {
			if (!ft_isdigit(s[i]) && s[i] != ' ') goto label_format;
		}
		avalue->as.cstring = ft_strndup(s+1, slen-2);
		break;
	}
	case ASN_V2_TYPE_KIND_VISIBLE_STRING:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING: {
		expected_fmt = "DOUBLE-QUOTED STRING";
		if (jvalue->kind != JSON_V2_TYPE_STRING) goto label_format;
		const char *s = jvalue->as.cstring;
		size_t slen = ft_strlen(s);
		if (slen < 2) goto label_format;
		if (s[0] != '"' || s[slen-1] != '"') goto label_format;
		for (size_t i = 1; i < slen-1; i++) {
			if (!ft_isprint(s[i])) goto label_format;
		}
		avalue->as.cstring = ft_strndup(s+1, slen-2);
		break;
	}
	case ASN_V2_TYPE_KIND_UTC_TIME: {
		// TODO: Handle leap years, etc.
		expected_fmt = "YYMMDDhhmmssZ";
		if (jvalue->kind != JSON_V2_TYPE_STRING) goto label_format;
		const char *s = jvalue->as.cstring;
		size_t slen = ft_strlen(s);
		int t[6] = {0};
		char z = 0;
		size_t scanned = 0;
		int matches = textutil_sscanf(s, slen, "%2d%2d%2d%2d%2d%2d%c%zn", t, t+1, t+2, t+3, t+4, t+5, &z, &scanned);
		if (matches != 7 || scanned != slen) goto label_format;
		if (t[1] < 1 || t[1] > 12) goto label_format;
		if (t[2] < 1 || t[2] > 31) goto label_format;
		if (t[3] < 0 || t[3] > 23) goto label_format;
		if (t[4] < 0 || t[4] > 59) goto label_format;
		if (t[5] < 0 || t[5] > 59) goto label_format;
		if (z != 'Z') goto label_format;
		avalue->as.cstring = ft_strdup(s);
		break;
	}
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME: {
		// TODO: Handle leap years, etc.
		expected_fmt = "YYYYMMDDhhmmss[.fraction]Z";
		if (jvalue->kind != JSON_V2_TYPE_STRING) goto label_format;
		const char *s = jvalue->as.cstring;
		size_t slen = ft_strlen(s);
		int t[6] = {0};
		char z = 0;
		bool has_fraction = false;
		int fraction = 0;
		size_t scanned = 0;
		int matches = textutil_sscanf(s, slen, "%4d%2d%2d%2d%2d%2d%zn", t, t+1, t+2, t+3, t+4, t+5, &scanned);
		if (matches != 6 || scanned != 14) goto label_format;
		if (t[1] < 1 || t[1] > 12) goto label_format;
		if (t[2] < 1 || t[2] > 31) goto label_format;
		if (t[3] < 0 || t[3] > 23) goto label_format;
		if (t[4] < 0 || t[4] > 59) goto label_format;
		if (t[5] < 0 || t[5] > 59) goto label_format;

		if (scanned < slen && s[scanned] == '.') {
			has_fraction = true;
			size_t scanned_suffix = 0;
			matches = textutil_sscanf(s+scanned, slen-scanned, ".%d%c%zn", &fraction, &z, &scanned_suffix);
			if (matches != 2 || scanned + scanned_suffix != slen) goto label_format;
		}
		else {
			if (scanned+1 != slen) goto label_format;
			z = s[scanned];
		}

		// Zero fraction must be omitted.
		if (has_fraction && fraction == 0) goto label_format;
		if (z != 'Z') goto label_format;
		avalue->as.cstring = ft_strdup(s);
		break;
	}
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_ID: {
		expected_fmt = "[ ARC_1, ARC_2, ..., ARC_N ] where ARC is STRING or NUMBER";
		if (jvalue->kind != JSON_V2_TYPE_ARRAY) goto label_format;
		if (jvalue->as.list.size < 1) goto label_format;
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&jvalue->as.list, &next, &content)) {
			t_json_v2 *jarc = content;
			t_asn_v2_oid_arc *arc = NULL;
			if (jarc->kind == JSON_V2_TYPE_STRING) {
				if (NULL == jarc->as.cstring) goto label_error;
				const char *s = jarc->as.cstring;
				size_t slen = ft_strlen(s);
				size_t i = 0;
				for (; i < slen && s[i] != '('; i++) {
					if (!ft_isalpha(s[i])) goto label_format;
				}
				if (s[i] == '(') {
					size_t name_end = i;
					size_t num_start = ++i;
					for (; i < slen && s[i] != ')'; i++) {
						if (!ft_isdigit(s[i])) goto label_format;
					}
					if (i == num_start || s[i] != ')') goto label_format;
					arc = asn1_v2_oid_arc_create();
					arc->kind = ASN_V2_OID_ARC_KIND_NAMED_AND_NUMERIC;
					arc->as.name = ft_strsub(jarc->as.cstring, 0, name_end);
					arc->as.number = ft_atoi_zu(s + num_start);
					ft_list_append_content(&avalue->as.object_id.arcs, arc);
				}
				else if (s[i] == '\0') {
					// Check if this is a value reference.
					if (__asn1_v2_schema_is_value_ref_symbol(symbol_table, s)) {
						const t_asn_v2_symbol *symbol = NULL;
						if (SSL_OK != __asn1_v2_schema_resolve_symbol(symbol_table, s, &symbol)) goto label_error;
						if (symbol->kind != ASN_V2_SYMBOL_KIND_VALUE_REF) {
							SSL_LOG(ERROR, "expected a reference to a type: %s", s);
							goto label_error;
						}
						const t_asn_v2_type *type = symbol->resolved.value_ref.type;
						const t_asn_v2_value *value = symbol->resolved.value_ref.value;
						if (type->kind != ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID) {
							SSL_LOG(ERROR, "invalid named arc: %s: expected %s but got %s", s, asn1_v2_get_type_name(ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID), asn1_v2_get_type_name(type->kind));
							goto label_error;
						}
						ft_list_copy_all_content(&value->as.object_id.arcs, &avalue->as.object_id.arcs, __asn1_v2_oid_arc_copy_adapter);
					}
				}
				else {
					goto label_format;
				}
			}
			else if (jarc->kind == JSON_V2_TYPE_NUMBER) {
				arc = asn1_v2_oid_arc_create();
				arc->kind = ASN_V2_OID_ARC_KIND_NUMERIC;
				arc->as.number = bnum_to_dig_u(&jarc->as.number);
				ft_list_append_content(&avalue->as.object_id.arcs, arc);
			}
			else {
				goto label_format;
			}
		}
		break;
	}
	case ASN_V2_TYPE_KIND_OBJECT_DESCR: {
		NOT_IMPLEMENTED("ASN_V2_TYPE_KIND_OBJECT_DESCR");
		break;
	}
	case ASN_V2_TYPE_KIND_BIT_STRING: {
		expected_fmt = "STRING as hex or binary literal such as 'AA2ABBCE0'H or '11101010'B";
		if (jvalue->kind != JSON_V2_TYPE_STRING) goto label_format;
		// Expect strings such as '1110101010'B or 'D10AEB11C'H
		const char *lit = jvalue->as.cstring;
		size_t lsize = ft_strlen(lit);
		if (lsize < 3) goto label_format;
		if (lit[0] != '\'' || lit[lsize-2] != '\'') goto label_format;
		if (lit[lsize-1] == 'B') {
			// Literal represents binary as a string with '1' and '0' characters.
			const char *bins = lit+1;
			size_t nbits = lsize-3;
			for (size_t i = 0; i < nbits; i++) {
				if (bins[i] != '0' && bins[i] != '1') goto label_format;
			}
			size_t trail_bits = nbits % 8;
			size_t nbytes = NBITS_TO_NBYTES(nbits);
			uint8_t *octets = NULL;
			SSL_ALLOC(octets, nbytes+1); // Extra byte for storing number of padded bits.
			octets[0] = 8 - trail_bits; // First content byte is for number of padded bits;
			ft_bins_to_bytes_left_align(octets+1, nbytes, bins, nbits);
			ft_ostr_append(&avalue->as.ostring, octets, nbytes+1);
			SSL_FREE(octets);
		}
		else if (lit[lsize-1] == 'H') {
			// Literal represents binary as a hex string.
			size_t lsize = ft_strlen(lit);
			const char *hexs = lit+1;
			size_t nhexs = lsize-3;
			for (size_t i = 0; i < nhexs; i++) {
				if (!ft_ishex(hexs[i])) goto label_format;
			}
			size_t trail_bits = 4 * (nhexs % 2);
			size_t nbytes = CEIL_DIV(nhexs, 2);
			uint8_t *octets = NULL;
			SSL_ALLOC(octets, nbytes+1); // Extra byte for storing number of padded bits.
			octets[0] = 8 - trail_bits; // First content byte is for number of padded bits;
			ft_hexs_to_bytes_left_align(octets+1, nbytes, hexs, nhexs);
			ft_ostr_append(&avalue->as.ostring, octets, nbytes+1);
			SSL_FREE(octets);
		} else {
			goto label_format;
		}
		break;
	}
	case ASN_V2_TYPE_KIND_OCTET_STRING: {
		expected_fmt = "STRING as hex or binary literal such as 'AA2ABBCE0'H or '11101010'B";
		if (jvalue->kind != JSON_V2_TYPE_STRING) goto label_format;
		// Expect strings such as '1110101010'B or 'D10AEB11C'H
		const char *lit = jvalue->as.cstring;
		size_t lsize = ft_strlen(lit);
		if (lsize < 3) goto label_format;
		if (lit[0] != '\'' || lit[lsize-2] != '\'') goto label_format;
		if (lit[lsize-1] == 'B') {
			// Literal represents binary as a string with '1' and '0' characters.
			const char *bins = lit+1;
			size_t nbits = lsize-3;
			for (size_t i = 0; i < nbits; i++) {
				if (bins[i] != '0' && bins[i] != '1') goto label_format;
			}
			size_t nbytes = NBITS_TO_NBYTES(nbits);
			uint8_t *octets = NULL;
			SSL_ALLOC(octets, nbytes);
			ft_bins_to_bytes_left_align(octets, nbytes, bins, nbits);
			ft_ostr_append(&avalue->as.ostring, octets, nbytes);
			SSL_FREE(octets);
		}
		else if (lit[lsize-1] == 'H') {
			// Literal represents binary as a hex string.
			const char *hexs = lit+1;
			size_t nhexs = lsize-3;
			for (size_t i = 0; i < nhexs; i++) {
				if (!ft_ishex(hexs[i])) goto label_format;
			}
			size_t nbytes = CEIL_DIV(nhexs, 2);
			uint8_t *octets = NULL;
			SSL_ALLOC(octets, nbytes);
			ft_hexs_to_bytes_left_align(octets, nbytes, hexs, nhexs);
			ft_ostr_append(&avalue->as.ostring, octets, nbytes);
			SSL_FREE(octets);
		} else {
			goto label_format;
		}
		break;
	}
	case ASN_V2_TYPE_KIND_INTEGER: {
		expected_fmt = "NUMBER";
		if (jvalue->kind != JSON_V2_TYPE_NUMBER) goto label_format;
		bnum_copy(&jvalue->as.number, &avalue->as.number);
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		expected_fmt = "STRING";
		if (jvalue->kind != JSON_V2_TYPE_STRING) goto label_format;
		t_list_next next = {0};
		void *content = NULL;
		const t_num *number = NULL;
		while (ft_list_next_content(&asn1_type->as.enumeration.items, &next, &content)) {
			t_asn_v2_enum *item = content;
			if (ft_streq(jvalue->as.cstring, item->name)) {
				number = &item->number;
				break;
			}
		}
		if (NULL == number) goto label_error;
		avalue->as.enumerated.name = ft_strdup(jvalue->as.cstring);
		bnum_copy(number, &avalue->as.enumerated.number);
		break;
	}
	case ASN_V2_TYPE_KIND_BOOLEAN: {
		expected_fmt = "BOOLEAN literal: true or false";
		if (jvalue->kind != JSON_V2_TYPE_BOOL) goto label_format;
		avalue->as.boolean = jvalue->as.boolean;
		break;
	}
	case ASN_V2_TYPE_KIND_UTF8_STRING: {
		expected_fmt = "DOUBLE-QUOTED STRING";
		if (jvalue->kind != JSON_V2_TYPE_STRING) goto label_format;
		const char *s = jvalue->as.cstring;
		size_t slen = ft_strlen(s);
		if (slen < 2) goto label_format;
		if (s[0] != '"' || s[slen-1] != '"') goto label_format;
		if (!utf8_is_valid((uint8_t *)(s+1), slen-2)) goto label_format;
		ft_ostr_init_from(&avalue->as.ostring, s+1, slen-2);
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE: {
		expected_fmt = "[[ID_1, VALUE_1], [ID_2, VALUE_2], ..., [ID_N, VALUE_N]]";
		if (jvalue->kind != JSON_V2_TYPE_ARRAY) goto label_format;
		t_list_next jnext = {0};
		t_list_next anext = {0};
		void *content = NULL;

		if (jvalue->as.list.size > asn1_type->as.composite.elements.size) {
			goto label_error;
		}
		while (ft_list_next_content(&jvalue->as.list, &jnext, &content)) {
			const t_json_v2 *jelement = content;
			const t_json_v2 *jelement_id = NULL;
			const t_json_v2 *jelement_value = NULL;
			if (jelement->kind != JSON_V2_TYPE_ARRAY) goto label_format;
			if (JSON_V2_OK != json_v2_query_nonnull("[0]", jelement, &jelement_id)) goto label_format;
			if (jelement_id->kind != JSON_V2_TYPE_STRING) goto label_format;
			if (JSON_V2_OK != json_v2_query("[1]", jelement, &jelement_value)) goto label_format;

			t_asn_v2_value *element_value = NULL;
			while (ft_list_next_content(&asn1_type->as.composite.elements, &anext, &content)) {
				t_asn_v2_component *element = content;
				assert(NULL != element->id && NULL != element->type);

				bool id_match = ft_streq(element->id, jelement_id->as.cstring);

				if (id_match) {
					if (SSL_OK != __asn1_v2_schema_parse_value(symbol_table, element->type, &element_value, jelement_value)) {
						goto label_error;
					}
					ft_list_append_content(&avalue->as.composite.elements, element_value);
					break;
				}
				else {
					if (NULL != element->default_value) {
						element_value = __asn1_v2_value_copy_adapter(element->default_value);
						ft_list_append_content(&avalue->as.composite.elements, element_value);
						break;
					}
					else if (!element->optional) {
						goto label_error;
					}
				}
			}
		}
		break;
	}
	case ASN_V2_TYPE_KIND_SET: {
		expected_fmt = "[[ID_1, VALUE_1], [ID_2, VALUE_2], ..., [ID_N, VALUE_N]]";
		if (jvalue->kind != JSON_V2_TYPE_OBJECT) goto label_format;
		t_list_next anext = {0};
		void *content = NULL;

		if (jvalue->as.list.size > asn1_type->as.composite.elements.size) {
			goto label_error;
		}
		while (ft_list_next_content(&asn1_type->as.composite.elements, &anext, &content)) {
			t_asn_v2_component *element = content;
			assert(NULL != element->id && NULL != element->type);
			t_asn_v2_value *element_value = NULL;
			const t_json_v2 *jelement_value = NULL;

			bool id_exists = (JSON_V2_OK == json_v2_query(element->id, jvalue, &jelement_value));

			if (id_exists) {
				if (SSL_OK != __asn1_v2_schema_parse_value(symbol_table, element->type, &element_value, jelement_value)) {
					goto label_error;
				}
				ft_list_append_content(&avalue->as.composite.elements, element_value);
			}
			else {
				if (NULL != element->default_value) {
					element_value = __asn1_v2_value_copy_adapter(element->default_value);
					ft_list_append_content(&avalue->as.composite.elements, element_value);
				}
				else if (!element->optional) {
					goto label_error;
				}
			}
		}
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF: {
		expected_fmt = "[VALUE_1, VALUE_2, ..., VALUE_N]";
		if (jvalue->kind != JSON_V2_TYPE_ARRAY) goto label_format;
		assert(NULL != asn1_type->as.collection.element_type);
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&jvalue->as.list, &next, &content)) {
			t_asn_v2_value *element_value = NULL;
			if (SSL_OK != __asn1_v2_schema_parse_value(symbol_table, asn1_type->as.collection.element_type, &element_value, content)) {
				goto label_error;
			};
			ft_list_append_content(&avalue->as.composite.elements, element_value);
		}
		break;
	}
	case ASN_V2_TYPE_KIND_CHOICE: {
		expected_fmt = "{ID: VALUE}";
		if (jvalue->kind != JSON_V2_TYPE_OBJECT) goto label_format;
		t_list_next next = {0};
		void *content = NULL;
		bool found = false;
		while (!found && ft_list_next_content(&asn1_type->as.composite.elements, &next, &content)) {
			t_asn_v2_component *alt = content;
			assert(NULL != alt->id && NULL != alt->type);
			const t_json_v2 *jalt_value = NULL;
			if (JSON_V2_OK == json_v2_query(alt->id, jvalue, &jalt_value)) {
				found = true;
				t_asn_v2_value *alt_value = NULL;
				if (SSL_OK != __asn1_v2_schema_parse_value(symbol_table, alt->type, &alt_value, jalt_value)) {
					goto label_error;
				};
				avalue->as.choice.id = ft_strdup(alt->id);
				avalue->as.choice.value = alt_value;
			}
		}
		if (!found) goto label_error;
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		SSL_LOG(ERROR, "parsing values for `%s` type is not supported", asn1_v2_get_type_name(asn1_type->kind));
		goto label_error;
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 type: %d", asn1_type->kind);
		goto label_error;
	}}

	*asn1_value = avalue;
	return (SSL_OK);

label_format:
	if (NULL != expected_fmt) SSL_LOG(ERROR, "value does not match expected format: %s", expected_fmt);
	SSL_LOG(ERROR, "provided json value is not a valid asn1 %s: %s", asn1_v2_get_type_name(asn1_type->kind), json_v2_dumpb(jvalue, cbuf, sizeof(cbuf)));
	asn1_v2_value_delete(avalue);
	return (SSL_ERR);

label_error:
	SSL_LOG(ERROR, "failed to parse json value: %s", json_v2_dumpb(jvalue, cbuf, sizeof(cbuf)));
	asn1_v2_value_delete(avalue);
	return (SSL_ERR);
}

static int	__asn1_v2_schema_parse_tag(t_asn_v2_symbol_table *symbol_table, t_asn_v2_tag **asn1_tag, const t_json_v2 *jtag)
{
	if (jtag->kind != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid asn1 tag: expected json object but got json %s: %s", json_v2_get_type_name(jtag->kind));
		return (SSL_ERR);
	}
	*asn1_tag = NULL;

	char cbuf[1024] = {0};

	t_asn_v2_tag *atag = asn1_v2_tag_create();

	const t_json_v2 *jmode = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TAG_MODE, jtag, &jmode)) {
		if (jmode->kind != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid asn1 tag: expected `%s` to be a json string but got json %s: %s", __JQ_TAG_MODE, json_v2_get_type_name(jmode->kind), json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		SSL_LOG(TRACE, "asn1 tag: `%s`: %s", __JQ_TAG_MODE, json_v2_dumpb(jmode, cbuf, sizeof(cbuf)));
		t_asn_v2_tag_mode tag_mode = asn1_v2_get_tag_mode_by_name(jmode->as.cstring);
		if (tag_mode == -1) {
			SSL_LOG(ERROR, "invalid asn1 tag: bad tag mode `%s`: %s", jmode->as.cstring, json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		atag->mode = tag_mode;
	}

	const t_json_v2 *jclass = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TAG_CLASS, jtag, &jclass)) {
		SSL_LOG(ERROR, "invalid asn1 tag: expected `%s` key: %s", __JQ_TAG_CLASS, json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
		goto label_error;
	}
	if (jclass->kind != JSON_V2_TYPE_STRING) {
		SSL_LOG(ERROR, "invalid asn1 tag: expected `%s` to be a json string but got json %s: %s", __JQ_TAG_CLASS, json_v2_get_type_name(jclass->kind), json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
		goto label_error;
	}
	SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TAG_CLASS, json_v2_dumpb(jclass, cbuf, sizeof(cbuf)));
	t_asn_v2_tag_class tag_class = asn1_v2_get_tag_class_by_name(jclass->as.cstring);
	if (tag_class == -1) {
		SSL_LOG(ERROR, "invalid asn1 tag: bad tag class: `%s`: %s", jclass->as.cstring, json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
		goto label_error;
	}
	atag->class = tag_class;

	const t_json_v2 *jnumber = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TAG_NUMBER, jtag, &jnumber)) {
		SSL_LOG(ERROR, "invalid asn1 tag: expected `%s` key: %s", __JQ_TAG_NUMBER, json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
		goto label_error;
	}
	if (jnumber->kind != JSON_V2_TYPE_NUMBER) {
		SSL_LOG(ERROR, "invalid asn1 tag: expected `%s` to be a json number but got json %s: %s", __JQ_TAG_NUMBER, json_v2_get_type_name(jnumber->kind), json_v2_dumpb(jtag, cbuf, sizeof(cbuf)));
		goto label_error;
	}
	SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TAG_NUMBER, json_v2_dumpb(jnumber, cbuf, sizeof(cbuf)));
	atag->number = (uint32_t)bnum_to_dig_u(&jnumber->as.number);

	*asn1_tag = atag;
	return (SSL_OK);

label_error:
	asn1_v2_tag_delete(atag);
	return (SSL_ERR);
}

static int	__asn1_v2_schema_parse_constraint(t_asn_v2_symbol_table *symbol_table, t_asn_v2_constraint **asn1_constraint, const t_json_v2 *jconstraint)
{
	assert(jconstraint->kind == JSON_V2_TYPE_OBJECT);
	*asn1_constraint = NULL;

	char cbuf[1024] = {0};

	t_asn_v2_constraint *aconstraint = asn1_v2_constraint_create();

	const t_json_v2 *jtype = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_CONSTRAINT_TYPE, jconstraint, &jtype)) {
		SSL_LOG(ERROR, "invalid asn1 constraint: expected `%s` key: %s", __JQ_CONSTRAINT_TYPE, json_v2_dumpb(jconstraint, cbuf, sizeof(cbuf)));
		goto label_error;
	}
	if (jtype->kind != JSON_V2_TYPE_STRING) {
		SSL_LOG(ERROR, "invalid asn1 constraint: expected `%s` to be a json string but got json %s: %s", __JQ_CONSTRAINT_TYPE, json_v2_get_type_name(jtype->kind), json_v2_dumpb(jconstraint, cbuf, sizeof(cbuf)));
		goto label_error;
	}
	SSL_LOG(TRACE, "asn1 constraint: `%s`: %s", __JQ_CONSTRAINT_TYPE, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
	t_asn_v2_constraint_kind kind = asn1_v2_get_constraint_type_by_name(jtype->as.cstring);
	if (kind == -1) {
		SSL_LOG(ERROR, "invalid asn1 constraint: bad type `%s`: %s", jconstraint->as.cstring, json_v2_dumpb(jconstraint, cbuf, sizeof(cbuf)));
		goto label_error;
	}
	aconstraint->kind = kind;

	const t_json_v2 *jmin = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_CONSTRAINT_MIN, jconstraint, &jmin)) {
		if (jmin->kind != JSON_V2_TYPE_NUMBER) {
			SSL_LOG(ERROR, "invalid asn1 constraint: expected `%s` to be a json number but got json %s: %s", __JQ_CONSTRAINT_MIN, json_v2_get_type_name(jmin->kind), json_v2_dumpb(jconstraint, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		SSL_LOG(TRACE, "asn1 constraint: `%s`: %s", __JQ_CONSTRAINT_MIN, json_v2_dumpb(jmin, cbuf, sizeof(cbuf)));
	}

	const t_json_v2 *jmax = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_CONSTRAINT_MAX, jconstraint, &jmax)) {
		if (jmax->kind != JSON_V2_TYPE_NUMBER) {
			SSL_LOG(ERROR, "invalid asn1 constraint: expected `%s` to be a json number but got json %s: %s", __JQ_CONSTRAINT_MAX, json_v2_get_type_name(jmax->kind), json_v2_dumpb(jconstraint, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		SSL_LOG(TRACE, "asn1 constraint: `%s`: %s", __JQ_CONSTRAINT_MAX, json_v2_dumpb(jmax, cbuf, sizeof(cbuf)));
	}

	switch (aconstraint->kind) {
	case ASN_V2_CONSTRAINT_KIND_RANGE:
		if (NULL == jmin) aconstraint->as.range.min = bnum_clone(&jmin->as.number);
		if (NULL == jmax) aconstraint->as.range.max = bnum_clone(&jmax->as.number);
		break;
	case ASN_V2_CONSTRAINT_KIND_SIZE:
		aconstraint->as.size.min = (NULL == jmin) ? (size_t)bnum_to_dig_u(&jmin->as.number) : 0;
		aconstraint->as.size.max = (NULL == jmax) ? (size_t)bnum_to_dig_u(&jmax->as.number) : SIZE_MAX;
		break;
	default:
		UNREACHABLE("__asn1_v2_schema_parse_constraint");
	}

	*asn1_constraint = aconstraint;
	return (SSL_OK);

label_error:
	SSL_FREE(aconstraint);
	return (SSL_ERR);
}

static int	__asn1_v2_schema_parse_type(t_asn_v2_symbol_table *symbol_table, t_asn_v2_type **asn1_type, const t_json_v2 *jtype)
{
	if (jtype->kind != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "expected json object but got json %s", json_v2_get_type_name(jtype->kind));
		return (SSL_ERR);
	}
	*asn1_type = NULL;

	char cbuf[1024] = {0};

	const t_json_v2 *jkind = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_KIND, jtype, &jkind)) {
		SSL_LOG(ERROR, "expected `%s` key", __JQ_TYPE_KIND);
		return (SSL_ERR);
	}
	if (jkind->kind != JSON_V2_TYPE_STRING) {
		SSL_LOG(ERROR, "expected `%s` to be a json string but got json %s", __JQ_TYPE_KIND, json_v2_get_type_name(jkind->kind));
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "asn1 type: `%s`: %s", __JQ_TYPE_KIND, json_v2_dumpb(jkind, cbuf, sizeof(cbuf)));

	const char *type_name = jkind->as.cstring;

	// Check if this is a type reference.
	if (__asn1_v2_schema_is_type_ref_symbol(symbol_table, type_name)) {
		const t_asn_v2_symbol *symbol = NULL;
		if (SSL_OK == __asn1_v2_schema_resolve_symbol(symbol_table, type_name, &symbol)) {
			if (symbol->kind != ASN_V2_SYMBOL_KIND_TYPE_REF) {
				SSL_LOG(ERROR, "not a type reference: %s", type_name);
				return (SSL_ERR);
			}
			*asn1_type = __asn1_v2_type_copy_adapter(symbol->resolved.type_ref.type);
			return (SSL_OK);
		}
	}

	t_asn_v2_type *atype = asn1_v2_type_create();
	atype->kind = asn1_v2_get_type_by_name(type_name);

	switch (atype->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET: {
		const t_json_v2 *jcomponents = NULL;
		if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_COMPONENTS, jtype, &jcomponents)) {
			if (jcomponents->kind != JSON_V2_TYPE_ARRAY) {
				SSL_LOG(ERROR, "expected `%s` to be a json array but got json %s", __JQ_TYPE_COMPONENTS, json_v2_get_type_name(jcomponents->kind));
				goto label_error;
			}
			void *content = NULL;
			t_list_next next = {0};
			while (ft_list_next_content(&jcomponents->as.list, &next, &content)) {
				t_asn_v2_component *component = NULL;
				if (SSL_OK != __asn1_v2_schema_parse_component(symbol_table, &component, content)) {
					SSL_LOG(ERROR, "failed to parse %s field: %s", asn1_v2_get_type_name(atype->kind), json_v2_dumpb(content, cbuf, sizeof(cbuf)));
					goto label_error;
				}
				if (NULL == component->id) {
					SSL_LOG(ERROR, "id is missing in %s field: %s", asn1_v2_get_type_name(atype->kind), json_v2_dumpb(content, cbuf, sizeof(cbuf)));
					goto label_error;
				}
				if (NULL != component->default_value && ASN_V2_TYPE_KIND_CHOICE == atype->kind) {
					SSL_LOG(ERROR, "default values are not allowed in %s fields: %s", asn1_v2_get_type_name(atype->kind), json_v2_dumpb(content, cbuf, sizeof(cbuf)));
					goto label_error;
				}
				ft_list_append_content(&atype->as.composite.elements, component);
			}
		}
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF: {
		const t_json_v2 *jcomponents = NULL;
		if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_COMPONENTS, jtype, &jcomponents)) {
			SSL_LOG(ERROR, "`%s` is not allowed in %s type", __JQ_TYPE_COMPONENTS, asn1_v2_get_type_name(atype->kind));
			goto label_error;
		}
		const t_json_v2 *jelement_type = NULL;
		if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_COMPONENT_TYPE, jtype, &jelement_type)) {
			SSL_LOG(TRACE, "asn1 component: `%s`: %s", __JQ_TYPE_COMPONENT_TYPE, json_v2_dumpb(jelement_type, cbuf, sizeof(cbuf)));
			if (SSL_OK != __asn1_v2_schema_parse_type(symbol_table, &atype->as.collection.element_type, jelement_type)) {
				SSL_LOG(ERROR, "failed to parse %s field type", asn1_v2_get_type_name(atype->kind));
				goto label_error;
			}
		}
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		const t_json_v2 *jenumeration = NULL;
		if (JSON_V2_OK != json_v2_query_nonnull(__JQ_TYPE_ENUMERATION, jtype, &jenumeration)) {
			SSL_LOG(ERROR, "invalid asn1 type: `%s` is missing in %s type", __JQ_TYPE_ENUMERATION, asn1_v2_get_type_name(atype->kind));
			goto label_error;
		}
		if (jenumeration->kind != JSON_V2_TYPE_ARRAY) {
			SSL_LOG(ERROR, "invalid asn1 type: expected `%s` to be json array but got json %s", __JQ_TYPE_ENUMERATION, json_v2_get_type_name(jenumeration->kind));
			goto label_error;
		}
		t_htbl_v2 *htable = ft_htbl_v2_create(1024);
		t_list_next next = {0};
		void *content = NULL;
		t_num last = {0};
		bnum_set_dig_u(&last, 0u);
		while (ft_list_next_content(&jenumeration->as.list, &next, &content)) {
			t_asn_v2_enum *item = NULL;
			const t_json_v2 *jelement = content;
			if (jelement->kind == JSON_V2_TYPE_STRING && ft_streq(__JQ_TOKEN_ELLIPSIS, jelement->as.cstring)) {
				// Special ellipsis token: not an enumeration item but indicates that every enumeration after this point is an extension.
				atype->as.enumeration.extendable = true;
				continue;
			}
			if (jelement->kind != JSON_V2_TYPE_ARRAY) {
				SSL_LOG(ERROR, "expected `%s` element to be a json array but got json %s: %s", __JQ_TYPE_ENUMERATION, json_v2_get_type_name(jelement->kind), json_v2_dumpb(jelement, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			if (jelement->as.list.size != 1 && jelement->as.list.size != 2) {
				SSL_LOG(ERROR, "expected `%s` element to be a json array of size 1 or 2 but got %zu: %s", __JQ_TYPE_ENUMERATION, jelement->as.list.size, json_v2_dumpb(jelement, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			const t_json_v2 *jname = NULL;
			const t_json_v2 *jnumber = NULL;
			if (JSON_V2_OK != json_v2_query_nonnull("[0]", jelement, &jname)) {
				SSL_LOG(ERROR, "expected `%s` 1st element to be present: %s", __JQ_TYPE_ENUMERATION, json_v2_dumpb(jelement, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			if (jname->kind != JSON_V2_TYPE_STRING) {
				SSL_LOG(ERROR, "expected `%s` 1st element to be a json string but got json %s: %s", __JQ_TYPE_ENUMERATION, json_v2_get_type_name(jname->kind), json_v2_dumpb(jelement, cbuf, sizeof(cbuf)));
				goto label_error;
			}
			if (JSON_V2_OK == json_v2_query_nonnull("[1]", jelement, &jnumber)) {
				if (jnumber->kind != JSON_V2_TYPE_NUMBER) {
					SSL_LOG(ERROR, "expected `%s` 2nd element to be a json number but got json %s: %s", __JQ_TYPE_ENUMERATION, json_v2_get_type_name(jnumber->kind), json_v2_dumpb(jelement, cbuf, sizeof(cbuf)));
					goto label_error;
				}
			}
			item = asn1_v2_enum_create();
			item->name = ft_strdup(jname->as.cstring);
			if (NULL != jnumber) {
				bnum_copy(&jnumber->as.number, &item->number);
				bnum_add_dig(&jnumber->as.number, 1, &last);
				char *dumps = bnum_to_dec(&jnumber->as.number);
				bool ok = ft_htbl_v2_set(htable, dumps, NULL);
				SSL_FREE(dumps);
				if (!ok) {
					ft_htbl_v2_del(htable, NULL);
					SSL_LOG(ERROR, "invalid asn1 type: expected `%s` element to be a unique enumeration: %s", __JQ_TYPE_ENUMERATION, json_v2_dumpb(jelement, cbuf, sizeof(cbuf)));
					goto label_error;
				}
			} else {
				bnum_copy(&last, &item->number);
				bnum_add_dig(&last, 1, &last);
			}
			ft_list_append_content(&atype->as.enumeration.items, item);
		}
		ft_htbl_v2_del(htable, NULL);
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
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
		break;
	default:
		SSL_LOG(ERROR, "unknown type or type reference: %s", type_name);
		goto label_error;
	}

	const t_json_v2 *jtags = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_TAGS, jtype, &jtags)) {
		if (jtags->kind != JSON_V2_TYPE_ARRAY) {
			SSL_LOG(ERROR, "expected `%s` to be a json array but got json %s", __JQ_TYPE_TAGS, json_v2_get_type_name(jtags->kind));
			goto label_error;
		}
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&jtags->as.list, &next, &content)) {
			t_asn_v2_tag *atag = NULL;
			if (SSL_OK != __asn1_v2_schema_parse_tag(symbol_table, &atag, content)) {
				SSL_LOG(ERROR, "failed to parse `%s` in asn1 type", __JQ_TYPE_TAGS);
				goto label_error;
			}
			ft_list_append_content(&atype->tags, atag);
		}
	}

	const t_json_v2 *jconstraints = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_CONSTRAINTS, jtype, &jconstraints)) {
		if (jtags->kind != JSON_V2_TYPE_ARRAY) {
			SSL_LOG(ERROR, "expected `%s` to be a json array but got json %s", __JQ_TYPE_CONSTRAINTS, json_v2_get_type_name(jconstraints->kind));
			goto label_error;
		}
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&jconstraints->as.list, &next, &content)) {
			t_asn_v2_constraint *aconstraint = NULL;
			if (SSL_OK != __asn1_v2_schema_parse_constraint(symbol_table, &aconstraint, content)) {
				SSL_LOG(ERROR, "failed to parse `%s` in asn1 type", __JQ_TYPE_CONSTRAINTS);
				goto label_error;
			}
			ft_list_append_content(&atype->constraints, aconstraint);
		}
	}

	*asn1_type = atype;
	return (SSL_OK);

label_error:
	SSL_LOG(ERROR, "failed to parse asn1 type: %s", json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
	asn1_v2_type_delete(atype);
	return (SSL_ERR);
}

static int	__asn1_v2_schema_parse_component(t_asn_v2_symbol_table *symbol_table, t_asn_v2_component **asn1_component, const t_json_v2 *jcomponent)
{
	if (jcomponent->kind != JSON_V2_TYPE_OBJECT) {
		SSL_LOG(ERROR, "invalid asn1 component: expected a json object but got json %s", json_v2_get_type_name(jcomponent->kind));
		return (SSL_ERR);
	}
	*asn1_component = NULL;

	t_asn_v2_component *acomponent = asn1_v2_component_create();
	char cbuf[1024] = {0};

	const t_json_v2 *jid = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_COMPONENT_ID, jcomponent, &jid)) {
		if (jid->kind != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid asn1 component: expected `%s` to be a json string but got json %s", __JQ_COMPONENT_ID, json_v2_get_type_name(jid->kind));
			return (SSL_ERR);
		}
		SSL_LOG(TRACE, "asn1 component: `%s`: %s", __JQ_COMPONENT_ID, json_v2_dumpb(jid, cbuf, sizeof(cbuf)));
		acomponent->id = ft_strdup(jid->as.cstring);
	}

	const t_json_v2 *jtype = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_COMPONENT_TYPE, jcomponent, &jtype)) {
		if (jtype->kind != JSON_V2_TYPE_OBJECT) {
			SSL_LOG(ERROR, "invalid asn1 component: expected `%s` to be a json object but got json %s", __JQ_COMPONENT_TYPE, json_v2_get_type_name(jtype->kind));
			return (SSL_ERR);
		}
		SSL_LOG(TRACE, "asn1 component: `%s`: %s", __JQ_COMPONENT_TYPE, json_v2_dumpb(jtype, cbuf, sizeof(cbuf)));
		if (SSL_OK != __asn1_v2_schema_parse_type(symbol_table, &acomponent->type, jtype)) {
			SSL_LOG(ERROR, "failed to parse `%s` in asn1 component: %s", __JQ_COMPONENT_TYPE, json_v2_dumpb(jcomponent, cbuf, sizeof(cbuf)));
			goto label_error;
		}
	}

	const t_json_v2 *joptional = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_COMPONENT_OPTIONAL, jcomponent, &joptional)) {
		if (joptional->kind != JSON_V2_TYPE_BOOL) {
			SSL_LOG(ERROR, "invalid asn1 component: expected `%s` to be a json boolean but got json %s", __JQ_COMPONENT_OPTIONAL, json_v2_get_type_name(joptional->kind));
			return (SSL_ERR);
		}
		SSL_LOG(TRACE, "asn1 component: `%s`: %s", __JQ_COMPONENT_OPTIONAL, json_v2_dumpb(joptional, cbuf, sizeof(cbuf)));
		acomponent->optional = joptional->as.boolean;
	}

	const t_json_v2 *jdefined_by = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_TYPE_DEFINED_BY, jcomponent, &jdefined_by)) {
		if (jdefined_by->kind != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid asn1 component: expected `%s` to be a json string but got json %s", __JQ_TYPE_DEFINED_BY, json_v2_get_type_name(jdefined_by->kind));
			return (SSL_ERR);
		}
		SSL_LOG(TRACE, "asn1 component: `%s`: %s", __JQ_TYPE_DEFINED_BY, json_v2_dumpb(jdefined_by, cbuf, sizeof(cbuf)));
		acomponent->defined_by = ft_strdup(jdefined_by->as.cstring);
	} else {
		acomponent->defined_by = NULL;
	}

	const t_json_v2 *jdefault = NULL;
	if (JSON_V2_OK == json_v2_query(__JQ_COMPONENT_DEFAULT, jcomponent, &jdefault)) {
		if (acomponent->optional) {
			SSL_LOG(ERROR, "cannot have both `%s` and `%s` defined: %s", __JQ_COMPONENT_OPTIONAL, __JQ_COMPONENT_DEFAULT, json_v2_dumpb(jcomponent, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		if (NULL == acomponent->type) {
			SSL_LOG(ERROR, "cannot parse `%s` without `%s` defined: %s", __JQ_COMPONENT_DEFAULT, __JQ_COMPONENT_TYPE, json_v2_dumpb(jcomponent, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		SSL_LOG(TRACE, "asn1 component: `%s`: %s", __JQ_COMPONENT_DEFAULT, json_v2_dumpb(jdefault, cbuf, sizeof(cbuf)));
		if (SSL_OK != __asn1_v2_schema_parse_value(symbol_table, acomponent->type, &acomponent->default_value, jdefault)) {
			SSL_LOG(ERROR, "failed to parse `%s` in asn1 component: %s", __JQ_COMPONENT_DEFAULT, json_v2_dumpb(jcomponent, cbuf, sizeof(cbuf)));
			goto label_error;
		}
	}

	*asn1_component = acomponent;
	return (SSL_OK);

label_error:
	SSL_LOG(ERROR, "failed to parse asn1 component: %s", json_v2_dumpb(jcomponent, cbuf, sizeof(cbuf)));
	asn1_v2_component_delete(acomponent);
	return (SSL_ERR);
}

static int	__asn1_v2_type_resolve_tags(t_asn_v2_module *asn1_module, t_asn_v2_type *asn1_type)
{
	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET: {
		bool has_tagged_component = false;
		void *content = NULL;
		t_list_next next = {0};
		while (ft_list_next_content(&asn1_type->as.composite.elements, &next, &content)) {
			t_asn_v2_component *component = content;
			assert(NULL != component->type);
			if (component->type->tags.size > 0) has_tagged_component = true;
			if (SSL_OK != __asn1_v2_type_resolve_tags(asn1_module, component->type)) {
				return (SSL_ERR);
			}
		}
		if (ASN_V2_TAG_MODE_AUTOMATIC == asn1_module->tag_mode && !has_tagged_component) {
			// We must tag types for all components: implicit for SEQUENCE/SET or explicit for CHOICE.
			uint32_t tag_number = 0;
			next = (t_list_next){0};
			while (ft_list_next_content(&asn1_type->as.composite.elements, &next, &content)) {
				t_asn_v2_component *component = content;
				t_asn_v2_tag *tag = asn1_v2_tag_create();
				tag->mode = (asn1_type->kind == ASN_V2_TYPE_KIND_CHOICE) ? ASN_V2_TAG_MODE_IMPLICIT : ASN_V2_TAG_MODE_EXPLICIT;
				tag->class = ASN_V2_TAG_CLASS_CONTEXT_SPECIFIC;
				tag->number = tag_number;
				ft_list_append_content(&component->type->tags, tag);
				tag_number++;
			}
		}
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF: {
		if (SSL_OK != __asn1_v2_type_resolve_tags(asn1_module, asn1_type->as.collection.element_type)) {
			SSL_LOG(ERROR, "failed to resolve tags for asn1 %s element type", asn1_v2_get_type_name(asn1_type->kind));
			return (SSL_ERR);
		}
		break;
	}
	case ASN_V2_TYPE_KIND_ANY:
	case ASN_V2_TYPE_KIND_BIT_STRING:
	case ASN_V2_TYPE_KIND_INTEGER:
	case ASN_V2_TYPE_KIND_ENUMERATED:
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
		return (SSL_ERR);
	}}

	t_list_next next = {0};
	void *content = NULL;
	while (ft_list_next_content(&asn1_type->tags, &next, &content)) {
		t_asn_v2_tag *tag = content;
		if (tag->mode == ASN_V2_TAG_MODE_DEFAULT) {
			if (ASN_V2_TAG_MODE_AUTOMATIC == asn1_module->tag_mode) {
				tag->mode = ASN_V2_TAG_MODE_EXPLICIT;
			} else {
				tag->mode = asn1_module->tag_mode;
			}
		}
	}
	return (SSL_OK);
}

int	asn1_v2_schema_parse(t_asn_v2_module **asn1_module, const t_json_v2 *jschema)
{
	if (NULL == asn1_module) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "parsing asn1 schema json: %p", jschema);

	t_asn_v2_module *amodule = asn1_v2_module_create();

	const t_json_v2 *jtagmode = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_SCHEMA_TAG_MODE, jschema, &jtagmode)) {
		if (jtagmode->kind != JSON_V2_TYPE_STRING) {
			SSL_LOG(ERROR, "invalid json schema: expected `%s` to be a json string but got %s", __JQ_SCHEMA_TAG_MODE, json_v2_get_type_name(jtagmode->kind));
			goto label_error;
		}
		t_asn_v2_tag_mode tag_mode = asn1_v2_get_tag_mode_by_name(jtagmode->as.cstring);
		if (tag_mode == -1) {
			SSL_LOG(ERROR, "invalid `%s`: %s", __JQ_SCHEMA_TAG_MODE, jtagmode->as.cstring);
			goto label_error;
		}
		amodule->tag_mode = tag_mode;
	}

	const t_json_v2 *jvalues = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_SCHEMA_VALUES, jschema, &jvalues)) {
		if (jvalues->kind != JSON_V2_TYPE_OBJECT) {
			SSL_LOG(ERROR, "invalid schema: expected `%s` to be a json object but got json %s", __JQ_SCHEMA_VALUES, json_v2_get_type_name(jvalues->kind));
			goto label_error;
		}
		const char *key = NULL;
		void *value = NULL;
		t_htbl_v2_next next = {0};
		while (ft_htbl_v2_next(&jvalues->as.htable, &next, &key, &value)) {
			if (SSL_OK != asn1_v2_symbol_register_value(&amodule->symbol_table, key, value)) {
				SSL_LOG(ERROR, "failed to register symbol: %s", key);
				goto label_error;
			}
		}
	}
	const t_json_v2 *jtypes = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull(__JQ_SCHEMA_TYPES, jschema, &jtypes)) {
		if (jtypes->kind != JSON_V2_TYPE_OBJECT) {
			SSL_LOG(ERROR, "invalid schema: expected `%s` to be a json object but got json %s", __JQ_SCHEMA_TYPES, json_v2_get_type_name(jvalues->kind));
			goto label_error;
		}
		const char *key = NULL;
		void *value = NULL;
		t_htbl_v2_next next = {0};
		while (ft_htbl_v2_next(&jtypes->as.htable, &next, &key, &value)) {
			if (SSL_OK != asn1_v2_symbol_register_type(&amodule->symbol_table, key, value)) {
				SSL_LOG(ERROR, "failed to register symbol: %s", key);
				goto label_error;
			}
		}
	}

	const char *key = NULL;
	void *value = NULL;
	t_htbl_v2_next next = {0};
	while (ft_htbl_v2_next(&amodule->symbol_table.symbols, &next, &key, &value)) {
		const t_asn_v2_symbol *symbol = NULL;
		if (SSL_OK != __asn1_v2_schema_resolve_symbol(&amodule->symbol_table, key, &symbol)) {
			SSL_LOG(ERROR, "failed to resolve symbol: `%s`", key);
			goto label_error;
		}
		if (symbol->kind == ASN_V2_SYMBOL_KIND_TYPE_REF) {
			SSL_LOG(TRACE, "compiling automatic tags for asn1 type: %s", key);
			if (SSL_OK != __asn1_v2_type_resolve_tags(amodule, symbol->resolved.type_ref.type)) {
				SSL_LOG(ERROR, "failed to resolve automatic tags for asn1 type: `%s`", key);
				goto label_error;
			}
		}
	}

	*asn1_module = amodule;
	return (SSL_OK);

label_error:
	asn1_v2_module_delete(amodule);
	return (SSL_ERR);
}
