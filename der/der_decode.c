#include <common.h>
#include <logger.h>
#include <io.h>
#include <asn1.h>
#include <bnum.h>
#include <der.h>
#include <json.h>
#include <libft.h>
#include <unistd.h>

static int		__decode(t_node **node, t_io_v2_stream *in);
static ssize_t	__read_tag(uint8_t *tag, uint32_t *tagnum, t_io_v2_stream *in);
static ssize_t	__read_len(size_t *len, uint8_t *form, t_io_v2_stream *in);
static ssize_t	__read_content_octets(t_ostring *osbuf, t_io_v2_stream *in);

static int	__decode_construct(t_node **nodes, t_ostring *encoded);
static int	__decode_ostring(uint8_t tag, t_ostring *decoded, t_ostring *encoded);
static int	__decode_bitstring(uint8_t tag, t_ostring *decoded, t_ostring *encoded);
static int	__decode_bool(uint8_t tag, t_ostring *decoded, t_ostring *encoded);
static int	__decode_sequence(uint8_t tag, t_ostring *decoded, t_ostring *encoded);
static int	__decode_null(uint8_t tag, t_ostring *decoded, t_ostring *encoded);
static int	__decode_int(uint8_t tag, t_ostring *decoded, t_ostring *encoded);
static int	__decode_oid(uint8_t tag, t_ostring *decoded, t_ostring *encoded);

typedef int	(*t_func_der_decode)(uint8_t tag, t_ostring *decoded, t_ostring *encoded);

int	der_decode(t_node **asn1_node, t_ostring *encoded)
{
	SSL_LOG(TRACE, "starting DER decode octet string");

	if (NULL == asn1_node || NULL == encoded) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	t_io_v2_stream *in = NULL;
	if (SSL_OK != io_v2_bytes_reader(&in, encoded)) {
		SSL_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != der_decode_stream(asn1_node, in)) {
		SSL_LOG(ERROR, "failed to decode octet string");
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "DER decode octet string completed successfully");

	return (SSL_OK);
}

int	der_decode_stream(t_node **asn1_node, t_io_v2_stream *in)
{
	SSL_LOG(TRACE, "starting DER decode stream");

	if (NULL == asn1_node || NULL == in) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != __decode(asn1_node, in)) {
		SSL_LOG(TRACE, "DER decode failed");
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "DER decode stream completed successfully");

	return (SSL_OK);
}

static int	__decode(t_node **node, t_io_v2_stream *in)
{
	*node = NULL;

	uint8_t	tag = 0;
	uint32_t tagnum = 0;
	ssize_t	rbytes = __read_tag(&tag, &tagnum, in);

	// If no tag octets, then end of encoding.
	if (rbytes == 0) {
		SSL_LOG(TRACE, "end of encoding reached");
		return (SSL_OK);
	}
	if (rbytes < 0) {
		SSL_LOG(ERROR, "read tag error");
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "decoding tag number: %u, tag: %u", tagnum, tag);

	t_func_der_decode f_decode = NULL;
	switch (tagnum) {
		case ASN_TAGNUM_SEQUENCE:
			f_decode = __decode_sequence;
			break;
		case ASN_TAGNUM_BOOLEAN:
			f_decode = __decode_bool;
			break;
		case ASN_TAGNUM_OBJECT_ID:
			f_decode = __decode_oid;
			break;
		case ASN_TAGNUM_INT:
			f_decode = __decode_int;
			break;
		case ASN_TAGNUM_OCTET_STRING:
			f_decode = __decode_ostring;
			break;
		case ASN_TAGNUM_BIT_STRING:
			f_decode = __decode_bitstring;
			break;
		case ASN_TAGNUM_NULL:
			f_decode = __decode_null;
			break;
		default:
			SSL_LOG(ERROR, "unknown tag number: %#x", tagnum);
			return (SSL_ERR);
	}

	t_ostring encoded;
	ft_ostr_init(&encoded);
	if (__read_content_octets(&encoded, in) < 0) {
		SSL_LOG(ERROR, "read content octets error");
		return (SSL_ERR);
	}
	SSL_LOG(TRACE, "content octets read, size: %zu", encoded.size);

	t_ostring decoded;
	ft_ostr_init(&decoded);
	int ret = f_decode(tag, &decoded, &encoded);
	ft_ostr_clear(&encoded);

	if (SSL_OK != ret) {
		SSL_LOG(ERROR, "decode function failed for tag: %u", tagnum);
		return (SSL_ERR);
	}

	t_iasn *asn1_item = asn1_item_create();
	asn1_item->tag = tag;
	asn1_item->tagnum = tagnum;
	asn1_item->content = decoded.content;
	asn1_item->size = decoded.size;
	*node = asn1_node_create(asn1_item);

	SSL_LOG(TRACE, "node created successfully for tag: %u", tagnum);

	return (SSL_OK);
}

static ssize_t	__read_tag(uint8_t *tag, uint32_t *tagnum, t_io_v2_stream *in)
{
	ssize_t	rbytes = 0;
	ssize_t	tbytes = 0;

	*tag = 0;
	tbytes = 0;

	rbytes = io_v2_read(in, tag, 1);
	if (rbytes < 0) {
		if (in->status == IO_V2_STATUS_EOF) {
			SSL_LOG(TRACE, "no tag bytes read");
			return (0);
		}
		SSL_LOG(ERROR, "read tag error: bad read");
		return (-1);
	}
	tbytes += rbytes;

	*tagnum = *tag & 0x1F;
	*tag = *tag & 0xE0;

	SSL_LOG(TRACE, "read tag number: %u, tag: %u", *tagnum, *tag);

    if (ASN_TAGNUM_COMPLEX == *tagnum) {
    	uint8_t octet = 0;
        *tagnum = 0;

        do {
            rbytes = io_v2_read(in, &octet, 1);
            if (rbytes <= 0) {
                SSL_LOG(ERROR, "read complex tag error: bad read");
                return (-1);
            }
            tbytes += rbytes;

            *tagnum <<= 7;
            *tagnum |= (uint32_t)octet & 0x7F;
        } while (octet & 0x80);

        SSL_LOG(TRACE, "complex tag number: %u", *tagnum);
    }
	return (tbytes);
}

static ssize_t	__read_len(size_t *len, uint8_t *form, t_io_v2_stream *in)
{
	uint8_t	octet = 0;
	ssize_t rbytes = 0;
	ssize_t tbytes = 0;

	*len = 0;

	if ((rbytes = io_v2_read(in, &octet, 1)) < 0) {
		if (in->status == IO_V2_STATUS_EOF) {
			SSL_LOG(TRACE, "no length bytes read");
			return (0);
		}
		SSL_LOG(ERROR, "read length error: bad read");
		return (-1);
	}
	tbytes += rbytes;

	if (octet & ASN_LEN_LONG) {
		*form = ASN_LEN_LONG;
		size_t lensize = octet & 0x7F;

		SSL_LOG(TRACE, "reading long length form, size bytes: %zu", lensize);

		while (lensize > 0) {
			rbytes = io_v2_read(in, &octet, 1);
			if (rbytes <= 0) {
				SSL_LOG(ERROR, "read long length form error: bad read");
				return (-1);
			}
			*len <<= CHAR_BIT;
			*len |= (size_t)octet;

			tbytes += rbytes;
			lensize--;
		}
	}
	else {
		*form = ASN_LEN_SHORT;
		*len = (size_t)octet;
		SSL_LOG(TRACE, "reading short length form: %zu", *len);
	}
	SSL_LOG(TRACE, "length read: %zu, form: %u", *len, *form);

	return (tbytes);
}

static ssize_t	__read_content_octets(t_ostring *osbuf, t_io_v2_stream *in)
{
	ssize_t	rbytes = 0;
	ssize_t tbytes = 0;
	size_t len = 0;
	uint8_t lenform = 0;

	rbytes = __read_len(&len, &lenform, in);
	if (rbytes <= 0) {
		SSL_LOG(ERROR, "read content octets error: bad read");
		return (-1);
	}
	tbytes += rbytes;

	SSL_LOG(TRACE, "reading content octets, length: %zu", len);

    if (ASN_LEN_LONG == lenform && len == 0) {
		SSL_LOG(TRACE, "reading indefinite length content");

		t_io_v2_stream *out = NULL;
		if (SSL_OK != io_v2_bytes_writer(&out, osbuf)) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (-1);
		}

		uint8_t octet = 0;
		int	null_count = 0;
		rbytes = 0;
		tbytes = 0;
        // Read until End-of-Contents (0x00 0x00), but avoid busy-waiting
        while (null_count < 2) {
            rbytes = io_v2_read(in, &octet, 1);
            if (rbytes <= 0) {
                SSL_LOG(ERROR, "read indefinite length content error: bad read");
                goto label_error;
            }
            tbytes += 1;
            if (io_v2_write(out, &octet, 1) != 1) {
                goto label_error;
            }
            if (octet == 0) {
                null_count += 1;
            } else {
                null_count = 0;
            }
        }
		SSL_LOG(TRACE, "indefinite length content read, total bytes: %zd", tbytes);
	}
	else if (len > 0) {
		uint8_t buf[len];
		rbytes = io_v2_read(in, buf, sizeof(buf));
		if (rbytes < 0) {
			if (in->status == IO_V2_STATUS_EOF) {
				rbytes = 0;
			} else {
				SSL_LOG(ERROR, "read definite length content error: bad read");
				goto label_error;
			}
		}
		ft_ostr_append(osbuf, buf, sizeof(buf));
		tbytes += rbytes;
		SSL_LOG(TRACE, "definite length content is read, bytes: %zd", rbytes);
	}
	else {
		SSL_LOG(TRACE, "no definite length content bytes");
	}
	return (tbytes);

label_error:
	ft_ostr_clear(osbuf);

	return (-1);
}


static int	__decode_construct(t_node **nodes, t_ostring *encoded)
{
	SSL_LOG(TRACE, "decoding constructed type, size: %zu", encoded->size);

	*nodes = NULL;

	t_io_v2_stream *in = NULL;
	if (SSL_OK != io_v2_bytes_reader(&in, encoded)) {
		SSL_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}

	t_node *child = NULL;
	do {
		if (SSL_OK != __decode(&child, in)) {
			SSL_LOG(ERROR, "failed to decode child node in constructed type");
			return (SSL_ERR);
		}
		ft_lst_append(nodes, child);
	}
	while (child != NULL);

	SSL_LOG(TRACE, "constructed type decoded successfully");

	return (SSL_OK);
}

static int	__decode_ostring(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	SSL_LOG(TRACE, "decoding octet string, size: %zu", encoded->size);

	if (encoded->size == 0) {
		decoded->content = NULL;
		decoded->size = 0;
		SSL_LOG(TRACE, "empty octet string decoded");
		return (SSL_OK);
	}

	if (SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		SSL_LOG(ERROR, "invalid der encoding: ostring type: expected primitive, got construct");
		return (SSL_ERR);
	}

	ft_ostr_append_ostr(decoded, encoded);
	SSL_LOG(TRACE, "octet string decoded successfully");
	return (SSL_OK);
}

static int	__decode_bitstring(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	SSL_LOG(TRACE, "decoding bit string, size: %zu", encoded->size);

	if (encoded->size < 1) {
		SSL_LOG(ERROR, "invalid der encoding: bitstring: bad length");
		return (SSL_ERR);
	}

	if (SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		SSL_LOG(TRACE, "decoding constructed bit string");

		t_node *nodes = NULL;
		if (SSL_OK != __decode_construct(&nodes, encoded)) {
			SSL_LOG(ERROR, "invalid der encoding: bitstring: bad construct");
			return (SSL_ERR);
		}
		for (t_node *child = nodes; child != NULL; child = child->next) {
			ft_ostr_append(decoded, child->content, child->size);
		}
		ft_lst_del(nodes);
		SSL_LOG(TRACE, "constructed bit string decoded successfully");
		return (SSL_OK);
	}

	ft_ostr_append_ostr(decoded, encoded);
	SSL_LOG(TRACE, "primitive bit string decoded successfully");
	return (SSL_OK);
}

static int	__decode_bool(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	SSL_LOG(TRACE, "decoding boolean, size: %zu", encoded->size);

	if (encoded->size != 1) {
		SSL_LOG(ERROR, "invalid der encoding: bool: bad length");
		return (SSL_ERR);
	}

	if (SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		SSL_LOG(ERROR, "invalid der encoding: bool type: expected primitive, got construct");
		return (SSL_ERR);
	}

	ft_ostr_append_ostr(decoded, encoded);
	SSL_LOG(TRACE, "boolean decoded successfully");
	return (SSL_OK);
}

static int	__decode_sequence(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	SSL_LOG(TRACE, "decoding sequence, size: %zu", encoded->size);

	if (!SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		SSL_LOG(ERROR, "invalid der encoding: sequence type: expected construct, got primitive");
		return (SSL_ERR);
	}

	t_node *nodes = NULL;
	if (SSL_OK != __decode_construct(&nodes, encoded)) {
		SSL_LOG(ERROR, "sequence construct decode failed");
		return (SSL_ERR);
	}

	decoded->content = (uint8_t *)nodes;
	decoded->size = ft_lst_size(nodes);

	SSL_LOG(TRACE, "sequence decoded successfully, child nodes: %zu", decoded->size);

	return (SSL_OK);
}

static int	__decode_null(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	SSL_LOG(TRACE, "decoding null, size: %zu", encoded->size);

	if (SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		SSL_LOG(ERROR, "invalid der encoding: null type: expected primitive, got construct");
		return (SSL_ERR);
	}

	ft_ostr_append_ostr(decoded, encoded);
	SSL_LOG(TRACE, "null decoded successfully");
	return (SSL_OK);
}

static int	__decode_int(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	SSL_LOG(TRACE, "decoding integer, size: %zu", encoded->size);

	if (SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		SSL_LOG(ERROR, "invalid der encoding: int type: expected primitive, got construct");
		return (SSL_ERR);
	}

	t_num *num = bnum_create();
	bnum_from_bytes_u(num, (char *)encoded->content, encoded->size);
	decoded->content = (uint8_t *)num;
	decoded->size = 0;

	SSL_LOG(TRACE, "integer decoded successfully");
	return (SSL_OK);
}

static int	__decode_oid(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	SSL_LOG(TRACE, "decoding object identifier, size: %zu", encoded->size);

	if (encoded->size == 0) {
		SSL_LOG(ERROR, "invalid der encoding");
		return (SSL_ERR);
	}

	if (SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		SSL_LOG(ERROR, "invalid der encoding: oid type: expected primitive, got construct");
		return (SSL_ERR);
	}

	uint32_t sub_ids[encoded->size + 1];
	int num_sub_ids = 0;

	for (int i = 0; i < (int)encoded->size; ) {
		sub_ids[num_sub_ids] = 0;

		// Get 7-bit blocks, up to the last one
		while (i < (int)encoded->size && (((char *)encoded->content)[i] & 0x80)) {
			sub_ids[num_sub_ids] <<= 7;
			sub_ids[num_sub_ids] |= ((char *)encoded->content)[i] & 0x7F;
			i++;
		}

		// If we've reached the end of the encoded content, then we've got an invalid der encoding
		if (i >= (int)encoded->size) {
			SSL_LOG(ERROR, "invalid der encoding");
			return (SSL_ERR);
		}

		// Get the last block
		sub_ids[num_sub_ids] <<= 7;
		sub_ids[num_sub_ids] |= ((char *)encoded->content)[i] & 0x7F;
		i++;
		num_sub_ids++;
	}

	SSL_LOG(TRACE, "parsed %d sub-identifiers", num_sub_ids);

	if (num_sub_ids < 2) {
		SSL_LOG(ERROR, "invalid number of object sub ids");
		return (SSL_ERR);
	}

	//	First two ids are concatenated into one single id using following formula:
	//	CONCAT_ID = 40 * ID_0 + ID_1
	char *sub_id_strings[encoded->size + 1];
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

	SSL_LOG(TRACE, "object identifier: %s", obj_id);

	decoded->content = (unsigned char *)obj_id;
	decoded->size = ft_strlen(obj_id);

	SSL_LOG(TRACE, "object identifier decoded successfully: %s", obj_id);

	return (SSL_OK);
}
