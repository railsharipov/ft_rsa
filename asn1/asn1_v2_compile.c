#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <der.h>
#include <io.h>
#include <libft.h>
#include <bnum.h>

#include "__asn1_private.h"

/****************************************************************************/

int asn1_v2_value_compile(t_der_v2_value **der_value, const t_asn_v2_value *asn1_value)
{
	assert(NULL != asn1_value && NULL != der_value);

	char cbuf[1024];

	t_der_v2_value *compiled = der_v2_value_create();
	compiled->kind = asn1_value->kind;

	switch (asn1_value->kind) {
	case ASN_V2_TYPE_KIND_NULL: {
		break;
	}
	case ASN_V2_TYPE_KIND_BOOLEAN: {
		compiled->as.boolean = asn1_value->as.boolean;
		break;
	}
	case ASN_V2_TYPE_KIND_INTEGER: {
		bnum_copy(&asn1_value->as.number, &compiled->as.number);
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		asn1_v2_enum_copy(&asn1_value->as.enumerated, &compiled->as.enumerated);
		break;
	}
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NUMERIC_STRING:
	case ASN_V2_TYPE_KIND_VISIBLE_STRING:
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
	case ASN_V2_TYPE_KIND_UTC_TIME:
	case ASN_V2_TYPE_KIND_IA5_STRING: {
		if (NULL != asn1_value->as.cstring) compiled->as.cstring = ft_strdup(asn1_value->as.cstring);
		break;
	}
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_ID: {
		t_node_v2 *oid_tree = NULL;
		if (SSL_OK != asn1_v2_oid_tree_create(ASN_V2_OID_TREE_FILE_PATH, &oid_tree)) {
			SSL_LOG(ERROR, "failed to init asn1 oid tree");
			goto label_error;
		}
		t_list_next next = {0};
		void *content = NULL;
		t_asn_v2_oid *compiled_oid = &compiled->as.object_id;
		while (ft_list_next_content(&asn1_value->as.object_id.arcs, &next, &content)) {
			t_asn_v2_oid_arc *arc = content;
			if (compiled_oid->narcs >= ASN_V2_OID_MAX_NARCS) {
				asn1_v2_oid_tree_delete(oid_tree);
				goto label_error;
			}
			switch (arc->kind) {
			case ASN_V2_OID_ARC_KIND_NUMERIC:
			case ASN_V2_OID_ARC_KIND_NAMED_AND_NUMERIC: {
				compiled_oid->arc_nums[compiled_oid->narcs++] = arc->as.number;
				break;
			}
			case ASN_V2_OID_ARC_KIND_NAMED: {
				assert(NULL != arc->as.name);
				size_t arc_number = 0;
				if (SSL_OK != asn1_v2_get_arc_number_by_name(oid_tree, arc->as.name, &arc_number)) {
					asn1_v2_oid_tree_delete(oid_tree);
					goto label_error;
				}
				compiled_oid->arc_nums[compiled_oid->narcs++] = arc_number;
				break;
			}}
		}
		break;
	}
	case ASN_V2_TYPE_KIND_UTF8_STRING:
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_BIT_STRING: {
		ft_ostr_copy(&asn1_value->as.ostring, &compiled->as.ostring);
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET:
	case ASN_V2_TYPE_KIND_SET_OF: {
		if (asn1_value->as.composite.elements.size > 0) {
			SSL_ALLOC(compiled->as.composite.items, asn1_value->as.composite.elements.size * sizeof(t_der_v2_value));
			compiled->as.composite.count = asn1_value->as.composite.elements.size;
			t_list_next next = {0};
			void *content;
			for (size_t idx = 0; ft_list_next_content(&asn1_value->as.composite.elements, &next, &content); idx++) {
				assert(idx < compiled->as.composite.count);
				t_der_v2_value *compiled_element = NULL;
				if (SSL_OK != asn1_v2_value_compile(&compiled_element, content)) {
					goto label_error;
				}
				der_v2_value_copy(compiled_element, compiled->as.composite.items + idx);
				der_v2_value_delete(compiled_element);
			}
		}
		break;
	}
	case ASN_V2_TYPE_KIND_CHOICE: {
		if (NULL != asn1_value->as.choice.id) compiled->as.choice.id = ft_strdup(asn1_value->as.choice.id);
		if (SSL_OK != asn1_v2_value_compile(&compiled->as.choice.value, asn1_value->as.choice.value)) {
			goto label_error;
		}
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		ft_ostr_copy(&asn1_value->as.any.octets, &compiled->as.any.octets);
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 value type: %d", asn1_value->kind);
		goto label_error;
	}}
	*der_value = compiled;
	return (SSL_OK);

label_error:
	SSL_LOG(ERROR, "failed to compile `%s` asn1 value: %s", asn1_v2_get_type_name(asn1_value->kind), asn1_v2_value_dumpb(asn1_value, cbuf, sizeof(cbuf)));
	der_v2_value_delete(compiled);
	return (SSL_ERR);
}

static int __asn1_v2_component_compile(t_der_v2_component **der_component, const t_asn_v2_component *asn1_component)
{
	char cbuf[1024] = {0};

	if (NULL != asn1_component->id) {
		SSL_LOG(TRACE, "compiling `%s` asn1 component", asn1_component->id);
	} else {
		SSL_LOG(TRACE, "compiling asn1 component: %s", asn1_v2_component_dumpb(asn1_component, cbuf, sizeof(cbuf)));
	}

	t_der_v2_type *compiled = NULL;
	if (SSL_OK != asn1_v2_type_compile(&compiled, asn1_component->type)) {
		SSL_LOG(ERROR, "failed to compile asn1 component type: %s", asn1_v2_type_dumpb(asn1_component->type, cbuf, sizeof(cbuf)));
		return (SSL_ERR);
	}

	t_der_v2_component *dcomponent = der_v2_component_create();

	if (NULL != asn1_component->default_value) {
		dcomponent->default_value = NULL;
		if (SSL_OK != asn1_v2_value_compile(&dcomponent->default_value, asn1_component->default_value)) {
			SSL_LOG(ERROR, "failed to compile asn1 component default value: %s", asn1_v2_value_dumpb(asn1_component->default_value, cbuf, sizeof(cbuf)));
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

	t_der_v2_type *compiled = der_v2_type_create();
	compiled->kind = asn1_type->kind;

	switch (asn1_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SET: {
		// Compile type components.
		{
			t_list_next next = {0};
			void *content = NULL;
			while (ft_list_next_content(&asn1_type->as.composite.elements, &next, &content)) {
				t_asn_v2_component *element = content;
				t_der_v2_component *compiled_element = NULL;
				if (SSL_OK != __asn1_v2_component_compile(&compiled_element, element)) {
					SSL_LOG(ERROR, "failed to compile asn1 %s component", asn1_v2_get_type_name(asn1_type->kind));
					goto label_error;
				}
				ft_list_append_content(&compiled->as.composite.elements, compiled_element);
			}
		}
		// Run through a list a second time to resolve component references, if any.
		{
			t_list_next next = {0};
			t_list_next next_compiled = {0};
			void *content = NULL;
			while (ft_list_next_content(&asn1_type->as.composite.elements, &next, &content)) {
				t_asn_v2_component *element = content;
				bool ok = ft_list_next_content(&compiled->as.composite.elements, &next_compiled, &content);
				assert(ok);
				t_der_v2_component *compiled_element = content;
				if (NULL != element->defined_by) {
					const t_der_v2_component *defined_by_compiled_element = der_v2_component_find_by_id(compiled->as.composite.elements, element->defined_by);
					if (NULL == defined_by_compiled_element) {
						SSL_LOG(ERROR, "invalid asn1 %s component reference: defined by: %s", element->defined_by);
						goto label_error;
					}
					compiled_element->defined_by = defined_by_compiled_element;
				}
			}
		}
		break;
	}
	case ASN_V2_TYPE_KIND_CHOICE: {
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&asn1_type->as.composite.elements, &next, &content)) {
			t_der_v2_component *compiled_element = NULL;
			if (SSL_OK != __asn1_v2_component_compile(&compiled_element, content)) {
				SSL_LOG(ERROR, "failed to compile asn1 %s alternative", asn1_v2_get_type_name(asn1_type->kind));
				goto label_error;
			}
			if (NULL == compiled_element->id) {
				SSL_LOG(ERROR, "missing id for asn1 %s alternative", asn1_v2_get_type_name(asn1_type->kind));
				goto label_error;
			}
			if (NULL != compiled_element->default_value) {
				SSL_LOG(ERROR, "unexpected default value for asn1 %s alternative", asn1_v2_get_type_name(asn1_type->kind));
				goto label_error;
			}
			ft_list_append_content(&compiled->as.composite.elements, compiled_element);
		}
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF: {
		if (SSL_OK != asn1_v2_type_compile(&compiled->as.collection.element_type, asn1_type->as.collection.element_type)) {
			SSL_LOG(ERROR, "failed to compile asn1 %s collection element type", asn1_v2_get_type_name(asn1_type->kind));
			goto label_error;
		}
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		ft_list_copy_all_content(&asn1_type->as.enumeration.items, &compiled->as.enumeration.items, __asn1_v2_enum_copy_adapter);
		compiled->as.enumeration.extendable = asn1_type->as.enumeration.extendable;
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
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
	case ASN_V2_TYPE_KIND_UTC_TIME:
	case ASN_V2_TYPE_KIND_NULL:
	case ASN_V2_TYPE_KIND_OBJECT_ID:
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_DESCR: {
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 type: %d", asn1_type->kind);
		goto label_error;
	}}

	// CHOICE and ANY do not have their own tag.
	if (ASN_V2_TYPE_KIND_CHOICE != asn1_type->kind && ASN_V2_TYPE_KIND_ANY != asn1_type->kind) {
		// Semantic tag is the one describing which type decoder should expect.
		const t_der_v2_tag semantic_tag = der_v2_get_tag_universal(asn1_type->kind);
		// The innermost tag shall be the one that wraps the value of a type known to decoder.
		t_der_v2_tag *inner_tag = der_v2_tag_create();
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
			SSL_LOG(ERROR, "cannot use reserved tag: %s", asn1_v2_get_type_name(asn1_type->kind), asn1_v2_tag_dumpb(asn1_tag, cbuf, sizeof(cbuf)));
			goto label_error;
		}
		t_der_v2_tag *compiled_tag = NULL;
		switch (asn1_tag->mode) {
		case ASN_V2_TAG_MODE_DEFAULT:
		case ASN_V2_TAG_MODE_AUTOMATIC: {
			SSL_LOG(ERROR, "unresolved tag mode: %s", asn1_v2_get_tag_mode_name(asn1_tag->mode));
			goto label_error;
		}
		case ASN_V2_TAG_MODE_EXPLICIT: {
			compiled_tag = der_v2_tag_create();
			compiled_tag->class = asn1_tag->class;
			compiled_tag->number = asn1_tag->number;
			compiled_tag->constructed = true;
			ft_list_append_content(&compiled->tags, compiled_tag);
			break;
		}
		case ASN_V2_TAG_MODE_IMPLICIT: {
			if (ASN_V2_TYPE_KIND_CHOICE == asn1_type->kind || ASN_V2_TYPE_KIND_ANY == asn1_type->kind) {
				SSL_LOG(ERROR, "implicit tags are not allowed for %s asn1 type", asn1_v2_get_type_name(asn1_type->kind));
				goto label_error;
			}
			compiled_tag = der_v2_tag_create();
			compiled_tag->class = asn1_tag->class;
			compiled_tag->number = asn1_tag->number;
			// An implicit tag's constructedness is defined by the type it wraps.
			const t_der_v2_tag semantic_tag = der_v2_get_tag_universal(asn1_type->kind);
			compiled_tag->constructed = semantic_tag.constructed;
			// An implicit tag replaces all underlying tags, including the innermost tag.
			ft_list_clear_all_content(&compiled->tags, __asn1_v2_tag_delete_adapter);
			ft_list_append_content(&compiled->tags, compiled_tag);
			break;
		}
		default: {
			SSL_LOG(ERROR, "unknown asn1 tag mode: %d", asn1_tag->mode);
			goto label_error;
		}}
	}

	ft_list_copy_all_content(&asn1_type->constraints, &compiled->constraints, __asn1_v2_constraint_copy_adapter);

	*der_type = compiled;
	return (SSL_OK);

label_error:
	SSL_LOG(ERROR, "failed to compile `%s` asn1 type: %s", asn1_v2_get_type_name(asn1_type->kind), asn1_v2_type_dumpb(asn1_type, cbuf, sizeof(cbuf)));
	der_v2_type_delete(compiled);
	return (SSL_ERR);
}

/****************************************************************************/

char *der_v2_tag_dumps(const t_der_v2_tag *der_tag)
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

char *der_v2_value_dumps(const t_der_v2_value *der_value)
{
	if (NULL == der_value) return ft_strdup("null");

	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);

	const char *type_name = asn1_v2_get_type_name(der_value->kind);
	ft_ostr_appendf(&ostring, "{\"%s\":", type_name);

	switch (der_value->kind) {
	case ASN_V2_TYPE_KIND_NULL: {
		ft_ostr_append_cstr(&ostring, "null");
		break;
	}
	case ASN_V2_TYPE_KIND_BOOLEAN: {
		if (der_value->as.boolean) {
			ft_ostr_append_cstr(&ostring, "true");
		} else {
			ft_ostr_append_cstr(&ostring, "false");
		}
		break;
	}
	case ASN_V2_TYPE_KIND_INTEGER: {
		char *dumps = bnum_to_dec(&der_value->as.number);
		ft_ostr_append_cstr(&ostring, dumps);
		SSL_FREE(dumps);
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		char *dumps = bnum_to_dec(&der_value->as.enumerated.number);
		if (NULL != der_value->as.enumerated.name) {
			ft_ostr_appendf(&ostring, "[\"%s\", %s]", der_value->as.enumerated.name, dumps);
		} else {
			ft_ostr_appendf(&ostring, "[%s]", dumps);
		}
		SSL_FREE(dumps);
		break;
	}
	case ASN_V2_TYPE_KIND_OBJECT_DESCR:
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING:
	case ASN_V2_TYPE_KIND_NUMERIC_STRING:
	case ASN_V2_TYPE_KIND_VISIBLE_STRING:
	case ASN_V2_TYPE_KIND_IA5_STRING: {
		t_ostring tmp = {0};
		ft_ostr_appendf(&tmp, "\"%s\"", der_value->as.cstring);
		char *escaped = json_v2_escape_string((const char *)tmp.content, tmp.size);
		ft_ostr_clear(&tmp);
		ft_ostr_appendf(&ostring, "\"%s\"", escaped);
		SSL_FREE(escaped);
		break;
	}
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME:
	case ASN_V2_TYPE_KIND_UTC_TIME: {
		ft_ostr_appendf(&ostring, "\"%s\"", der_value->as.cstring);
		break;
	}
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID:
	case ASN_V2_TYPE_KIND_OBJECT_ID: {
		char *dumps = asn1_v2_oid_dumps(&der_value->as.object_id);
		ft_ostr_append_cstr(&ostring, dumps);
		SSL_FREE(dumps);
		break;
	}
	case ASN_V2_TYPE_KIND_UTF8_STRING: {
		t_ostring tmp = {0};
		ft_ostr_init_with_capacity(&tmp, der_value->as.ostring.size+2);
		ft_ostr_append_cstr(&tmp, "\"");
		ft_ostr_append(&tmp, der_value->as.ostring.content, der_value->as.ostring.size);
		ft_ostr_append_cstr(&tmp, "\"");
		char *escaped = json_v2_escape_string((const char *)tmp.content, tmp.size);
		ft_ostr_clear(&tmp);
		ft_ostr_appendf(&ostring, "\"%s\"", escaped);
		SSL_FREE(escaped);
		break;
	}
	case ASN_V2_TYPE_KIND_OCTET_STRING:
	case ASN_V2_TYPE_KIND_BIT_STRING: {
		ft_ostr_append_cstr(&ostring, "\"'");
		char *hexs = ft_bytes_dumps_hex_upper(der_value->as.ostring.content, der_value->as.ostring.size);
		ft_ostr_append_cstr(&ostring, hexs);
		SSL_FREE(hexs);
		ft_ostr_append_cstr(&ostring, "'H\"");
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET:
	case ASN_V2_TYPE_KIND_SET_OF: {
		ft_ostr_append_cstr(&ostring, "[");
		size_t commas = 0;
		for (size_t idx = 0; idx < der_value->as.composite.count; idx++) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = der_v2_value_dumps(der_value->as.composite.items + idx);
			ft_ostr_append_cstr(&ostring, dumps);
			SSL_FREE(dumps);
		}
		ft_ostr_append_cstr(&ostring, "]");
		break;
	}
	case ASN_V2_TYPE_KIND_CHOICE: {
		char *dumps = der_v2_value_dumps(der_value->as.choice.value);
		if (NULL != der_value->as.choice.id) {
			ft_ostr_appendf(&ostring, "{\"%s\":%s}", der_value->as.choice.id, dumps);
		} else {
			ft_ostr_append_cstr(&ostring, dumps);
		}
		SSL_FREE(dumps);
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		char *dumps = ft_bytes_dumps_hex_upper(der_value->as.any.octets.content, der_value->as.any.octets.size);
		ft_ostr_appendf(&ostring, "\"'%s'H\"", dumps);
		SSL_FREE(dumps);
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown der value type: %d", der_value->kind);
		ft_ostr_append_cstr(&ostring, "\"???\"");
		break;
	}}
	if (der_value->explicit_tags.count > 0) {
		ft_ostr_append_cstr(&ostring, ",\"explicit_tags\":[");
		size_t commas = 0;
		for (size_t i = 0; i < der_value->explicit_tags.count; i++) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = der_v2_tag_dumps(der_value->explicit_tags.items + i);
			ft_ostr_append_cstr(&ostring, dumps);
			SSL_FREE(dumps);
		}
		ft_ostr_append_cstr(&ostring, "]");
	}
	if (der_value->implicit_tags.count > 0) {
		ft_ostr_append_cstr(&ostring, ",\"implicit_tags\":[");
		size_t commas = 0;
		for (size_t i = 0; i < der_value->implicit_tags.count; i++) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = der_v2_tag_dumps(der_value->implicit_tags.items + i);
			ft_ostr_append_cstr(&ostring, dumps);
			SSL_FREE(dumps);
		}
		ft_ostr_append_cstr(&ostring, "]");
	}
	ft_ostr_append_cstr(&ostring, "}");
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

char *der_v2_component_dumps(const t_der_v2_component *der_component)
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

	if (NULL != der_component->defined_by && NULL != der_component->defined_by->id) {
		ft_ostr_appendf(&ostring, ",\"defined_by\":\"%s\"", der_component->defined_by->id);
	}

	if (NULL != der_component->default_value) {
		dumps = der_v2_value_dumps(der_component->default_value);
		ft_ostr_appendf(&ostring, ",\"default_value\":%s", dumps);
		SSL_FREE(dumps);
	}

	ft_ostr_appendf(&ostring, ",\"optional\":%s", der_component->optional ? "true" : "false");
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

	if (der_type->tags.size > 0) {
		ft_ostr_append_cstr(&ostring, ",\"tags\":[");
		t_list_next next = {0};
		void *content = NULL;
		size_t commas = 0;
		while (ft_list_next_content(&der_type->tags, &next, &content)) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *tag_dumps = der_v2_tag_dumps(content);
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
			char *constraint_dumps = asn1_v2_constraint_dumps(content);
			ft_ostr_append_cstr(&ostring, constraint_dumps);
			SSL_FREE(constraint_dumps);
		}
		ft_ostr_append_cstr(&ostring, "]");
	}

	switch (der_type->kind) {
	case ASN_V2_TYPE_KIND_SEQUENCE:
	case ASN_V2_TYPE_KIND_CHOICE:
	case ASN_V2_TYPE_KIND_SET: {
		t_list_next next = {0};
		void *content;
		size_t commas = 0;
		ft_ostr_append_cstr(&ostring, ",\"components\":[");
		while (ft_list_next_content(&der_type->as.composite.elements, &next, &content)) {
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = der_v2_component_dumps(content);
			ft_ostr_append_cstr(&ostring, dumps);
			SSL_FREE(dumps);
		}
		ft_ostr_append_cstr(&ostring, "]");
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE_OF:
	case ASN_V2_TYPE_KIND_SET_OF: {
		char *dumps = der_v2_type_dumps(der_type->as.collection.element_type);
		ft_ostr_appendf(&ostring, ",\"element_type\":%s", dumps);
		SSL_FREE(dumps);
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		t_list_next next = {0};
		void *content = NULL;
		size_t commas = 0;
		ft_ostr_append_cstr(&ostring, ",\"enumeration\":{\"items\":[");
		while (ft_list_next_content(&der_type->as.enumeration.items, &next, &content)) {
			t_asn_v2_enum *item = content;
			if (commas++) ft_ostr_append_cstr(&ostring, ",");
			char *dumps = bnum_to_dec(&item->number);
			ft_ostr_appendf(&ostring, "[\"%s\",%s]", item->name, dumps);
			SSL_FREE(dumps);
		}
		ft_ostr_appendf(&ostring, "],\"extendable\":%s}", der_type->as.enumeration.extendable ? "true" : "false");
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
		SSL_LOG(ERROR, "unknown der type: %d", der_type->kind);
	}}

	ft_ostr_append_cstr(&ostring, "}");
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}

char *der_v2_type_pretty_dumps(const t_der_v2_type *der_type, bool colored)
{
	char *dumps = der_v2_type_dumps(der_type);
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

char *der_v2_value_pretty_dumps(const t_der_v2_value *der_value, bool colored)
{
	char *dumps = der_v2_value_dumps(der_value);
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
