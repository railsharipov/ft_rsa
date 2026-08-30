#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <bnum.h>
#include <der.h>
#include <io.h>
#include <libft.h>

static int	__encode(t_node *node, t_io_v2_stream *out);

static int	__encode_ostring(uint8_t tag, t_ostring *encoded, t_ostring *data);
static int	__encode_bitstring(uint8_t tag, t_ostring *encoded, t_ostring *data);
static int	__encode_bool(uint8_t tag, t_ostring *encoded, t_ostring *data);
static int	__encode_sequence(uint8_t tag, t_ostring *encoded, t_ostring *data);
static int	__encode_null(uint8_t tag, t_ostring *encoded, t_ostring *data);
static int	__encode_int(uint8_t tag, t_ostring *encoded, t_ostring *data);
static int	__encode_oid(uint8_t tag, t_ostring *encoded, t_ostring *data);

/* DER write functions */
static ssize_t	__write_content_octets(char *content, size_t size, t_io_v2_stream *out);
static ssize_t	__write_tag(uint8_t tag, uint32_t tagnum, t_io_v2_stream *out);
static ssize_t	__write_len(size_t len, t_io_v2_stream *out);

typedef int	(*t_func_der_encode)(uint8_t tag, t_ostring *encoded, t_ostring *data);

int	der_encode(t_node *asn1_node, t_ostring *encoded)
{
	SSL_LOG(TRACE, "starting DER encode octet string");

	if (NULL == asn1_node || NULL == encoded) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	t_io_v2_stream *out = NULL;
	ft_ostr_init(encoded);

	if (SSL_OK != io_v2_bytes_writer(&out, encoded)) {
		SSL_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != der_encode_stream(asn1_node, out)) {
		SSL_LOG(ERROR, "DER encode octet string failed");
		return (SSL_ERR);
	}
	if (SSL_OK != io_v2_close(out)) {
		SSL_LOG(ERROR, IO_CLOSE_ERROR);
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "DER encode octet string completed successfully");

	return (SSL_OK);
}

int	der_encode_stream(t_node *asn1_node, t_io_v2_stream *out)
{
	SSL_LOG(TRACE, "starting DER encode stream");

	if (NULL == asn1_node || NULL == out) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	int ret = __encode(asn1_node, out);

	if (SSL_OK != ret) {
		SSL_LOG(ERROR, "DER encode stream failed");
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "DER encode stream completed successfully");

	return (SSL_OK);
}

static int	__encode(t_node *asn1_node, t_io_v2_stream *out)
{
	if (NULL == asn1_node) {
		SSL_LOG(TRACE, "encoding done");
		return (SSL_OK);
	}

	t_iasn *asn1_item = asn1_node->content;
	if (NULL == asn1_item) {
		SSL_LOG(ERROR, "invalid asn node: no content");
		return (SSL_ERR);
	}
	uint32_t tagnum = asn1_item->tagnum;

	SSL_LOG(TRACE, "encoding asn item: %s, tag number: %#x", asn1_item_get_type_name(asn1_item), tagnum);

	t_func_der_encode f_encode = NULL;
	uint8_t tag = 0;
	switch (tagnum) {
		case ASN_TAGNUM_SEQUENCE:
			f_encode = __encode_sequence;
			tag = ASN_ENCODE_CONSTRUCT;
			break;
		case ASN_TAGNUM_BOOLEAN:
			f_encode = __encode_bool;
			tag = ASN_ENCODE_PRIMITIVE;
			break;
		case ASN_TAGNUM_OBJECT_ID:
			f_encode = __encode_oid;
			tag = ASN_ENCODE_PRIMITIVE;
			break;
		case ASN_TAGNUM_INT:
			f_encode = __encode_int;
			tag = ASN_ENCODE_PRIMITIVE;
			break;
		case ASN_TAGNUM_OCTET_STRING:
			f_encode = __encode_ostring;
			tag = ASN_ENCODE_PRIMITIVE;
			break;
		case ASN_TAGNUM_BIT_STRING:
			f_encode = __encode_bitstring;
			tag = ASN_ENCODE_PRIMITIVE;
			break;
		case ASN_TAGNUM_NULL:
			f_encode = __encode_null;
			tag = ASN_ENCODE_PRIMITIVE;
			break;
		default:
			SSL_LOG(ERROR, "unknown tag number: %#x", tagnum);
			return (SSL_ERR);
	}

	SSL_LOG(TRACE, "writing der tag: %#x", tagnum);
	if (__write_tag(tag, tagnum, out) < 0) {
		SSL_LOG(ERROR, "failed to write der tag");
		return (SSL_ERR);
	}

	SSL_LOG(TRACE, "encoding content: %p, size: %zu", asn1_item->content, asn1_item->size);
	t_ostring data, encoded;
	ft_ostr_init_with_content(&data, asn1_item->content, asn1_item->size);
	ft_ostr_init(&encoded);

	if (SSL_OK != f_encode(tag, &encoded, &data)) {
		SSL_LOG(TRACE, "encode function failed for tag: %u", tagnum);
		return (SSL_ERR);
	}
	ssize_t wbytes = __write_content_octets((char *)encoded.content, encoded.size, out);
	ft_ostr_clear(&encoded);

	if (wbytes < 0) {
		SSL_LOG(ERROR, "der content write error");
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "node encoded successfully for tag: %u", tagnum);

	return (SSL_OK);
}

static ssize_t	__write_content_octets(char *content, size_t size, t_io_v2_stream *out)
{
	SSL_LOG(TRACE, "writing content octets, size: %zu", size);

	if (__write_len(size, out) < 0) {
		SSL_LOG(ERROR, "failed to write length octets");
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "length octets written");

	if (size > 0) {
		ssize_t	wbytes = io_v2_write(out, content, size);
		if (wbytes < 0) {
			SSL_LOG(ERROR, "failed to write content octets");
			return (-1);
		}
		SSL_LOG(TRACE, "content octets written, bytes: %zd", wbytes);
		return (wbytes);
	}
	else {
		SSL_LOG(TRACE, "no content octets to write");
		return (0);
	}
}

static ssize_t	__write_tag(uint8_t tag, uint32_t tagnum, t_io_v2_stream *out)
{
	SSL_LOG(TRACE, "writing tag: %#x, tag flags: %#x", tagnum, tag);

	ssize_t	wbytes = 0;

	if (tagnum > 30) {
		// Complex tag
		int tagnum_nbits = ft_uint_lmbit(tagnum, 8 * sizeof(tagnum));
        // Number of 7-bit base-128 continuation octets required
        int tagnum_nbytes = CEIL_TO_MULTIPLE(tagnum_nbits, 7) / 7;

		// Additional octet for tag flags
		uint8_t *buf = NULL;
		int buf_size = 1 + tagnum_nbytes;
		SSL_ALLOC(buf, buf_size);

		// Since tag number is in big-ENDian, write tag number octets to the buffer backwards
		int idx = buf_size - 1;
		if (idx > 0) {
			buf[idx--] = 0x7F & tagnum;
			tagnum >>= 7;
		}
		while (idx > 0) {
			buf[idx--] = (0x7F & tagnum) | 0x80;
			tagnum >>= 7;
		}
		buf[0] = ASN_TAGNUM_COMPLEX | tag;

		wbytes = io_v2_write(out, buf, buf_size);
		SSL_FREE(buf);
		if (wbytes < 0) {
			SSL_LOG(ERROR, IO_WRITE_ERROR);
			return (-1);
		}
		SSL_LOG(TRACE, "complex tag written, bytes: %zd", wbytes);
	}
	else {
		// Simple tag
		size_t buf_size = 1;
		uint8_t buf[buf_size];

		buf[0] = ASN_TAGNUM_SIMPLE | tag;
		buf[0] |= tagnum & 0x7F;

		wbytes = io_v2_write(out, (char *)buf, buf_size);
		if (wbytes < 0) {
			SSL_LOG(ERROR, IO_WRITE_ERROR);
			return (-1);
		}
		SSL_LOG(TRACE, "simple tag written, bytes: %zd", wbytes);
	}
	return (wbytes);
}

static ssize_t	__write_len(size_t len, t_io_v2_stream *out)
{
	SSL_LOG(TRACE, "writing length: %zu", len);

	ssize_t	wbytes = 0;

	if (len > 127) {
		// Long length form
		int len_nbits = ft_uint_lmbit(len, 8 * sizeof(len));
        // Number of bytes needed to represent "len" in big-endian
        int len_nbytes = CEIL_TO_MULTIPLE(len_nbits, 8) / 8;
		SSL_LOG(TRACE, "number of length bytes: %d", len_nbytes);

		// Additional octet for length flags
		uint8_t *buf = NULL;
		int buf_size = 1 + len_nbytes;
		SSL_ALLOC(buf, buf_size);

		// Since length is in big-ENDian, write length octets to the buffer backwards
		int idx = buf_size - 1;
		if (idx > 0) {
			buf[idx--] = 0xFF & len;
			len >>= 8;
		}
		while (idx > 0) {
			buf[idx--] = 0xFF & len;
			len >>= 8;
		}
		buf[0] = ASN_LEN_LONG | len_nbytes;

		wbytes = io_v2_write(out, buf, (size_t)buf_size);
		if (wbytes < 0) {
			SSL_LOG(ERROR, IO_WRITE_ERROR);
			return (-1);
		}
		SSL_FREE(buf);
		SSL_LOG(TRACE, "long length form written, bytes: %zd", wbytes);
	}
	else {
		// Short length form
		size_t buf_size = 1;
		uint8_t buf[buf_size];

		buf[0] = len;

		wbytes = io_v2_write(out, buf, buf_size);
		if (wbytes < 0) {
			SSL_LOG(ERROR, IO_WRITE_ERROR);
			return (-1);
		}
		SSL_LOG(TRACE, "short length form written, bytes: %zd", wbytes);
	}
	return (wbytes);
}

static int	__encode_ostring(uint8_t tag, t_ostring *encoded, t_ostring *data)
{
	SSL_LOG(TRACE, "encoding octet string, size: %zu", data->size);

	if (!SSL_FLAG(ASN_ENCODE_PRIMITIVE, tag)) {
		SSL_LOG(ERROR, "invalid asn type tag: expected primitive");
		return (SSL_ERR);
	}
	ft_ostr_append_ostr(encoded, data);

	SSL_LOG(TRACE, "octet string encoded successfully");
	return (SSL_OK);
}

static int	__encode_bitstring(uint8_t tag, t_ostring *encoded, t_ostring *data)
{
	SSL_LOG(TRACE, "encoding bit string, size: %zu", data->size);

	if (!SSL_FLAG(ASN_ENCODE_PRIMITIVE, tag)) {
		SSL_LOG(ERROR, "invalid asn type tag: expected primitive");
		return (SSL_ERR);
	}

	// First content octet SHALL be an extra octet that contains the number of unused trailing bits
	uint8_t num_unused_bits = ((uint8_t *)data->content)[0];

	if (num_unused_bits > 7u) {
		SSL_LOG(ERROR, "invalid bitstring: bad first content octet");
		return (SSL_ERR);
	}
	ft_ostr_append_ostr(encoded, data);

	SSL_LOG(TRACE, "bit string encoded successfully");
	return (SSL_OK);
}

static int	__encode_bool(uint8_t tag, t_ostring *encoded, t_ostring *data)
{
	SSL_LOG(TRACE, "encoding boolean, size: %zu", data->size);

	if (!SSL_FLAG(ASN_ENCODE_PRIMITIVE, tag)) {
		SSL_LOG(ERROR, "invalid asn type tag: expected primitive");
		return (SSL_ERR);
	}
	if (data->size != 1) {
		SSL_LOG(ERROR, "invalid boolean type: bad content size");
		return (SSL_ERR);
	}
	ft_ostr_append_ostr(encoded, data);

	SSL_LOG(TRACE, "boolean encoded successfully");
	return (SSL_OK);
}

static int	__encode_sequence(uint8_t tag, t_ostring *encoded, t_ostring *data)
{
	SSL_LOG(TRACE, "encoding sequence, size: %zu", data->size);

	if (!SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		SSL_LOG(ERROR, "invalid asn type tag: expected construct");
		return (SSL_ERR);
	}

	t_node *asn1_node = (t_node *)data->content;
	if (NULL == asn1_node) {
		SSL_LOG(ERROR, "invalid asn sequence: bad content");
		return (SSL_ERR);
	}

	t_io_v2_stream *out = NULL;
	if (SSL_OK != io_v2_bytes_writer(&out, encoded)) {
		SSL_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}

	while (asn1_node) {
		if (SSL_OK != __encode(asn1_node, out)) {
			SSL_LOG(TRACE, "sequence child encoding failed");
			return (SSL_ERR);
		}
		asn1_node = asn1_node->next;
	}

	if (SSL_OK != io_v2_close(out)) {
		SSL_LOG(ERROR, IO_CLOSE_ERROR);
		return (SSL_ERR);
	}

	SSL_LOG(TRACE, "sequence encoded successfully");
	return (SSL_OK);
}

static int	__encode_null(uint8_t tag, t_ostring *encoded, t_ostring *data)
{
	SSL_LOG(TRACE, "encoding null, size: %zu", data->size);

	if (!SSL_FLAG(ASN_ENCODE_PRIMITIVE, tag)) {
		SSL_LOG(ERROR, "invalid asn type tag: expected primitive");
		return (SSL_ERR);
	}
	if (data->size != 0) {
		SSL_LOG(ERROR, "invalid null type: bad content size");
		return (SSL_ERR);
	}
	ft_ostr_append_ostr(encoded, data);

	SSL_LOG(TRACE, "null encoded successfully");
	return (SSL_OK);
}

static int	__encode_int(uint8_t tag, t_ostring *encoded, t_ostring *data)
{
	SSL_LOG(TRACE, "encoding integer, size: %zu", data->size);

	if (!SSL_FLAG(ASN_ENCODE_PRIMITIVE, tag)) {
		SSL_LOG(ERROR, "invalid asn type tag: expected primitive");
		return (SSL_ERR);
	}
	if (data->content == NULL) {
		SSL_LOG(ERROR, "invalid integer type: no content");
		return (SSL_ERR);
	}
	t_num *num = (t_num *)data->content;
	if (num->sign == BNUM_NEG) {
		// TODO: implement negative integer encoding
		SSL_LOG(ERROR, "invalid integer type: negative integer encoding is not implemented");
		return (SSL_ERR);
	}
	char *buf = NULL;
	size_t size = 0;
	bnum_to_bytes_u(num, &buf, &size);
	// If a positive integer's most significant bit of its first byte is 1, it must be prefixed with a 0x00 byte
	if (size > 0 && (((uint8_t *)buf)[0] & 0x80)) {
		uint8_t	byte = 0x00;
		ft_ostr_append(encoded, (char *)&byte, sizeof(byte));
	}
	ft_ostr_append(encoded, (char *)buf, size);
	SSL_FREE(buf);

	SSL_LOG(TRACE, "integer encoded successfully");
	return (SSL_OK);
}

static int	__encode_oid(uint8_t tag, t_ostring *encoded, t_ostring *data)
{
	SSL_LOG(TRACE, "encoding object identifier, size: %zu", data->size);

	if (!SSL_FLAG(ASN_ENCODE_PRIMITIVE, tag)) {
		SSL_LOG(ERROR, "invalid asn type tag: expected primitive");
		return (SSL_ERR);
	}
	if (data->size == 0) {
		SSL_LOG(ERROR, "invalid oid type: bad content size");
		return (SSL_ERR);
	}
	char *obj_id_cstr = ft_ostr_to_cstr(data, 0, data->size);
	SSL_LOG(TRACE, "object identifier: %s", obj_id_cstr);

	char *obj_name = asn1_oid_get_name(obj_id_cstr);
	if (NULL == obj_name) {
		SSL_LOG(WARN, "invalid or unknown asn object id: %s", obj_id_cstr);
	} else {
		SSL_LOG(TRACE, "object identifier matches: %s", obj_name);
	}
	SSL_FREE(obj_name);

	// Split the OID string into sub-identifiers
	char **sub_id_cstrs = ft_strsplit(obj_id_cstr, '.');
	SSL_FREE(obj_id_cstr);

	int num_sub_id_cstrs = ft_2darray_len_null_terminated((void **)sub_id_cstrs);

	if (num_sub_id_cstrs < 2 || NULL == sub_id_cstrs) {
		SSL_LOG(ERROR, UNSPECIFIED_ERROR);
		ft_2darray_del_null_terminated((void **)sub_id_cstrs);
		return (SSL_ERR);
	}

	SSL_LOG(TRACE, "parsed %d sub-identifiers", num_sub_id_cstrs);

	// Validate that all sub-identifiers are digits
	for (int i = 0; i < num_sub_id_cstrs; i++) {
		if (!ft_str_isnum(sub_id_cstrs[i])) {
			SSL_LOG(ERROR, UNSPECIFIED_ERROR);
			ft_2darray_del_null_terminated((void **)sub_id_cstrs);
			return (SSL_ERR);
		}
	}

	// Convert sub-identifiers to integers
	uint32_t sub_ids[64] = {0};
	int num_sub_ids = num_sub_id_cstrs - 1;
	sub_ids[0] = 40 * ft_atoi(sub_id_cstrs[0]) + ft_atoi(sub_id_cstrs[1]);

	for (int i = 1; i < num_sub_ids; i++) {
		sub_ids[i] = ft_atoi(sub_id_cstrs[i + 1]);
	}

	SSL_LOG(TRACE, "converted to %d integer sub-identifiers", num_sub_ids);

	// Encode sub-identifiers into octets
	unsigned char *id_octets = NULL;
	size_t id_octets_size = NBITS_TO_NWORDS(8 * sizeof(*sub_ids) * num_sub_ids, 7);
	id_octets_size = MAX(1, id_octets_size);
	SSL_ALLOC(id_octets, id_octets_size);

	id_octets_size = 0;

	for (int i = 0; i < num_sub_ids; i++) {
		uint32_t sub_id = sub_ids[i];
		int sub_id_enc_nbits = ft_uint_lmbit(sub_id, 8 * sizeof(sub_id));
		int sub_id_enc_size = NBITS_TO_NWORDS(sub_id_enc_nbits, 7);

		uint8_t *sub_id_enc = NULL;
		SSL_ALLOC(sub_id_enc, sub_id_enc_size);

		ssize_t idx = sub_id_enc_size;

		sub_id_enc[--idx] = 0x7F & sub_id;
		sub_id >>= 7;

		while (sub_id != 0) {
			sub_id_enc[--idx] = (0x7F & sub_id) | 0x80;
			sub_id >>= 7;
		}

		ft_memcpy(id_octets + id_octets_size, sub_id_enc, sub_id_enc_size);
		id_octets_size += sub_id_enc_size;
		SSL_FREE(sub_id_enc);
	}

	ft_ostr_append(encoded, (char *)id_octets, id_octets_size);
	SSL_FREE(id_octets);
	ft_2darray_del_null_terminated((void **)sub_id_cstrs);

	SSL_LOG(TRACE, "object identifier encoded successfully, octets: %zu", id_octets_size);

	return (SSL_OK);
}
