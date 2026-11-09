#include <common.h>
#include <logger.h>
#include <asn1_v2.h>
#include <der.h>
#include <libft.h>
#include <bnum.h>
#include <textutil.h>

#include "__asn1_v2.h"

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
static bool __der_v2_next_tlv_as(const uint8_t *enc, size_t encsize, t_der_v2_tlv *tlv, t_der_v2_type *type);

static int __der_v2_generic_decode_value(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int __der_v2_generic_decode_choice(t_der_v2_tlv tlv, t_der_v2_value **der_value);

static int	__der_v2_generic_decode_ia5_string(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_printable_string(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_numeric_string(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_visible_string(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_utc_time(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_generalized_time(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_octet_string(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_bit_string(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_utf8_string(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_boolean(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_sequence(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_set(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_null(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_integer(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_enumerated(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_object_id(t_der_v2_tlv tlv, t_der_v2_value **der_value);
static int	__der_v2_generic_decode_relative_object_id(t_der_v2_tlv tlv, t_der_v2_value **der_value);

static bool __der_v2_next_tlv(const uint8_t *enc, size_t encsize, t_der_v2_tlv *tlv)
{
	assert(NULL != tlv);

	// SSL_LOG(TRACE, "current tlv: tag={class=%d,number=%d,constructed=%d}, length:%zu, content=%p", tlv->tag.class, tlv->tag.number, tlv->tag.constructed, tlv->length, tlv->value);
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

	tlv->tag.class = DER_V2_TAG_CLASS_MAP[(octet & 0xC0)>>6];
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

static bool __der_v2_next_tlv_as(const uint8_t *enc, size_t encsize, t_der_v2_tlv *tlv, t_der_v2_type *type)
{
	assert(NULL != tlv);

	t_der_v2_tlv tmp = *tlv;
	if (!__der_v2_next_tlv(enc, encsize, &tmp)) return false;

	// Some types like CHOICE don't have their own tags.
	if (type->tags.size > 0) {
		if (!der_v2_tag_eq(&tmp.tag, ft_list_last_content(&type->tags))) return false;
	}
	*tlv = tmp;
	return true;
}

static struct s_der_v2_generic_decode {
	t_func_der_v2_generic_decode f_decode;
	const char *name;
} DER_V2_GENERIC_DECODE_TYPE_MAP[] = {
	/* ASN_V2_TAG_NUMBER_EOC                = 0 */	{ NULL, "EOC" },
	/* ASN_V2_TAG_NUMBER_BOOLEAN            = 1 */	{ __der_v2_generic_decode_boolean, "BOOLEAN" },
	/* ASN_V2_TAG_NUMBER_INTEGER            = 2 */	{ __der_v2_generic_decode_integer, "INTEGER" },
	/* ASN_V2_TAG_NUMBER_BIT_STRING         = 3 */	{ __der_v2_generic_decode_bit_string, "BIT_STRING" },
	/* ASN_V2_TAG_NUMBER_OCTET_STRING       = 4 */	{ __der_v2_generic_decode_octet_string, "OCTET_STRING" },
	/* ASN_V2_TAG_NUMBER_NULL               = 5 */	{ __der_v2_generic_decode_null, "NULL" },
	/* ASN_V2_TAG_NUMBER_OBJECT_ID          = 6 */	{ __der_v2_generic_decode_object_id, "OBJECT_ID" },
	/* ASN_V2_TAG_NUMBER_OBJECT_DESCR       = 7 */	{ NULL, "OBJECT_DESCR" },
	/* ASN_V2_TAG_NUMBER_EXTERNAL           = 8 */	{ NULL, "EXTERNAL" },
	/* ASN_V2_TAG_NUMBER_REAL               = 9 */	{ NULL, "REAL" },
	/* ASN_V2_TAG_NUMBER_ENUMERATED         = 10 */	{ __der_v2_generic_decode_enumerated, "ENUMERATED" },
	/* ASN_V2_TAG_NUMBER_EMBEDDED_PDV       = 11 */	{ NULL, "EMBEDDED_PDV" },
	/* ASN_V2_TAG_NUMBER_UTF8_STRING        = 12 */	{ __der_v2_generic_decode_utf8_string, "UTF8_STRING" },
	/* ASN_V2_TAG_NUMBER_RELATIVE_OBJECT_ID = 13 */	{ __der_v2_generic_decode_relative_object_id, "RELATIVE_OBJECT_ID" },
	/* ASN_V2_TAG_NUMBER_TIME               = 14 */	{ NULL, "TIME" },
	/* ASN_V2_TAG_NUMBER_RESERVED_15        = 15 */	{ NULL, "RESERVED_15" },
	/* ASN_V2_TAG_NUMBER_SEQUENCE(_OF)      = 16 */	{ __der_v2_generic_decode_sequence, "SEQUENCE" },
	/* ASN_V2_TAG_NUMBER_SET(_OF)           = 17 */	{ __der_v2_generic_decode_set, "SET" },
	/* ASN_V2_TAG_NUMBER_NUMERIC_STRING     = 18 */	{ __der_v2_generic_decode_numeric_string, "NUMERIC_STRING" },
	/* ASN_V2_TAG_NUMBER_PRINTABLE_STRING   = 19 */	{ __der_v2_generic_decode_printable_string, "PRINTABLE_STRING" },
	/* ASN_V2_TAG_NUMBER_TELETEX_STRING     = 20 */	{ NULL, "TELETEX_STRING" },
	/* ASN_V2_TAG_NUMBER_VIDEOTEX_STRING    = 21 */	{ NULL, "VIDEOTEX_STRING" },
	/* ASN_V2_TAG_NUMBER_IA5_STRING         = 22 */	{ __der_v2_generic_decode_ia5_string, "IA5_STRING" },
	/* ASN_V2_TAG_NUMBER_UTC_TIME           = 23 */	{ __der_v2_generic_decode_utc_time, "UTC_TIME" },
	/* ASN_V2_TAG_NUMBER_GENERALIZED_TIME   = 24 */	{ __der_v2_generic_decode_generalized_time, "GENERALIZED_TIME" },
	/* ASN_V2_TAG_NUMBER_GRAPHIC_STRING     = 25 */	{ NULL, "GRAPHIC_STRING" },
	/* ASN_V2_TAG_NUMBER_VISIBLE_STRING     = 26 */	{ __der_v2_generic_decode_visible_string, "VISIBLE_STRING" },
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
	t_list explicit_tags = {0};
	t_list implicit_tags = {0};
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
			t_der_v2_tag *tag = der_v2_tag_create();
			der_v2_tag_copy(&tlv.tag, tag);
			if (tlv.tag.constructed) {
				SSL_LOG(TRACE, "context-specific: nested tlv");
				ft_list_append_content(&explicit_tags, tag);
				// This is in external tag with nested TLV. Unwrap nested TLV.
				t_der_v2_tlv nested_tlv = {0};
				if (!__der_v2_next_tlv(tlv.value, tlv.length, &nested_tlv)) {
					SSL_LOG(ERROR, "context-specific: bad nested tlv read");
					goto label_error;
				}
				tlv.tag = nested_tlv.tag;
				tlv.value = nested_tlv.value;
				tlv.length = nested_tlv.length;
				continue;
			}
			else {
				SSL_LOG(TRACE, "context-specific: implicit tag");
				ft_list_append_content(&implicit_tags, tag);
				// We don't know how to decode the value and we have no type metadata.
				{
					t_der_v2_value *dvalue = der_v2_value_create();
					dvalue->kind = ASN_V2_TYPE_KIND_ANY;
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
		}
		default: {
			SSL_LOG(ERROR, "unknown tag class: %d", tlv.tag.class);
			goto label_error;
		}}
	}
	t_der_v2_value *dvalue = *der_value;
	dvalue->explicit_tags.count = explicit_tags.size;
	SSL_ALLOC(dvalue->explicit_tags.items, explicit_tags.size * sizeof(t_asn_v2_tag));
	for (size_t i = 0; i < explicit_tags.size; i++) {
		t_der_v2_tag *tag = ft_list_pop_content(&explicit_tags);
		dvalue->explicit_tags.items[i] = *tag;
		der_v2_tag_delete(tag);
	}
	dvalue->implicit_tags.count = implicit_tags.size;
	SSL_ALLOC(dvalue->implicit_tags.items, implicit_tags.size * sizeof(t_asn_v2_tag));
	for (size_t i = 0; i < implicit_tags.size; i++) {
		t_der_v2_tag *tag = ft_list_pop_content(&implicit_tags);
		dvalue->implicit_tags.items[i] = *tag;
		der_v2_tag_delete(tag);
	}
	return (SSL_OK);

label_error:
	ft_list_clear_all_content(&explicit_tags, __asn1_v2_tag_delete_adapter);
	ft_list_clear_all_content(&implicit_tags, __asn1_v2_tag_delete_adapter);
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
	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_SEQUENCE;
	SSL_ALLOC(dvalue->as.composite.items, element_values.size * sizeof(t_der_v2_value));
	dvalue->as.composite.count = element_values.size;

	t_list_next next = {0};
	void *content = NULL;
	size_t idx = 0;
	while (ft_list_next_content(&element_values, &next, &content)) {
		der_v2_value_copy(content, dvalue->as.composite.items + idx);
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

static int	__der_v2_generic_decode_set(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "set: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	t_der_v2_tlv element_tlv = {0};
	t_list element_values = {0};
	while (__der_v2_next_tlv(tlv.value, tlv.length, &element_tlv)) {
		t_der_v2_value *element_value = NULL;
		if (SSL_OK != __der_v2_generic_decode_value(element_tlv, &element_value)) {
			SSL_LOG(ERROR, "set: bad decode element");
			goto label_error;
		}
		assert(element_value != NULL);
		ft_list_append_content(&element_values, element_value);
	}
	// Compile the list of values.
	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_SET;
	SSL_ALLOC(dvalue->as.composite.items, element_values.size * sizeof(t_der_v2_value));
	dvalue->as.composite.count = element_values.size;

	t_list_next next = {0};
	void *content = NULL;
	size_t idx = 0;
	while (ft_list_next_content(&element_values, &next, &content)) {
		der_v2_value_copy(content, dvalue->as.composite.items + idx);
		idx++;
	}
	ft_list_clear_all_content(&element_values, __der_v2_value_delete_adapter);

	SSL_LOG(TRACE, "set: done");
	*der_value = dvalue;
	return (SSL_OK);

label_error:
	ft_list_clear_all_content(&element_values, __der_v2_value_delete_adapter);
	return (SSL_ERR);
}

static int	__der_v2_generic_decode_ia5_string(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "ia5 string: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	for (size_t i = 0; i < tlv.length; i++) {
		if (!ft_isascii((int)tlv.value[i])) {
			SSL_LOG(ERROR, "ia5 string: bad value");
			return (SSL_ERR);
		}
	}
	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_IA5_STRING;
	char *cstr = NULL;
	SSL_ALLOC(cstr, tlv.length+1);
	ft_memcpy(cstr, tlv.value, tlv.length);
	cstr[tlv.length] = 0;
	dvalue->as.cstring = cstr;

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_printable_string(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "printable string: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	for (size_t i = 0; i < tlv.length; i++) {
		if (!ft_isprint((int)tlv.value[i])) {
			SSL_LOG(ERROR, "printable string: bad value");
			return (SSL_ERR);
		}
	}
	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_PRINTABLE_STRING;
	char *cstr = NULL;
	SSL_ALLOC(cstr, tlv.length+1);
	ft_memcpy(cstr, tlv.value, tlv.length);
	cstr[tlv.length] = 0;
	dvalue->as.cstring = cstr;

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_visible_string(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "visible string: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	for (size_t i = 0; i < tlv.length; i++) {
		if (!ft_isprint((int)tlv.value[i])) {
			SSL_LOG(ERROR, "visible string: bad value");
			return (SSL_ERR);
		}
	}
	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_VISIBLE_STRING;
	char *cstr = NULL;
	SSL_ALLOC(cstr, tlv.length+1);
	ft_memcpy(cstr, tlv.value, tlv.length);
	cstr[tlv.length] = 0;
	dvalue->as.cstring = cstr;

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_utc_time(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	const char *expected_fmt = "YYMMDDhhmmssZ";
	size_t expected_len = ft_strlen(expected_fmt);

	SSL_LOG(TRACE, "utc time: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	if (tlv.length != expected_len || tlv.value[tlv.length-1] != 'Z') goto label_error;

	for (size_t i = 0; i < tlv.length-1; i++) {
		if (!ft_isdigit((int)tlv.value[i])) goto label_error;
	}

	int t[6] = {0};
	char z = 0;
	size_t scanned = 0;
	int matches = textutil_sscanf((const char *)tlv.value, tlv.length, "%2d%2d%2d%2d%2d%2d%c%zn", t, t+1, t+2, t+3, t+4, t+5, &z, &scanned);
	if (matches != 7 || scanned != tlv.length) goto label_error;
	if (t[1] < 1 || t[1] > 12) goto label_error;
	if (t[2] < 1 || t[2] > 31) goto label_error;
	if (t[3] < 0 || t[3] > 23) goto label_error;
	if (t[4] < 0 || t[4] > 59) goto label_error;
	if (t[5] < 0 || t[5] > 59) goto label_error;
	if (z != 'Z') goto label_error;

	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_UTC_TIME;
	char *cstr = NULL;
	SSL_ALLOC(cstr, tlv.length+1);
	ft_memcpy(cstr, tlv.value, tlv.length);
	cstr[tlv.length] = 0;
	dvalue->as.cstring = cstr;

	*der_value = dvalue;
	return (SSL_OK);

label_error:
	SSL_LOG(ERROR, "utc time: bad value");
	return (SSL_ERR);
}

static int	__der_v2_generic_decode_generalized_time(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	const char *expected_fmt = "YYYYMMDDhhmmss[.fraction]Z";
	const size_t min_expected_len = 15;
	UNUSED(expected_fmt);

	const char *expected_prefix_fmt = "YYYYMMDDhhmmss";
	const size_t expected_prefix_len = ft_strlen(expected_prefix_fmt);

	SSL_LOG(TRACE, "generalized time: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	if (tlv.length < min_expected_len || tlv.value[tlv.length-1] != 'Z') goto label_error;

	int t[6] = {0};
	char z = 0;
	bool has_fraction = false;
	int fraction = 0;
	size_t scanned = 0;
	int matches = textutil_sscanf((const char *)tlv.value, tlv.length, "%4d%2d%2d%2d%2d%2d%zn", t, t+1, t+2, t+3, t+4, t+5, &scanned);
	if (matches != 6 || scanned != expected_prefix_len) goto label_error;
	if (t[1] < 1 || t[1] > 12) goto label_error;
	if (t[2] < 1 || t[2] > 31) goto label_error;
	if (t[3] < 0 || t[3] > 23) goto label_error;
	if (t[4] < 0 || t[4] > 59) goto label_error;
	if (t[5] < 0 || t[5] > 59) goto label_error;

	if (scanned < tlv.length && tlv.value[scanned] == '.') {
		has_fraction = true;
		if (scanned+3 > tlv.length) goto label_error;
		size_t scanned_suffix = 0;
		matches = textutil_sscanf((const char *)tlv.value+scanned, tlv.length-scanned, ".%d%c%zn", &fraction, &z, &scanned_suffix);
		if (matches != 2 || scanned + scanned_suffix != tlv.length) goto label_error;
	}
	else {
		if (scanned+1 != tlv.length) goto label_error;
		z = tlv.value[scanned];
	}

	// Zero fraction must be omitted.
	if (has_fraction && fraction == 0) goto label_error;
	if (z != 'Z') goto label_error;

	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_GENERALIZED_TIME;
	char *cstr = NULL;
	SSL_ALLOC(cstr, tlv.length+1);
	ft_memcpy(cstr, tlv.value, tlv.length);
	cstr[tlv.length] = 0;
	dvalue->as.cstring = cstr;

	*der_value = dvalue;
	return (SSL_OK);

label_error:
	SSL_LOG(ERROR, "generalized time: bad value");
	return (SSL_ERR);
}

static int	__der_v2_generic_decode_numeric_string(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "numeric string: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	for (size_t i = 0; i < tlv.length; i++) {
		if (!ft_isdigit((int)tlv.value[i]) && tlv.value[i] != ' ') {
			SSL_LOG(ERROR, "numeric string: bad value");
			return (SSL_ERR);
		}
	}
	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_NUMERIC_STRING;
	char *cstr = NULL;
	SSL_ALLOC(cstr, tlv.length+1);
	ft_memcpy(cstr, tlv.value, tlv.length);
	cstr[tlv.length] = 0;
	dvalue->as.cstring = cstr;

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_octet_string(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "octet string: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_OCTET_STRING;
	if (tlv.length > 0) {
		ft_ostr_init_with_capacity(&dvalue->as.ostring, tlv.length);
		ft_ostr_append(&dvalue->as.ostring, tlv.value, tlv.length);
	} else {
		ft_ostr_init(&dvalue->as.ostring);
	}

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_bit_string(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "bit string: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	if (tlv.length < 1) {
		SSL_LOG(ERROR, "bit string: bad length");
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "bit string: unused bits: %u", tlv.value[0]);
	// TODO: check last value octet: unused octet bits must be unset.
	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_BIT_STRING;
	ft_ostr_init_with_capacity(&dvalue->as.ostring, tlv.length);
	ft_ostr_append(&dvalue->as.ostring, tlv.value, tlv.length);

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_utf8_string(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "utf8s string: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	if (!utf8_is_valid(tlv.value, tlv.length)) {
		SSL_LOG(ERROR, "utf8 string: bad value");
		return (SSL_ERR);
	}

	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_UTF8_STRING;
	if (tlv.length > 0) {
		ft_ostr_init_with_capacity(&dvalue->as.ostring, tlv.length);
		ft_ostr_append(&dvalue->as.ostring, tlv.value, tlv.length);
	} else {
		ft_ostr_init(&dvalue->as.ostring);
	}

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
	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_BOOLEAN;
	dvalue->as.boolean = boolean;

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_null(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "null: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_NULL;

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_integer(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "integer: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_INTEGER;
	bnum_init_from_bytes(&dvalue->as.number, tlv.value, tlv.length);

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_enumerated(t_der_v2_tlv tlv, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "enumerated: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	// We can't tell the name of enum without metadata, so decode value the same way as INTEGER.
	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_ENUMERATED;
	dvalue->as.enumerated.name = NULL;
	bnum_init_from_bytes(&dvalue->as.enumerated.number, tlv.value, tlv.length);
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
	if (num_sub_ids < 2 || num_sub_ids+1 > ASN_V2_OID_MAX_NARCS) {
		SSL_LOG(ERROR, "bad value");
		return (SSL_ERR);
	}

	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_OBJECT_ID;
	t_asn_v2_oid *oid = &dvalue->as.object_id;
	oid->narcs = 0;
	//	First two ids are concatenated into one single id using following formula:
	//	CONCAT_ID = 40 * ID_0 + ID_1
	oid->arc_nums[oid->narcs++] = sub_ids[0] / 40;
	oid->arc_nums[oid->narcs++] = sub_ids[0] % 40;

	// Get the rest of ids, except the last one
	for (size_t i = 1; i < num_sub_ids; i++) {
		oid->arc_nums[oid->narcs++] = sub_ids[i];
	}

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_generic_decode_relative_object_id(t_der_v2_tlv tlv, t_der_v2_value **der_value)
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
		// Get 7-bit blocks, up to the last one.
		while (i < encsize && enc[i] & 0x80) {
			sub_ids[num_sub_ids] <<= 7;
			sub_ids[num_sub_ids] |= enc[i] & 0x7F;
			i++;
		}
		// If we've reached the end of the encoded content, then we've got an invalid der encoding.
		if (i >= encsize) {
			SSL_LOG(ERROR, "bad value");
			return (SSL_ERR);
		}
		// Get the last block.
		sub_ids[num_sub_ids] <<= 7;
		sub_ids[num_sub_ids] |= enc[i] & 0x7F;
		i++;
		num_sub_ids++;
	}
	if (num_sub_ids < 1 || num_sub_ids > ASN_V2_OID_MAX_NARCS) {
		SSL_LOG(ERROR, "bad value");
		return (SSL_ERR);
	}

	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID;
	t_asn_v2_oid *oid = &dvalue->as.object_id;
	oid->narcs = 0;
	for (size_t i = 0; i < num_sub_ids; i++) {
		oid->arc_nums[oid->narcs++] = sub_ids[i];
	}

	*der_value = dvalue;
	return (SSL_OK);
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

typedef int (*t_func_der_v2_decode)(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);

static int __der_v2_decode_value(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int __der_v2_decode_choice(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);

static int	__der_v2_decode_ia5_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_printable_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_numeric_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_visible_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_octet_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_bit_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_utf8_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_utc_time(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_generalized_time(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_boolean(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_sequence(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_set(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_null(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_integer(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_object_id(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_enumerated(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);
static int	__der_v2_decode_relative_object_id(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value);

static struct s_der_v2_decode {
	t_func_der_v2_decode f_decode;
	const char *name;
} DER_V2_DECODE_TYPE_MAP[] = {
	/* ASN_V2_TAG_NUMBER_EOC                = 0 */	{ NULL, "EOC" },
	/* ASN_V2_TAG_NUMBER_BOOLEAN            = 1 */	{ __der_v2_decode_boolean, "BOOLEAN" },
	/* ASN_V2_TAG_NUMBER_INTEGER            = 2 */	{ __der_v2_decode_integer, "INTEGER" },
	/* ASN_V2_TAG_NUMBER_BIT_STRING         = 3 */	{ __der_v2_decode_bit_string, "BIT_STRING" },
	/* ASN_V2_TAG_NUMBER_OCTET_STRING       = 4 */	{ __der_v2_decode_octet_string, "OCTET_STRING" },
	/* ASN_V2_TAG_NUMBER_NULL               = 5 */	{ __der_v2_decode_null, "NULL" },
	/* ASN_V2_TAG_NUMBER_OBJECT_ID          = 6 */	{ __der_v2_decode_object_id, "OBJECT_ID" },
	/* ASN_V2_TAG_NUMBER_OBJECT_DESCR       = 7 */	{ NULL, "OBJECT_DESCR" },
	/* ASN_V2_TAG_NUMBER_EXTERNAL           = 8 */	{ NULL, "EXTERNAL" },
	/* ASN_V2_TAG_NUMBER_REAL               = 9 */	{ NULL, "REAL" },
	/* ASN_V2_TAG_NUMBER_ENUMERATED         = 10 */	{ __der_v2_decode_enumerated, "ENUMERATED" },
	/* ASN_V2_TAG_NUMBER_EMBEDDED_PDV       = 11 */	{ NULL, "EMBEDDED_PDV" },
	/* ASN_V2_TAG_NUMBER_UTF8_STRING        = 12 */	{ __der_v2_decode_utf8_string, "UTF8_STRING" },
	/* ASN_V2_TAG_NUMBER_RELATIVE_OBJECT_ID = 13 */	{ __der_v2_decode_relative_object_id, "RELATIVE_OBJECT_ID" },
	/* ASN_V2_TAG_NUMBER_TIME               = 14 */	{ NULL, "TIME" },
	/* ASN_V2_TAG_NUMBER_RESERVED_15        = 15 */	{ NULL, "RESERVED_15" },
	/* ASN_V2_TAG_NUMBER_SEQUENCE(_OF)      = 16 */	{ __der_v2_decode_sequence, "SEQUENCE" },
	/* ASN_V2_TAG_NUMBER_SET(_OF)           = 17 */	{ __der_v2_decode_set, "SET" },
	/* ASN_V2_TAG_NUMBER_NUMERIC_STRING     = 18 */	{ __der_v2_decode_numeric_string, "NUMERIC_STRING" },
	/* ASN_V2_TAG_NUMBER_PRINTABLE_STRING   = 19 */	{ __der_v2_decode_printable_string, "PRINTABLE_STRING" },
	/* ASN_V2_TAG_NUMBER_TELETEX_STRING     = 20 */	{ NULL, "TELETEX_STRING" },
	/* ASN_V2_TAG_NUMBER_VIDEOTEX_STRING    = 21 */	{ NULL, "VIDEOTEX_STRING" },
	/* ASN_V2_TAG_NUMBER_IA5_STRING         = 22 */	{ __der_v2_decode_ia5_string, "IA5_STRING" },
	/* ASN_V2_TAG_NUMBER_UTC_TIME           = 23 */	{ __der_v2_decode_utc_time, "UTC_TIME" },
	/* ASN_V2_TAG_NUMBER_GENERALIZED_TIME   = 24 */	{ __der_v2_decode_generalized_time, "GENERALIZED_TIME" },
	/* ASN_V2_TAG_NUMBER_GRAPHIC_STRING     = 25 */	{ NULL, "GRAPHIC_STRING" },
	/* ASN_V2_TAG_NUMBER_VISIBLE_STRING     = 26 */	{ __der_v2_decode_visible_string, "VISIBLE_STRING" },
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
		SSL_LOG(ERROR, "bad tlv read");
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
		SSL_LOG(TRACE, "metadata type: kind: %s", asn1_v2_get_type_name(der_type->kind));
		// Use type metadata to validate the TLV tag.
		const t_der_v2_tag *meta_tag = ft_list_pop_ref(&tags);
		if (NULL != meta_tag) {
			SSL_LOG(TRACE, "type metadata: tag={class=%d,number=%d,constructed=%d}", meta_tag->class, meta_tag->number, meta_tag->constructed);
		} else {
			SSL_LOG(ERROR, "type metadata: missing tag");
			goto label_error;
		}
		if (!der_v2_tag_eq(&tlv.tag, meta_tag)) {
			SSL_LOG(ERROR, "type metadata: tag mismatch");
			goto label_error;
		}
		// Decode the value in TLV.
		switch (tlv.tag.class) {
		case ASN_V2_TAG_CLASS_UNIVERSAL: {
			// This must be a standard asn1 type and we should know how to decode the value.
			SSL_LOG(TRACE, "universal: standard asn1 type");
			if (tlv.tag.number >= DER_V2_DECODE_TYPE_COUNT) {
				SSL_LOG(ERROR, "universal: bad tag: unknown tag number: %lu", tlv.tag.number);
				goto label_error;
			}
			t_func_der_v2_decode f_decode = DER_V2_DECODE_TYPE_MAP[tlv.tag.number].f_decode;
			if (NULL == f_decode) {
				SSL_LOG(ERROR, "universal: decoding for `%s` asn1 type is not implemented", asn1_v2_get_type_name(der_type->kind));
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
			SSL_LOG(ERROR, "application: no application scoped asn1 tags currently implemented");
			goto label_error;
		}
		case ASN_V2_TAG_CLASS_CONTEXT_SPECIFIC: {
			if (tlv.tag.constructed) {
				// This is an external tag with nested TLV. Unwrap nested TLV.
				SSL_LOG(TRACE, "context-specific: nested tlv");
				t_der_v2_tlv nested_tlv = {0};
				if (!__der_v2_next_tlv(tlv.value, tlv.length, &nested_tlv)) {
					SSL_LOG(ERROR, "context-specific: bad nested tlv read");
					goto label_error;
				}
				tlv.tag = nested_tlv.tag;
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
			SSL_LOG(ERROR, "private: no private scoped asn1 tags currently implemented");
			goto label_error;
		}
		default: {
			SSL_LOG(ERROR, "unknown tag class: %d", tlv.tag.class);
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
		assert(ASN_V2_TYPE_KIND_ANY != element->type->kind);
		// Element type can be choice as well.
		if (ASN_V2_TYPE_KIND_CHOICE == element->type->kind) {
			if (SSL_OK == __der_v2_decode_choice(tlv, element->type, &chosen_value)) {
				chosen_element = element;
				break;
			}
		}
		// Choice element type must have its own tag unless the element is also a choice.
		if (element->type->tags.size == 0) {
			SSL_LOG(ERROR, "choice: bad type metadata");
			return (SSL_ERR);
		}
		// Compare element type's outermost tag with TLV tag.
		if (der_v2_tag_eq(&tlv.tag, ft_list_last_content(&element->type->tags))) {
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
	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_CHOICE;
	dvalue->as.choice.id = ft_strdup(chosen_element->id);
	dvalue->as.choice.value = chosen_value;

	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_decode_sequence(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "sequence: tlv: tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	t_der_v2_tlv element_tlv = {0};
	t_list element_values = {0};
	t_list_next next = {0};
	void *content = NULL;

	// Decode the element value gracefully handling optional elements and elements with default values.
	// SEQUENCE element TLV order must match the element metadata order.
	while (__der_v2_next_tlv(tlv.value, tlv.length, &element_tlv)) {
		t_der_v2_value *element_value = NULL;
		while (ft_list_next_content(&der_type->as.composite.elements, &next, &content)) {
			t_der_v2_component *element = content;
			SSL_LOG(TRACE, "sequence: element: id=%s", element->id);
			SSL_LOG(TRACE, "sequence: decoding element: tlv: tag={class=%d,number=%d,constructed=%d}", element_tlv.tag.class, element_tlv.tag.number, element_tlv.tag.constructed);
			// If element TLV does not match element type metadata, check if element is optional or has default value.
			if (SSL_OK == __der_v2_decode_value(element_tlv, element->type, &element_value)) {
				SSL_LOG(TRACE, "sequence: decoded value for element: id=%s", element->id);
				ft_list_append_content(&element_values, element_value);
				break;
			}
			else if (NULL != element->default_value) {
				SSL_LOG(TRACE, "sequence: using default value for element: id=%s", element->id);
				element_value = der_v2_value_create();
				der_v2_value_copy(element->default_value, element_value);
				ft_list_append_content(&element_values, element_value);
			}
			else if (element->optional) {
				SSL_LOG(TRACE, "sequence: skipping optional element: id=%s", element->id);
			}
			else {
				SSL_LOG(ERROR, "sequence: bad decode element");
				goto label_error;
			}
		}
		if (element_value == NULL) {
			SSL_LOG(ERROR, "sequence: bad decode");
			goto label_error;
		}
	}
	// If we haven't reach the end of element metadata list we must check if remaining
	// element metadatas are optional or have default values. If not, we must fail SEQUENCE decode.
	while (ft_list_next_content(&der_type->as.composite.elements, &next, &content)) {
		t_der_v2_value *element_value = NULL;
		t_der_v2_component *element = content;
		if (NULL != element->default_value) {
			SSL_LOG(TRACE, "sequence: using default value for element: id=%s", element->id);
			element_value = der_v2_value_create();
			der_v2_value_copy(element->default_value, element_value);
			ft_list_append_content(&element_values, element_value);
		}
		else if (!element->optional) {
			SSL_LOG(ERROR, "sequence: bad decode element");
			goto label_error;
		}
		SSL_LOG(TRACE, "sequence: skipping optional element: id=%s", element->id);
	}
	// Compile the list of values.
	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_SEQUENCE;
	SSL_ALLOC(dvalue->as.composite.items, element_values.size * sizeof(t_der_v2_value));
	dvalue->as.composite.count = element_values.size;
	next = (t_list_next){0};
	size_t idx = 0;
	while (ft_list_next_content(&element_values, &next, &content)) {
		der_v2_value_copy(content, dvalue->as.composite.items + idx);
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

static int	__der_v2_decode_set(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "set: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	t_der_v2_tlv element_tlv = {0};
	t_list element_values = {0};
	t_list_next next = {0};
	void *content = NULL;

	// Create a temporary ref list for element type metadata.
	t_list elements = {0};
	while (ft_list_next_content(&der_type->as.composite.elements, &next, &content)) {
		ft_list_append_ref(&elements, content);
	}
	// Decode the element value gracefully handling optional elements and elements with default values.
	// SET element TLV order may NOT match the element metadata order.
	while (__der_v2_next_tlv(tlv.value, tlv.length, &element_tlv)) {
		t_der_v2_value *element_value = NULL;
		// Take a snapshot of the list size since it may change in the loop.
		size_t list_size = elements.size;
		for (size_t i = 0; i < list_size; i++) {
			// Borrow element metadata from the list.
			t_der_v2_component *element = ft_list_pop_ref(&elements);
			SSL_LOG(TRACE, "set: element: id=%s", element->id);
			SSL_LOG(TRACE, "set: decoding element: tlv: tag={class=%d,number=%d,constructed=%d}", element_tlv.tag.class, element_tlv.tag.number, element_tlv.tag.constructed);
			// Element TLV must match one of element type metadatas.
			if (SSL_OK == __der_v2_decode_value(element_tlv, element->type, &element_value)) {
				// Do NOT return element metadata to the list since it did match element TLV.
				SSL_LOG(TRACE, "set: decoded value for element: id=%s", element->id);
				if (element->id) element_value->as.choice.id = ft_strdup(element->id);
				break;
			}
			else {
				// Return element metadata to the list since it did NOT match element TLV.
				ft_list_append_ref(&elements, element);
			}
		}
		if (element_value == NULL) {
			SSL_LOG(ERROR, "set: bad decode");
			goto label_error;
		}
		ft_list_append_content(&element_values, element_value);
	}
	// If temporary element metadata is not empty we must check if remaining
	// element metadatas are optional or have default values. If not, we must fail SET decode.
	next = (t_list_next){0};
	while (ft_list_next_ref(&elements, &next, &content)) {
		t_der_v2_value *element_value = NULL;
		t_der_v2_component *element = content;
		if (NULL != element->default_value) {
			SSL_LOG(TRACE, "set: using default value for element: id=%s", element->id);
			element_value = der_v2_value_create();
			der_v2_value_copy(element->default_value, element_value);
			ft_list_append_content(&element_values, element_value);
		}
		else if (!element->optional) {
			SSL_LOG(ERROR, "set: bad decode element");
			goto label_error;
		}
		SSL_LOG(TRACE, "set: skipping optional element: id=%s", element->id);
	}
	// Compile the list of values.
	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_SEQUENCE;
	SSL_ALLOC(dvalue->as.composite.items, element_values.size * sizeof(t_der_v2_value));
	dvalue->as.composite.count = element_values.size;
	next = (t_list_next){0};
	size_t idx = 0;
	while (ft_list_next_content(&element_values, &next, &content)) {
		der_v2_value_copy(content, dvalue->as.composite.items + idx);
		idx++;
	}
	ft_list_clear_all_content(&element_values, __der_v2_value_delete_adapter);

	SSL_LOG(TRACE, "set: done");
	*der_value = dvalue;
	return (SSL_OK);

label_error:
	ft_list_clear_all_ref(&elements);
	ft_list_clear_all_content(&element_values, __der_v2_value_delete_adapter);
	return (SSL_ERR);
}

static int	__der_v2_decode_ia5_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_ia5_string(tlv, der_value));
}

static int	__der_v2_decode_printable_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_printable_string(tlv, der_value));
}

static int	__der_v2_decode_numeric_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_numeric_string(tlv, der_value));
}

static int	__der_v2_decode_visible_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_visible_string(tlv, der_value));
}

static int	__der_v2_decode_octet_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_octet_string(tlv, der_value));
}

static int	__der_v2_decode_bit_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_bit_string(tlv, der_value));
}

static int	__der_v2_decode_utf8_string(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_utf8_string(tlv, der_value));
}

static int	__der_v2_decode_boolean(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_boolean(tlv, der_value));
}

static int	__der_v2_decode_null(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_null(tlv, der_value));
}

static int	__der_v2_decode_integer(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_integer(tlv, der_value));
}

static int	__der_v2_decode_enumerated(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	SSL_LOG(TRACE, "enumerated: tlv tag={class=%d,number=%d,constructed=%d}", tlv.tag.class, tlv.tag.number, tlv.tag.constructed);

	t_num num = {0};
	bnum_init_from_bytes(&num, tlv.value, tlv.length);

	const char *name = NULL;
	t_list_next next = {0};
	void *content = NULL;
	while (ft_list_next_content(&der_type->as.enumeration.items, &next, &content)) {
		t_asn_v2_enum *enumerated = content;
		if (bnum_cmp(&enumerated->number, &num) == 0) {
			SSL_LOG(TRACE, "enumerated: name: %s", enumerated->name);
			name = enumerated->name;
			break;
		}
	}
	if (name == NULL && !der_type->as.enumeration.extendable) {
		SSL_LOG(ERROR, "enumerated: bad value");
		bnum_clear(&num);
		return (SSL_ERR);
	}
	t_der_v2_value *dvalue = der_v2_value_create();
	dvalue->kind = ASN_V2_TYPE_KIND_ENUMERATED;
	dvalue->as.enumerated.name = ft_strdup(name);
	bnum_copy(&num, &dvalue->as.enumerated.number);
	bnum_clear(&num);
	*der_value = dvalue;
	return (SSL_OK);
}

static int	__der_v2_decode_object_id(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_object_id(tlv, der_value));
}

static int	__der_v2_decode_relative_object_id(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_relative_object_id(tlv, der_value));
}

static int	__der_v2_decode_utc_time(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_utc_time(tlv, der_value));
}

static int	__der_v2_decode_generalized_time(t_der_v2_tlv tlv, const t_der_v2_type *der_type, t_der_v2_value **der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_decode_generalized_time(tlv, der_value));
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

typedef int (*t_func_der_v2_generic_encode)(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);

static int	__der_v2_generic_encode_value(t_ostring *ostring, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_choice(t_ostring *ostring, const t_der_v2_value *der_value);

static int	__der_v2_generic_encode_sequence(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_set(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_ia5_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_printable_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_numeric_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_visible_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_utc_time(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_generalized_time(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_octet_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_bit_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_utf8_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_boolean(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_null(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_integer(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_enumerated(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_object_id(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);
static int	__der_v2_generic_encode_relative_object_id(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value);

static int __der_v2_write_tlv(t_ostring *ostring, t_der_v2_tlv tlv)
{
	SSL_LOG(TRACE, "writing tlv: tag={class=%d,number=%d,constructed=%d}, length:%zu, content=%p", tlv.tag.class, tlv.tag.number, tlv.tag.constructed, tlv.length, tlv.value);

	// {
	// 	// For debugging.
	// 	const char *type_name = "unknown";
	// 	if (tlv.tag.number < DER_V2_GENERIC_DECODE_TYPE_COUNT) {
	// 		type_name = DER_V2_GENERIC_DECODE_TYPE_MAP[tlv.tag.number].name;
	// 	}
	// 	char *dumps = ft_bytes_dumps_hex(tlv.value, tlv.length);
	// 	SSL_LOG(WARN, "writing %s content octets: %s", type_name, dumps);
	// }

	ssize_t wbytes = 0;
	uint8_t octet = 0;

	// Set tag class bits.
	octet |= (tlv.tag.class & 0x3)<<6;
	// Set constructedness bit.
	octet |= ((uint8_t)tlv.tag.constructed)<<5;

	// Set length bits.
	if (tlv.tag.number <= 30) {
		// Simple tag: tag number fit into last 5 bits of the first octet.
		octet |= (uint8_t)tlv.tag.number;
		ft_ostr_append(ostring, &octet, 1);
		wbytes++;
	}
	else {
		// Complex tag: set all of the last 5 bits of the first octet.
		// The tag number octets must then be written in consecutive octets.
		octet |= 0x1F;
		ft_ostr_append(ostring, &octet, 1);
		wbytes++;
		// Get number of octets required to store tag number bits.
		int nbits = ft_uint_lmbit(tlv.tag.number, 8*sizeof(tlv.tag.number));
		int nbytes = CEIL_DIV(nbits, 7);
		// Assuming this machine uses little-ENDian and since tag number must be big-ENDian, write tag number octets backwards.
		// Every consecutive octet except the last must have most significant bit set.
		uint8_t buf[nbytes];
		uint8_t msbit = 0x0;
		uint32_t tagnum = tlv.tag.number;
		for (int i = nbytes-1; i >= 0; i++) {
			buf[i--] = (0x7F & tagnum) | msbit;
			tagnum >>= 7;
			msbit = 0x80;
		}
		ft_ostr_append(ostring, buf, nbytes);
		wbytes += nbytes;
	}

	// Write length octets.
	if (tlv.length < 128) {
		// Short length form.
		octet = (uint8_t)tlv.length;
		ft_ostr_append(ostring, &octet, 1);
		wbytes++;
	}
	else {
		// SSL_LOG(WARN, "writing long form length: %zu", tlv.length);
		// Long length form.
        // First octet must store number of length octets and its most significant bit must be set.
		int nbits = ft_uint_lmbit(tlv.length, 8 * sizeof(tlv.length));
        int nbytes = CEIL_DIV(nbits, 8);
  		octet = (uint8_t)nbytes | 0x80;
		ft_ostr_append(ostring, &octet, 1);
		wbytes++;
		// Assuming this machine uses little-ENDian and since length must be big-ENDian, write tag number octets backwards.
		uint8_t buf[nbytes];
		size_t len = tlv.length;
		for (int i = nbytes-1; i >= 0; i--) {
			buf[i] = 0xFF & len;
			len >>= 8;
		}
		// {
		// 	char *dumps = ft_bytes_dumps_hex(buf, nbytes);
		// 	SSL_LOG(CRIT, "writing long form length octets: %s", dumps);
		// }
		ft_ostr_append(ostring, buf, nbytes);
		wbytes += nbytes;
	}

	// Write content contents.
	ft_ostr_append(ostring, tlv.value, tlv.length);
	wbytes += tlv.length;
	SSL_LOG(TRACE, "wrote %zu tlv bytes", wbytes);

	// {
	// 	// For debugging.
	// 	const char *type_name = "unknown";
	// 	if (tlv.tag.number < DER_V2_GENERIC_DECODE_TYPE_COUNT) {
	// 		type_name = DER_V2_GENERIC_DECODE_TYPE_MAP[tlv.tag.number].name;
	// 	}
	// 	char *dumps = ft_bytes_dumps_hex((ostring->content+ostring->size)-wbytes, wbytes);
	// 	SSL_LOG(WARN, "wrote %s tlv octets: %s", type_name, dumps);
	// }

	return (SSL_OK);
}

int der_v2_generic_encode(uint8_t **encoded, size_t *encsize, const t_der_v2_value *der_value)
{
	if (NULL == encoded || NULL == encsize || NULL == der_value) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);
	if (SSL_OK != __der_v2_generic_encode_value(&ostring, der_value)) {
		SSL_LOG(ERROR, "bad generic encode");
		return (SSL_ERR);
	}
	ft_ostr_unwrap_content(&ostring, encoded, encsize);
	return (SSL_OK);
}

static int	__der_v2_generic_encode_value(t_ostring *ostring, const t_der_v2_value *der_value)
{
	t_func_der_v2_generic_encode f_encode = NULL;

	switch (der_value->kind) {
	case ASN_V2_TYPE_KIND_NULL: {
		f_encode = __der_v2_generic_encode_null;
		break;
	}
	case ASN_V2_TYPE_KIND_BOOLEAN: {
		f_encode = __der_v2_generic_encode_boolean;
		break;
	}
	case ASN_V2_TYPE_KIND_INTEGER: {
		f_encode = __der_v2_generic_encode_integer;
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		f_encode = __der_v2_generic_encode_enumerated;
		break;
	}
	case ASN_V2_TYPE_KIND_BIT_STRING: {
		f_encode = __der_v2_generic_encode_bit_string;
		break;
	}
	case ASN_V2_TYPE_KIND_OCTET_STRING: {
		f_encode = __der_v2_generic_encode_octet_string;
		break;
	}
	case ASN_V2_TYPE_KIND_IA5_STRING: {
		f_encode = __der_v2_generic_encode_ia5_string;
		break;
	}
	case ASN_V2_TYPE_KIND_UTF8_STRING: {
		f_encode = __der_v2_generic_encode_utf8_string;
		break;
	}
	case ASN_V2_TYPE_KIND_NUMERIC_STRING: {
		f_encode = __der_v2_generic_encode_numeric_string;
		break;
	}
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING: {
		f_encode = __der_v2_generic_encode_printable_string;
		break;
	}
	case ASN_V2_TYPE_KIND_VISIBLE_STRING: {
		f_encode = __der_v2_generic_encode_visible_string;
		break;
	}
	case ASN_V2_TYPE_KIND_UTC_TIME: {
		f_encode = __der_v2_generic_encode_utc_time;
		break;
	}
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME: {
		f_encode = __der_v2_generic_encode_generalized_time;
		break;
	}
	case ASN_V2_TYPE_KIND_OBJECT_ID: {
		f_encode = __der_v2_generic_encode_object_id;
		break;
	}
	case ASN_V2_TYPE_KIND_OBJECT_DESCR: {
		NOT_IMPLEMENTED("ASN_V2_TYPE_KIND_OBJECT_DESCR");
	}
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID: {
		f_encode = __der_v2_generic_encode_relative_object_id;
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE: {
		f_encode = __der_v2_generic_encode_sequence;
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE_OF: {
		f_encode = __der_v2_generic_encode_sequence;
		break;
	}
	case ASN_V2_TYPE_KIND_SET: {
		f_encode = __der_v2_generic_encode_set;
		break;
	}
	case ASN_V2_TYPE_KIND_SET_OF: {
		f_encode = __der_v2_generic_encode_set;
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		SSL_LOG(ERROR, "encoding ANY is not supported");
		return (SSL_ERR);
	}
	case ASN_V2_TYPE_KIND_CHOICE: {
		return (__der_v2_generic_encode_choice(ostring, der_value));
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 type: %d", der_value->kind);
		return (SSL_ERR);
	}}

	uint8_t *encoded = NULL;
	size_t size = 0;
	if (SSL_OK != f_encode(&encoded, &size, der_value)) {
		SSL_LOG(ERROR, "der generic encode error");
		return (SSL_ERR);
	}

	t_ostring tmp = {0};
	t_der_v2_tlv tlv = {0};
	if (der_value->implicit_tags.count > 0) {
		tlv.tag = der_value->implicit_tags.items[0];
	} else {
		tlv.tag = der_v2_get_tag_universal(der_value->kind);
	}
	tlv.length = size;
	tlv.value = encoded;

	if (SSL_OK != __der_v2_write_tlv(&tmp, tlv)) {
		SSL_LOG(ERROR, "der generic encode write error");
		goto label_error;
	}
	ft_ostr_unwrap_content(&tmp, &encoded, &size);

	if (der_value->explicit_tags.count > 0) {
		for (size_t i = 0; i < der_value->explicit_tags.count; i++) {
			// Reverse order: we want the most inner tag first.
			t_der_v2_tag tag = der_value->explicit_tags.items[der_value->explicit_tags.count-1-i];
			tlv = (t_der_v2_tlv){.tag = tag, .length = size, .value = encoded};
			if (SSL_OK != __der_v2_write_tlv(&tmp, tlv)) {
				SSL_LOG(ERROR, "der generic encode write error");
				ft_ostr_clear(&tmp);
				goto label_error;
			}
			SSL_FREE(encoded);
			ft_ostr_unwrap_content(&tmp, &encoded, &size);
		}
	}

	ft_ostr_append(ostring, encoded, size);
	SSL_FREE(encoded);
	return (SSL_OK);

label_error:
	SSL_FREE(encoded);
	return (SSL_ERR);
}

static int	__der_v2_generic_encode_choice(t_ostring *ostring, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "choice: encoding");
	if (SSL_OK != __der_v2_generic_encode_value(ostring, der_value->as.choice.value)) {
		SSL_LOG(ERROR, "choice: alternative encode error");
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__der_v2_generic_encode_sequence(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "sequence: encoding");
	t_ostring ostring = {0};
	for (size_t i = 0; i < der_value->as.composite.count; i++) {
		if (SSL_OK != __der_v2_generic_encode_value(&ostring, der_value->as.composite.items + i)) {
			SSL_LOG(ERROR, "sequence: element encode error");
			return (SSL_ERR);
		}
	}
	ft_ostr_unwrap_content(&ostring, encoded, size);
	return (SSL_OK);
}

static int	__der_v2_generic_encode_set(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "set: encoding");
	t_ostring ostring = {0};
	for (size_t i = 0; i < der_value->as.composite.count; i++) {
		if (SSL_OK != __der_v2_generic_encode_value(&ostring, der_value->as.composite.items + i)) {
			SSL_LOG(ERROR, "set: element encode error");
			return (SSL_ERR);
		}
	}
	ft_ostr_unwrap_content(&ostring, encoded, size);
	return (SSL_OK);
}

static int	__der_v2_generic_encode_ia5_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "ia5 string: encoding");
	const char *s = der_value->as.cstring;
	size_t slen = ft_strlen(s);
	for (size_t i = 0; i < slen; i++) {
		if (!ft_isascii((int)s[i])) {
			SSL_LOG(ERROR, "ia5 string: bad value");
			return (SSL_ERR);
		}
	}
	*encoded = (uint8_t *)ft_strdup(s);
	*size = slen;
	return (SSL_OK);
}

static int	__der_v2_generic_encode_printable_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "printable string: encoding");
	const char *s = der_value->as.cstring;
	size_t slen = ft_strlen(s);
	for (size_t i = 0; i < slen; i++) {
		if (!ft_isprint((int)s[i])) {
			SSL_LOG(ERROR, "printable string: bad value");
			return (SSL_ERR);
		}
	}
	*encoded = (uint8_t *)ft_strdup(s);
	*size = slen;
	return (SSL_OK);
}

static int	__der_v2_generic_encode_numeric_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "numeric string: encoding");
	const char *s = der_value->as.cstring;
	size_t slen = ft_strlen(s);
	for (size_t i = 0; i < slen; i++) {
		if (!ft_isdigit((int)s[i]) && s[i] != ' ') {
			SSL_LOG(ERROR, "numeric string: bad value");
			return (SSL_ERR);
		}
	}
	*encoded = (uint8_t *)ft_strdup(s);
	*size = slen;
	return (SSL_OK);
}

static int	__der_v2_generic_encode_visible_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "visible string: encoding");
	const char *s = der_value->as.cstring;
	size_t slen = ft_strlen(s);
	for (size_t i = 0; i < slen; i++) {
		if (!ft_isprint((int)s[i])) {
			SSL_LOG(ERROR, "visible string: bad value");
			return (SSL_ERR);
		}
	}
	*encoded = (uint8_t *)ft_strdup(s);
	*size = slen;
	return (SSL_OK);
}

static int	__der_v2_generic_encode_utc_time(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	const char *expected_fmt = "YYMMDDhhmmssZ";
	size_t expected_len = ft_strlen(expected_fmt);

	SSL_LOG(TRACE, "utc time: encoding");

	const char *s = der_value->as.cstring;
	size_t slen = ft_strlen(s);

	if (slen != expected_len || s[slen-1] != 'Z') goto label_error;

	for (size_t i = 0; i < slen-1; i++) {
		if (!ft_isdigit((int)s[i])) goto label_error;
	}

	int t[6] = {0};
	char z = 0;
	size_t scanned = 0;
	int matches = textutil_sscanf(s, slen, "%2d%2d%2d%2d%2d%2d%c%zn", t, t+1, t+2, t+3, t+4, t+5, &z, &scanned);
	if (matches != 7 || scanned != slen) goto label_error;
	if (t[1] < 1 || t[1] > 12) goto label_error;
	if (t[2] < 1 || t[2] > 31) goto label_error;
	if (t[3] < 0 || t[3] > 23) goto label_error;
	if (t[4] < 0 || t[4] > 59) goto label_error;
	if (t[5] < 0 || t[5] > 59) goto label_error;
	if (z != 'Z') goto label_error;

	*encoded = (uint8_t *)ft_strdup(s);
	*size = slen;
	return (SSL_OK);

label_error:
	SSL_LOG(ERROR, "utc time: bad value");
	return (SSL_ERR);
}

static int	__der_v2_generic_encode_generalized_time(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	const char *expected_fmt = "YYYYMMDDhhmmss[.fraction]Z";
	const size_t min_expected_len = 15;
	UNUSED(expected_fmt);

	const char *expected_prefix_fmt = "YYYYMMDDhhmmss";
	const size_t expected_prefix_len = ft_strlen(expected_prefix_fmt);

	SSL_LOG(TRACE, "generalized time: encoding");

	const char *s = der_value->as.cstring;
	size_t slen = ft_strlen(s);

	if (slen < min_expected_len || s[slen-1] != 'Z') goto label_error;

	int t[6] = {0};
	char z = 0;
	bool has_fraction = false;
	int fraction = 0;
	size_t scanned = 0;
	int matches = textutil_sscanf(s, slen, "%4d%2d%2d%2d%2d%2d%zn", t, t+1, t+2, t+3, t+4, t+5, &scanned);
	if (matches != 6 || scanned != expected_prefix_len) goto label_error;
	if (t[1] < 1 || t[1] > 12) goto label_error;
	if (t[2] < 1 || t[2] > 31) goto label_error;
	if (t[3] < 0 || t[3] > 23) goto label_error;
	if (t[4] < 0 || t[4] > 59) goto label_error;
	if (t[5] < 0 || t[5] > 59) goto label_error;

	if (scanned < slen && s[scanned] == '.') {
		has_fraction = true;
		if (scanned+3 > slen) goto label_error;
		size_t scanned_suffix = 0;
		matches = textutil_sscanf(s+scanned, slen-scanned, ".%d%c%zn", &fraction, &z, &scanned_suffix);
		if (matches != 2 || scanned + scanned_suffix != slen) goto label_error;
	}
	else {
		if (scanned+1 != slen) goto label_error;
		z = s[scanned];
	}

	// Zero fraction must be omitted.
	if (has_fraction && fraction == 0) goto label_error;
	if (z != 'Z') goto label_error;

	*encoded = (uint8_t *)ft_strdup(s);
	*size = slen;
	return (SSL_OK);

label_error:
	SSL_LOG(ERROR, "generalized time: bad value");
	return (SSL_ERR);
}

static int	__der_v2_generic_encode_octet_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "octet string: encoding");
	*encoded = ft_memdup(der_value->as.ostring.content, der_value->as.ostring.size);
	*size = der_value->as.ostring.size;
	return (SSL_OK);
}

static int	__der_v2_generic_encode_bit_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "bit string: encoding");
	if (der_value->as.ostring.size < 1) {
		SSL_LOG(ERROR, "bit string: bad length");
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "bit string: unused bits: %u", der_value->as.ostring.content[0]);
	*encoded = ft_memdup(der_value->as.ostring.content, der_value->as.ostring.size);
	*size = der_value->as.ostring.size;
	return (SSL_OK);
}

static int	__der_v2_generic_encode_utf8_string(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "utf8s string: encoding");

	if (!utf8_is_valid(der_value->as.ostring.content, der_value->as.ostring.size)) {
		SSL_LOG(ERROR, "utf8 string: bad value");
		return (SSL_ERR);
	}
	*encoded = ft_memdup(der_value->as.ostring.content, der_value->as.ostring.size);
	*size = der_value->as.ostring.size;
	return (SSL_OK);
}

static int	__der_v2_generic_encode_boolean(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "boolean: encoding");

	uint8_t octet = 0x0;
	if (der_value->as.boolean == true) {
		octet = 0xFF;
	}
	*encoded = ft_memdup(&octet, 1);
	*size = 1;
	return (SSL_OK);
}

static int	__der_v2_generic_encode_null(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "null: encoding");
	*encoded = NULL;
	*size = 0;
	return (SSL_OK);
}

static int	__der_v2_generic_encode_integer(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "integer: encoding");
	bnum_to_bytes(&der_value->as.number, encoded, size);
	return (SSL_OK);
}

static int	__der_v2_generic_encode_enumerated(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "enumerated: encoding");
	bnum_to_bytes(&der_value->as.enumerated.number, encoded, size);
	return (SSL_OK);
}

static int	__der_v2_generic_encode_object_id(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "object id: encoding");
	const t_asn_v2_oid *oid = &der_value->as.object_id;
	if (oid->narcs < 2) {
		SSL_LOG(ERROR, "object id: bad value");
		return (SSL_ERR);
	}
	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, oid->narcs * sizeof(*oid->arc_nums));

	size_t num_count = oid->narcs-1;
	size_t nums[num_count];
	nums[0] = 40 * oid->arc_nums[0] + oid->arc_nums[1];

	for (size_t i = 1; i < num_count; i++) {
		nums[i] = oid->arc_nums[i+1];
	}

	for (int i = 0; i < num_count; i++) {
		uint32_t num = nums[i];
		int nbits = ft_uint_lmbit(num, 8 * sizeof(num));
		int nbytes = NBITS_TO_NWORDS(nbits, 7);

		uint8_t num_enc[nbytes];
		ssize_t idx = nbytes;

		num_enc[--idx] = 0x7F & num;
		num >>= 7;
		while (num != 0) {
			num_enc[--idx] = (0x7F & num) | 0x80;
			num >>= 7;
		}
		ft_ostr_append(&ostring, num_enc, nbytes);
	}
	ft_ostr_unwrap_content(&ostring, encoded, size);
	return (SSL_OK);
}

static int	__der_v2_generic_encode_relative_object_id(uint8_t **encoded, size_t *size, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "relative object id: encoding");
	const t_asn_v2_oid *oid = &der_value->as.object_id;
	if (oid->narcs < 1) {
		SSL_LOG(ERROR, "relatobject id: bad value");
		return (SSL_ERR);
	}
	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, oid->narcs * sizeof(*oid->arc_nums));

	size_t num_count = oid->narcs;
	const size_t *nums = oid->arc_nums;
	for (int i = 0; i < num_count; i++) {
		uint32_t num = nums[i];
		int nbits = ft_uint_lmbit(num, 8 * sizeof(num));
		int nbytes = NBITS_TO_NWORDS(nbits, 7);

		uint8_t num_enc[nbytes];
		ssize_t idx = nbytes;

		num_enc[--idx] = 0x7F & num;
		num >>= 7;
		while (num != 0) {
			num_enc[--idx] = (0x7F & num) | 0x80;
			num >>= 7;
		}
		ft_ostr_append(&ostring, num_enc, nbytes);
	}
	ft_ostr_unwrap_content(&ostring, encoded, size);
	return (SSL_OK);
}

/****************************************************************************/

typedef int (*t_func_der_v2_encode)(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);

static int	__der_v2_encode_value(t_ostring *ostring, const t_der_v2_type *der_type, const t_der_v2_value *der_value);

static int	__der_v2_encode_choice(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_sequence(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_set(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_ia5_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_printable_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_numeric_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_visible_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_utc_time(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_generalized_time(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_octet_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_bit_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_utf8_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_boolean(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_null(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_integer(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_enumerated(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_object_id(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_relative_object_id(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);
static int	__der_v2_encode_any(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value);

int der_v2_encode(uint8_t **encoded, size_t *encsize, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	if (NULL == encoded || NULL == encsize || NULL == der_type || NULL == der_value) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	t_ostring ostring = {0};
	ft_ostr_init_with_capacity(&ostring, 1024);
	if (SSL_OK != __der_v2_encode_value(&ostring, der_type, der_value)) {
		SSL_LOG(ERROR, "bad encode");
		return (SSL_ERR);
	}
	ft_ostr_unwrap_content(&ostring, encoded, encsize);
	return (SSL_OK);
}

static int	__der_v2_encode_value(t_ostring *ostring, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	if (der_type->kind != der_value->kind && der_type->kind != ASN_V2_TYPE_KIND_ANY) {
		SSL_LOG(ERROR, "bad value: type metadata mismatch");
		return (SSL_ERR);
	}
	// Get a list of tag references from metadata in reverse order: outermost to innermost tag.
	t_list tags = {0};
	{
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&der_type->tags, &next, &content)) {
			ft_list_prepend_ref(&tags, content);
		}
	}
	{
		t_list_next next = {0};
		void *content = NULL;
		for (size_t i = 0; i < der_value->implicit_tags.count; i++) {
			if (!ft_list_next_content(&tags, &next, &content)) {
				SSL_LOG(ERROR, "bad value: type metadata mismatch");
				return (SSL_ERR);
			}
			t_asn_v2_tag *atag = content;
			t_der_v2_tag *dtag = der_value->implicit_tags.items + i;
			if (atag->class != dtag->class || atag->number != dtag->number) {
				SSL_LOG(ERROR, "bad value: type metadata mismatch");
				return (SSL_ERR);
			}
		}
		for (size_t i = 0; i < der_value->implicit_tags.count; i++) {
			if (!ft_list_next_content(&tags, &next, &content)) {
				SSL_LOG(ERROR, "bad value: type metadata mismatch");
				return (SSL_ERR);
			}
			t_asn_v2_tag *atag = content;
			t_der_v2_tag *dtag = der_value->implicit_tags.items + i;
			if (atag->class != dtag->class || atag->number != dtag->number) {
				SSL_LOG(ERROR, "bad value: type metadata mismatch");
				return (SSL_ERR);
			}
		}
	}

	t_func_der_v2_encode f_encode = NULL;

	switch (der_value->kind) {
	case ASN_V2_TYPE_KIND_NULL: {
		f_encode = __der_v2_encode_null;
		break;
	}
	case ASN_V2_TYPE_KIND_BOOLEAN: {
		f_encode = __der_v2_encode_boolean;
		break;
	}
	case ASN_V2_TYPE_KIND_INTEGER: {
		f_encode = __der_v2_encode_integer;
		break;
	}
	case ASN_V2_TYPE_KIND_ENUMERATED: {
		f_encode = __der_v2_encode_enumerated;
		break;
	}
	case ASN_V2_TYPE_KIND_BIT_STRING: {
		f_encode = __der_v2_encode_bit_string;
		break;
	}
	case ASN_V2_TYPE_KIND_OCTET_STRING: {
		f_encode = __der_v2_encode_octet_string;
		break;
	}
	case ASN_V2_TYPE_KIND_IA5_STRING: {
		f_encode = __der_v2_encode_ia5_string;
		break;
	}
	case ASN_V2_TYPE_KIND_UTF8_STRING: {
		f_encode = __der_v2_encode_utf8_string;
		break;
	}
	case ASN_V2_TYPE_KIND_NUMERIC_STRING: {
		f_encode = __der_v2_encode_numeric_string;
		break;
	}
	case ASN_V2_TYPE_KIND_PRINTABLE_STRING: {
		f_encode = __der_v2_encode_printable_string;
		break;
	}
	case ASN_V2_TYPE_KIND_VISIBLE_STRING: {
		f_encode = __der_v2_encode_visible_string;
		break;
	}
	case ASN_V2_TYPE_KIND_UTC_TIME: {
		f_encode = __der_v2_encode_utc_time;
		break;
	}
	case ASN_V2_TYPE_KIND_GENERALIZED_TIME: {
		f_encode = __der_v2_encode_generalized_time;
		break;
	}
	case ASN_V2_TYPE_KIND_OBJECT_ID: {
		f_encode = __der_v2_encode_object_id;
		break;
	}
	case ASN_V2_TYPE_KIND_OBJECT_DESCR: {
		NOT_IMPLEMENTED("ASN_V2_TYPE_KIND_OBJECT_DESCR");
	}
	case ASN_V2_TYPE_KIND_RELATIVE_OBJECT_ID: {
		f_encode = __der_v2_encode_relative_object_id;
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE: {
		f_encode = __der_v2_encode_sequence;
		break;
	}
	case ASN_V2_TYPE_KIND_SEQUENCE_OF: {
		f_encode = __der_v2_encode_sequence;
		break;
	}
	case ASN_V2_TYPE_KIND_SET: {
		f_encode = __der_v2_encode_set;
		break;
	}
	case ASN_V2_TYPE_KIND_SET_OF: {
		f_encode = __der_v2_encode_set;
		break;
	}
	case ASN_V2_TYPE_KIND_ANY: {
		SSL_LOG(ERROR, "encoding ANY is not supported");
		return (SSL_ERR);
	}
	case ASN_V2_TYPE_KIND_CHOICE: {
		f_encode = __der_v2_encode_choice;
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 type: %d", der_value->kind);
		return (SSL_ERR);
	}}

	uint8_t *encoded = NULL;
	size_t size = 0;
	if (SSL_OK != f_encode(&encoded, &size, der_type, der_value)) {
		SSL_LOG(ERROR, "der encode error");
		return (SSL_ERR);
	}

	t_ostring tmp = {0};
	t_der_v2_tlv tlv = {0};

	// Some asn1 types must not have implicit tags.
	if (der_value->kind == ASN_V2_TYPE_KIND_CHOICE || der_value->kind == ASN_V2_TYPE_KIND_ANY) {
		if (der_value->implicit_tags.count > 0) {
			SSL_LOG(ERROR, "der encode error: bad type metadata");
			goto label_error;
		}
	}
	// Encode inner TLV.
	if (der_value->kind != ASN_V2_TYPE_KIND_CHOICE && der_value->kind != ASN_V2_TYPE_KIND_ANY) {
		if (der_value->implicit_tags.count > 0) {
			tlv.tag = der_value->implicit_tags.items[0];
		} else {
			tlv.tag = der_v2_get_tag_universal(der_value->kind);
		}
		tlv.length = size;
		tlv.value = encoded;

		if (SSL_OK != __der_v2_write_tlv(&tmp, tlv)) {
			SSL_LOG(ERROR, "der encode write error");
			goto label_error;
		}
		ft_ostr_unwrap_content(&tmp, &encoded, &size);
	}
	// Encode outer TLVs.
	if (der_value->explicit_tags.count > 0) {
		for (size_t i = 0; i < der_value->explicit_tags.count; i++) {
			// Reverse order: we want the most inner tag first.
			t_der_v2_tag tag = der_value->explicit_tags.items[der_value->explicit_tags.count-1-i];
			tlv = (t_der_v2_tlv){.tag = tag, .length = size, .value = encoded};
			if (SSL_OK != __der_v2_write_tlv(&tmp, tlv)) {
				SSL_LOG(ERROR, "der encode write error");
				ft_ostr_clear(&tmp);
				goto label_error;
			}
			SSL_FREE(encoded);
			ft_ostr_unwrap_content(&tmp, &encoded, &size);
		}
	}

	ft_ostr_append(ostring, encoded, size);
	SSL_FREE(encoded);
	return (SSL_OK);

label_error:
	SSL_FREE(encoded);
	return (SSL_ERR);
}

static int	__der_v2_encode_choice(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "choice: encoding");

	t_der_v2_value *chosen_value = NULL;
	t_der_v2_component *chosen_element = NULL;
	t_list_next next = {0};
	void *content = NULL;
	while (ft_list_next_content(&der_type->as.composite.elements, &next, &content)) {
		t_der_v2_component *element = content;
		SSL_LOG(TRACE, "choice: checking element with id: %s", element->id);
		// TODO: choice element type cannot be ANY, ensure type compilation handles this before decoding.
		assert(ASN_V2_TYPE_KIND_ANY != element->type->kind);

		if (ft_streq(element->id, der_value->as.choice.id)) {
			chosen_element = element;
			chosen_value = der_value->as.choice.value;
			break;
		}
	}
	if (NULL == chosen_element || NULL == chosen_value) {
		SSL_LOG(ERROR, "choice: alternative type metadata mismatch");
		return (SSL_ERR);
	}

	t_ostring tmp = {0};
	if (SSL_OK != __der_v2_encode_value(&tmp, chosen_element->type, chosen_value)) {
		SSL_LOG(ERROR, "choice: alternative encode error");
		return (SSL_ERR);
	}
	ft_ostr_unwrap_content(&tmp, encoded, size);

	return (SSL_OK);
}

static int	__der_v2_encode_sequence(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "sequence: encoding");

	t_ostring ostring = {0};
	t_list_next next = {0};
	void *content = NULL;
	for (size_t i = 0; i < der_value->as.composite.count; i++) {
		t_der_v2_value *element_value = der_value->as.composite.items + i;
		bool ok = false;
		while (ft_list_next_content(&der_type->as.composite.elements, &next, &content)) {
			t_der_v2_component *element = content;
			SSL_LOG(TRACE, "sequence: element: id=%s", element->id);
			// If element value does not match element type metadata, check if element is optional or has default value.
			t_ostring tmp = {0};
			if (SSL_OK == __der_v2_encode_value(&tmp, element->type, element_value)) {
				ft_ostr_append_ostr(&ostring, &tmp);
				ok = true;
				// TODO: if default element value equal to provided element value, do not encode the element value.
				break;
			}
			else if (NULL != element->default_value) {
				SSL_LOG(TRACE, "sequence: using default value for element: id=%s", element->id);
				if (SSL_OK != __der_v2_encode_value(&tmp, element->type, element->default_value)) {
					SSL_LOG(ERROR, "sequence: bad encode element");
					goto label_error;
				}
				ft_ostr_append_ostr(&ostring, &tmp);
				ok = true;
			}
			else if (element->optional) {
				SSL_LOG(TRACE, "sequence: skipping optional element: id=%s", element->id);
				ok = true;
			}
			else {
				SSL_LOG(ERROR, "sequence: bad encode element");
				goto label_error;
			}
			ft_ostr_clear(&tmp);
		}
		if (!ok) {
			SSL_LOG(ERROR, "sequence: bad encode");
			goto label_error;
		}
	}

	// If we haven't reach the end of element metadata list we must check if remaining
	// element metadatas are optional or have default values. If not, we must fail SEQUENCE encode.
	while (ft_list_next_content(&der_type->as.composite.elements, &next, &content)) {
		t_der_v2_component *element = content;
		if (NULL != element->default_value) {
			SSL_LOG(TRACE, "sequence: using default value for element: id=%s", element->id);
			if (SSL_OK != __der_v2_encode_value(&ostring, element->type, element->default_value)) {
				SSL_LOG(ERROR, "sequence: bad encode element");
				goto label_error;
			}
		}
		else if (!element->optional) {
			SSL_LOG(ERROR, "sequence: bad encode element");
			goto label_error;
		}
		SSL_LOG(TRACE, "sequence: skipping optional element: id=%s", element->id);
	}
	ft_ostr_unwrap_content(&ostring, encoded, size);
	SSL_LOG(TRACE, "sequence: done");
	return (SSL_OK);

label_error:
	ft_ostr_clear(&ostring);
	return (SSL_ERR);
}

static int	__der_v2_encode_set(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "set: encoding");

	t_ostring ostring = {0};

	// Create a temporary ref list for element type metadata.
	t_list_next next = {0};
	void *content = NULL;
	t_list elements = {0};
	while (ft_list_next_content(&der_type->as.composite.elements, &next, &content)) {
		ft_list_append_ref(&elements, content);
	}
	// Decode the element value gracefully handling optional elements and elements with default values.
	// SET element TLV order may NOT match the element metadata order.
	for (size_t i = 0; i < der_value->as.composite.count; i++) {
		t_der_v2_value *element_value = der_value->as.composite.items + i;
		// Take a snapshot of the list size since it may change in the loop.
		size_t list_size = elements.size;
		t_ostring tmp = {0};
		bool ok = false;
		for (size_t i = 0; i < list_size; i++) {
			// Borrow element metadata from the list.
			t_der_v2_component *element = ft_list_pop_ref(&elements);
			SSL_LOG(TRACE, "set: element: id=%s", element->id);
			// Element value must match one of element type metadatas.
			if (SSL_OK == __der_v2_encode_value(&tmp, element->type, element_value)) {
				SSL_LOG(TRACE, "set: encoded value for element: id=%s", element->id);
				ok = true;
				// TODO: if default element value equal to provided element value, do not encode the element value.
				break;
			}
			else {
				// Return element metadata to the list since it did NOT match element TLV.
				ft_list_append_ref(&elements, element);
			}
		}
		if (!ok) {
			SSL_LOG(ERROR, "set: bad encode");
			goto label_error;
		}
		ft_ostr_append_ostr(&ostring, &tmp);
		ft_ostr_clear(&tmp);
	}
	// If temporary element metadata is not empty we must check if remaining
	// element metadatas are optional or have default values. If not, we must fail SET decode.
	next = (t_list_next){0};
	while (ft_list_next_ref(&elements, &next, &content)) {
		t_der_v2_component *element = content;
		if (NULL != element->default_value) {
			SSL_LOG(TRACE, "set: using default value for element: id=%s", element->id);
			if (SSL_OK != __der_v2_encode_value(&ostring, element->type, element->default_value)) {
				SSL_LOG(ERROR, "set: bad encode element");
				goto label_error;
			}
		}
		else if (!element->optional) {
			SSL_LOG(ERROR, "set: bad encode element");
			goto label_error;
		}
		SSL_LOG(TRACE, "set: skipping optional element: id=%s", element->id);
	}
	ft_ostr_unwrap_content(&ostring, encoded, size);
	SSL_LOG(TRACE, "set: done");
	return (SSL_OK);

label_error:
	ft_list_clear_all_ref(&elements);
	ft_ostr_clear(&ostring);
	return (SSL_ERR);
}

static int	__der_v2_encode_ia5_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_ia5_string(encoded, size, der_value));
}

static int	__der_v2_encode_printable_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_printable_string(encoded, size, der_value));
}

static int	__der_v2_encode_numeric_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_numeric_string(encoded, size, der_value));
}

static int	__der_v2_encode_visible_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_visible_string(encoded, size, der_value));
}

static int	__der_v2_encode_utc_time(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_utc_time(encoded, size, der_value));
}

static int	__der_v2_encode_generalized_time(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_generalized_time(encoded, size, der_value));
}

static int	__der_v2_encode_octet_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_octet_string(encoded, size, der_value));
}

static int	__der_v2_encode_bit_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_bit_string(encoded, size, der_value));
}

static int	__der_v2_encode_utf8_string(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_utf8_string(encoded, size, der_value));
}

static int	__der_v2_encode_boolean(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_boolean(encoded, size, der_value));
}

static int	__der_v2_encode_null(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_null(encoded, size, der_value));
}

static int	__der_v2_encode_integer(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_integer(encoded, size, der_value));
}

static int	__der_v2_encode_enumerated(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	SSL_LOG(TRACE, "enumerated: encoding");
	t_list_next next = {0};
	void *content = NULL;
	while (ft_list_next_content(&der_type->as.enumeration.items, &next, &content)) {
		t_asn_v2_enum *denum = content;
		if (bnum_cmp(&denum->number, &der_value->as.enumerated.number) == 0) {
			if (denum->name) SSL_LOG(TRACE, "enumerated: encoding enum: %s", denum->name);
			bnum_to_bytes(&der_value->as.enumerated.number, encoded, size);
			return (SSL_OK);
		}
	}
	return (SSL_ERR);
}

static int	__der_v2_encode_object_id(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_object_id(encoded, size, der_value));
}

static int	__der_v2_encode_relative_object_id(uint8_t **encoded, size_t *size, const t_der_v2_type *der_type, const t_der_v2_value *der_value)
{
	UNUSED(der_type);
	return (__der_v2_generic_encode_relative_object_id(encoded, size, der_value));
}
