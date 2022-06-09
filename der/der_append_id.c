#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_der.h>
#include <ssl_asn.h>

static int	__tag_is_complex(int);
static int	__calculate_tag_num_nbytes(uint32_t);
static void	__append_complex_tag(char *, int, uint32_t);
static void	__append_simple_tag(char *, int, uint32_t);

/*
First octet:
	Bits 8 and 7 represent tag class. Bit 6 represent encoding complexity.
	Bits 5 to 1 represent tag number. If tag number <= 30, tag is simple.
	If tag number > 30, tag is complex, bits 5 to 1 are set and extra octets
	are used.
Following octets:
	If tag is complex, each following octet's 8th bit is set,
	except the last one. Bits 7 to 1 of each subsequent octet is used
	to store tag number represented as unsigned integer, with bit 7 of first
	subsequent octet representing most significant bit of integer.
*/

int	der_append_id_tag(t_der *der, int tag_flags, uint32_t tag_num)
{
	char	*id_content;
	int		id_nbytes;
	int		tag_num_nbytes;
	int		idx;

	if (NULL == der || tag_num < 0 || (tag_flags >> 8) != 0)
		return (DER_ERROR(INVALID_INPUT));

	tag_num_nbytes = __calculate_tag_num_nbytes(tag_num);

	if (tag_num_nbytes < 0)
		return (SSL_ERROR(UNEXPECTED_ERROR));

	id_nbytes = 1 + tag_num_nbytes;
	SSL_ALLOC(id_content, id_nbytes);
	id_content[0] = tag_flags;

	if (__tag_is_complex(tag_num))
		__append_complex_tag(id_content, id_nbytes, tag_num);
	else
		__append_simple_tag(id_content, id_nbytes, tag_num);

	der_append_content(der, id_content, id_nbytes);
	SSL_FREE(id_content);

	return (SSL_OK);
}

static int	__calculate_tag_num_nbytes(uint32_t tag_num)
{
	int	tag_num_nbits;
	int	tag_num_nbytes;

	tag_num_nbits = util_lmbit(tag_num, 8 * sizeof(tag_num));

	if (__tag_is_complex(tag_num))
	{
		tag_num_nbits = CEIL(tag_num_nbits, 7);
		tag_num_nbytes = tag_num_nbits / 7;
	}
	else
	{
		tag_num_nbytes = 0;
	}

	return (tag_num_nbytes);
}

static int	__tag_is_complex(int tag_num)
{
	return (tag_num > 30);
}

static void	__append_complex_tag(
	char *id_content, int id_nbytes, uint32_t tag_num)
{
	int	idx;

	id_content[0] |= ASN_TAG_COMPLEX;

	idx = id_nbytes - 1;

	if (idx > 0)
	{
		id_content[idx--] = 0x7F & tag_num;
		tag_num >>= 7;
	}
	while (idx > 0)
	{
		id_content[idx--] = (0x7F & tag_num) | 0x80;
		tag_num >>= 7;
	}
}

static void	__append_simple_tag(
	char *id_content, int id_nbytes, uint32_t tag_num)
{
	id_content[0] |= ASN_TAG_SIMPLE;
	id_content[0] |= tag_num & 0x7F;
}
