#include <common.h>
#include <io.h>
#include <asn.h>
#include <bnum.h>
#include <der.h>
#include <json.h>
#include <libft/list.h>
#include <libft/node.h>
#include <libft/htable.h>
#include <libft/2darray.h>
#include <libft/bytes.h>
#include <libft/string.h>
#include <unistd.h>

static int		__decode(t_node **node, t_iodes *in);
static ssize_t	__read_tag(uint8_t *tag, uint32_t *tagnum, t_iodes *in);
static ssize_t	__read_len(size_t *len, uint8_t *form, t_iodes *in);
static ssize_t	__read_content_octets(t_ostring *osbuf, t_iodes *in);

static int	__decode_construct(t_node **nodes, t_ostring *encoded);
static int	__decode_ostring(uint8_t tag, t_ostring *decoded, t_ostring *encoded);
static int	__decode_bitstring(uint8_t tag, t_ostring *decoded, t_ostring *encoded);
static int	__decode_bool(uint8_t tag, t_ostring *decoded, t_ostring *encoded);
static int	__decode_sequence(uint8_t tag, t_ostring *decoded, t_ostring *encoded);
static int	__decode_null(uint8_t tag, t_ostring *decoded, t_ostring *encoded);
static int	__decode_int(uint8_t tag, t_ostring *decoded, t_ostring *encoded);
static int	__decode_oid(uint8_t tag, t_ostring *decoded, t_ostring *encoded);

typedef int	(*FUNC_DER_DECODE)(uint8_t tag, t_ostring *decoded, t_ostring *encoded);

int	der_decode(t_node **tree, t_ostring *encoded)
{
	t_iodes	iodes;

	DER_LOG(TRACE, "starting DER decode octet string");

	if (NULL == tree || NULL == encoded) {
		DER_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	if (SSL_OK != io_osbuf(&iodes, IO_READ, encoded)) {
		DER_LOG(ERROR, "failed to init io");
		return (SSL_ERR);
	}
	if (SSL_OK != der_decode_stream(tree, &iodes)) {
		DER_LOG(ERROR, "failed to decode octet string");
		return (SSL_ERR);
	}
	DER_LOG(TRACE, "DER decode octet string completed successfully");

	return (SSL_OK);
}

int	der_decode_stream(t_node **tree, t_iodes *in)
{
	t_node	*node;

	DER_LOG(TRACE, "starting DER decode stream");

	if (NULL == tree || NULL == in) {
		DER_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	if (SSL_OK != __decode(&node, in)) {
		DER_LOG(TRACE, "DER decode failed");
		return (SSL_ERR);
	}

	*tree = node;
	DER_LOG(TRACE, "DER decode stream completed successfully");

	return (SSL_OK);
}

static int	__decode(t_node **node, t_iodes *in)
{
	FUNC_DER_DECODE	f_decode;
	t_ostring		encoded, decoded;
	t_iasn			*asn_item;
	ssize_t			rbytes;
	uint8_t			tag;
	uint32_t		tagnum;
	int				ret;

	*node = NULL;

	// If no tag octets, then end of encoding.
	if ((rbytes = __read_tag(&tag, &tagnum, in)) == 0) {
		DER_LOG(TRACE, "end of encoding reached");
		return (SSL_OK);
	}
	if (rbytes < 0) {
		DER_LOG(ERROR, "read tag error");
		return (SSL_ERR);
	}

	ft_ostr_init(&encoded);
	ft_ostr_init(&decoded);

	DER_LOG(TRACE, "decoding tag number: %u, tag: %u", tagnum, tag);

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
			DER_LOG(ERROR, "unknown tag number: %#x", tagnum);
			return (SSL_ERR);
	}

	if (__read_content_octets(&encoded, in) < 0) {
		DER_LOG(ERROR, "read content octets error");
		return (SSL_ERR);
	}

	DER_LOG(TRACE, "content octets read, size: %zu", encoded.size);

	ret = f_decode(tag, &decoded, &encoded);

	ft_ostr_clear(&encoded);

	if (SSL_OK != ret) {
		DER_LOG(ERROR, "decode function failed for tag: %u", tagnum);
		return (SSL_ERR);
	}

	asn_item = asn_item_create();
	asn_item->tag = tag;
	asn_item->tagnum = tagnum;
	asn_item->content = decoded.content;
	asn_item->size = decoded.size;
	*node = asn_node_create(asn_item);

	DER_LOG(TRACE, "node created successfully for tag: %u", tagnum);

	return (SSL_OK);
}

static ssize_t	__read_tag(uint8_t *tag, uint32_t *tagnum, t_iodes *in)
{
	char		octet;
	ssize_t		rbytes;
	ssize_t		tbytes;

	if (NULL == tag || NULL == tagnum || NULL == in) {
		return (-1);
	}
	*tag = 0;
	tbytes = 0;

	if ((rbytes = io_read(in, (char *)tag, 1)) < 0) {
		DER_LOG(ERROR, "read tag error: bad read");
		return (-1);
	}
	if (rbytes == 0) {
		DER_LOG(TRACE, "no tag bytes read");
		return (0);
	}
	tbytes += rbytes;

	*tagnum = *tag & 0x1F;
	*tag = *tag & 0xE0;

	DER_LOG(TRACE, "read tag number: %u, tag: %u", *tagnum, *tag);

    if (ASN_TAGNUM_COMPLEX == *tagnum) {
        *tagnum = 0;

        do {
            rbytes = io_read(in, (char *)&octet, 1);
            if (rbytes <= 0) {
                DER_LOG(ERROR, "read complex tag error: bad read");
                return (-1);
            }
            tbytes += rbytes;

            *tagnum <<= 7;
            *tagnum |= (uint32_t)octet & 0x7F;
        } while (octet & 0x80);

        DER_LOG(TRACE, "complex tag number: %u", *tagnum);
    }

	return (tbytes);
}

static ssize_t	__read_len(size_t *len, uint8_t *form, t_iodes *in)
{
	uint8_t		octet;
	ssize_t		rbytes;
	ssize_t		tbytes;
	size_t		lensize;

	if (NULL == len || NULL == form || NULL == in) {
		return (-1);
	}

	*len = 0;
	tbytes = 0;

	if ((rbytes = io_read(in, (char *)&octet, 1)) < 0) {
		DER_LOG(ERROR, "read length error: bad read");
		return (-1);
	}
	if (rbytes == 0) {
		DER_LOG(TRACE, "no length bytes read");
		return (0);
	}
	tbytes += rbytes;

	if (octet & ASN_LEN_LONG) {
		*form = ASN_LEN_LONG;
		lensize = octet & 0x7F;

		DER_LOG(TRACE, "reading long length form, size bytes: %zu", lensize);

		while (lensize > 0) {
			if ((rbytes = io_read(in, (char *)&octet, 1)) <= 0) {
				DER_LOG(ERROR, "read long length form error: bad read");
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
		DER_LOG(TRACE, "reading short length form: %zu", *len);
	}

	DER_LOG(TRACE, "length read: %zu, form: %u", *len, *form);

	return (tbytes);
}

static ssize_t	__read_content_octets(t_ostring *osbuf, t_iodes *in)
{
	ssize_t	rbytes, tbytes;
	size_t	len;
	uint8_t	lenform;

	tbytes = 0;

	if ((rbytes = __read_len(&len, &lenform, in)) <= 0) {
		DER_LOG(ERROR, "read content octets error: bad read");
		return (-1);
	}
	tbytes += rbytes;

	DER_LOG(TRACE, "reading content octets, length: %zu", len);

    if (ASN_LEN_LONG == lenform && len == 0) {
		t_iodes	out;
		char	octet;
		int		null_count;

		DER_LOG(TRACE, "reading indefinite length content");

		if (SSL_OK != io_osbuf(&out, IO_WRITE|IO_OSBUF, osbuf)) {
			return (-1);
		}
		null_count = 0;
		rbytes = 0;
		tbytes = 0;

        // Read until End-of-Contents (0x00 0x00), but avoid busy-waiting
        while (null_count < 2) {
            rbytes = io_read(in, &octet, 1);
            if (rbytes <= 0) {
                DER_LOG(ERROR, "read indefinite length content error: bad read");
                goto label_error;
            }
            tbytes += 1;

            if (io_write(&out, &octet, 1) != 1) {
                goto label_error;
            }

            if (octet == 0) {
                null_count += 1;
            } else {
                null_count = 0;
            }
        }
		DER_LOG(TRACE, "indefinite length content read, total bytes: %zd", tbytes);
	}
	else {
		ft_ostr_append(osbuf, NULL, len);

		if ((rbytes = io_read(in, (char *)osbuf->content, len)) < 0) {
			DER_LOG(ERROR, "read definite length content error: bad read");
			goto label_error;
		}
		osbuf->size = rbytes;
		DER_LOG(TRACE, "definite length content read, bytes: %zd", rbytes);
	}
	tbytes += rbytes;

	return (tbytes);

label_error:
	ft_ostr_clear(osbuf);

	return (-1);
}


static int	__decode_construct(t_node **nodes, t_ostring *encoded)
{
	t_node		*child;
	t_iodes		iodes;

	*nodes = NULL;

	DER_LOG(TRACE, "decoding constructed type, size: %zu", encoded->size);

	if (SSL_OK != io_osbuf(&iodes, IO_READ|IO_OSBUF, encoded)) {
		DER_LOG(ERROR, "failed to create input buffer for constructed type");
		return (SSL_ERR);
	}

	do {
		if (SSL_OK != __decode(&child, &iodes)) {
			DER_LOG(ERROR, "failed to decode child node in constructed type");
			return (SSL_ERR);
		}
		ft_lst_append(nodes, child);
	}
	while (child != NULL);

	DER_LOG(TRACE, "constructed type decoded successfully");

	return (SSL_OK);
}

static int	__decode_ostring(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	DER_LOG(TRACE, "decoding octet string, size: %zu", encoded->size);

	if (encoded->size == 0) {
		decoded->content = NULL;
		decoded->size = 0;
		DER_LOG(TRACE, "empty octet string decoded");
		return (SSL_OK);
	}

	if (SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		DER_LOG(ERROR, "invalid der encoding: ostring type: expected primitive, got construct");
		return (SSL_ERR);
	}

	ft_ostr_append_ostr(decoded, encoded);
	DER_LOG(TRACE, "octet string decoded successfully");
	return (SSL_OK);
}

static int	__decode_bitstring(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	DER_LOG(TRACE, "decoding bit string, size: %zu", encoded->size);

	if (encoded->size < 1) {
		DER_LOG(ERROR, "invalid der encoding: bitstring: bad length");
		return (SSL_ERR);
	}

	if (SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		t_node	*nodes;

		DER_LOG(TRACE, "decoding constructed bit string");

		if (SSL_OK != __decode_construct(&nodes, encoded)) {
			DER_LOG(ERROR, "invalid der encoding: bitstring: bad construct");
			return (SSL_ERR);
		}
		for (t_node *child = nodes; child != NULL; child = child->next) {
			ft_ostr_append(decoded, child->content, child->size);
		}
		ft_lst_del(nodes);
		DER_LOG(TRACE, "constructed bit string decoded successfully");
		return (SSL_OK);
	}

	ft_ostr_append_ostr(decoded, encoded);
	DER_LOG(TRACE, "primitive bit string decoded successfully");
	return (SSL_OK);
}

static int	__decode_bool(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	DER_LOG(TRACE, "decoding boolean, size: %zu", encoded->size);

	if (encoded->size != 1) {
		DER_LOG(ERROR, "invalid der encoding: bool: bad length");
		return (SSL_ERR);
	}

	if (SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		DER_LOG(ERROR, "invalid der encoding: bool type: expected primitive, got construct");
		return (SSL_ERR);
	}

	ft_ostr_append_ostr(decoded, encoded);
	DER_LOG(TRACE, "boolean decoded successfully");
	return (SSL_OK);
}

static int	__decode_sequence(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	t_node	*nodes;

	DER_LOG(TRACE, "decoding sequence, size: %zu", encoded->size);

	if (!SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		DER_LOG(ERROR, "invalid der encoding: sequence type: expected construct, got primitive");
		return (SSL_ERR);
	}

	if (SSL_OK != __decode_construct(&nodes, encoded)) {
		DER_LOG(ERROR, "sequence construct decode failed");
		return (SSL_ERR);
	}

	decoded->content = (unsigned char *)nodes;
	decoded->size = ft_lst_size(nodes);

	DER_LOG(TRACE, "sequence decoded successfully, child nodes: %zu", decoded->size);

	return (SSL_OK);
}

static int	__decode_null(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	DER_LOG(TRACE, "decoding null, size: %zu", encoded->size);

	if (SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		DER_LOG(ERROR, "invalid der encoding: null type: expected primitive, got construct");
		return (SSL_ERR);
	}

	ft_ostr_append_ostr(decoded, encoded);
	DER_LOG(TRACE, "null decoded successfully");
	return (SSL_OK);
}

static int	__decode_int(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	t_num	*num;

	DER_LOG(TRACE, "decoding integer, size: %zu", encoded->size);

	if (SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		DER_LOG(ERROR, "invalid der encoding: int type: expected primitive, got construct");
		return (SSL_ERR);
	}

	num = bnum_create();
	bnum_from_bytes_u(num, (char *)encoded->content, encoded->size);
	decoded->content = (unsigned char *)num;
	decoded->size = 0;

	DER_LOG(TRACE, "integer decoded successfully");
	return (SSL_OK);
}

static int	__decode_oid(uint8_t tag, t_ostring *decoded, t_ostring *encoded)
{
	char		*obj_name, *obj_id;
	uint32_t	sub_ids[encoded->size + 1];
	char		*sub_id_strings[encoded->size + 1];
	int			num_sub_ids;

	DER_LOG(TRACE, "decoding object identifier, size: %zu", encoded->size);

	if (encoded->size == 0) {
		DER_LOG(ERROR, "invalid der encoding");
		return (SSL_ERR);
	}

	if (SSL_FLAG(ASN_ENCODE_CONSTRUCT, tag)) {
		DER_LOG(ERROR, "invalid der encoding: oid type: expected primitive, got construct");
		return (SSL_ERR);
	}

	num_sub_ids = 0;

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
			DER_LOG(ERROR, "invalid der encoding");
			return (SSL_ERR);
		}

		// Get the last block
		sub_ids[num_sub_ids] <<= 7;
		sub_ids[num_sub_ids] |= ((char *)encoded->content)[i] & 0x7F;
		i++;
		num_sub_ids++;
	}

	DER_LOG(TRACE, "parsed %d sub-identifiers", num_sub_ids);

	if (num_sub_ids < 2) {
		DER_LOG(ERROR, "invalid number of object sub ids");
		return (SSL_ERR);
	}

	//	First two ids are concatenated into one single id using following formula:
	//	CONCAT_ID = 40 * ID_0 + ID_1
	ft_sprintf(&sub_id_strings[0], "%lu.", sub_ids[0] / 40);
	ft_sprintf(&sub_id_strings[1], "%lu.", sub_ids[0] % 40);

	// Get the rest of ids, except the last one
	for (int i = 1; i < num_sub_ids-1; i++) {
		ft_sprintf(&sub_id_strings[i+1], "%lu.", sub_ids[i]);
	}

	// Get the last id
 	ft_sprintf(&sub_id_strings[num_sub_ids], "%lu", sub_ids[num_sub_ids-1]);

	// Join all sub-id strings into an object id string
	obj_id = ft_2darray_strjoin(sub_id_strings, num_sub_ids + 1, "");

	DER_LOG(TRACE, "object identifier: %s", obj_id);

	obj_name = asn_oid_tree_get_name(obj_id);
	if (NULL == obj_name) {
		DER_LOG(WARN, "unknown asn object id: %s", obj_id);
	} else {
		DER_LOG(TRACE, "object identifier matches: %s", obj_name);
	}
	SSL_FREE(obj_name);

	decoded->content = (unsigned char *)obj_id;
	decoded->size = ft_strlen(obj_id);

	DER_LOG(TRACE, "object identifier decoded successfully: %s", obj_id);

	return (SSL_OK);
}
