#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_der.h>
#include <ssl_asn.h>

static int	__tag_is_complex(int);
static void	__append_complex_tag(t_der *der, int, uint32_t);
static void	__append_simple_tag(t_der *der, int, uint32_t);

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
	if (NULL == der || tag_num < 0 || (tag_flags >> 8) != 0)
		return (DER_ERROR(INVALID_INPUT));

	if (__tag_is_complex(tag_num))
		__append_complex_tag(der, tag_flags, tag_num);
	else
		__append_simple_tag(der, tag_flags, tag_num);

	return (SSL_OK);
}

static inline int	__tag_is_complex(int tag_num)
{
	return (tag_num > 30);
}

static void	__append_complex_tag(t_der *der, int tag_flags, uint32_t tag_num)
{
	char	*tag_buf;
	int		tag_buf_size;
	int		tag_num_nbytes;
	int		tag_num_nbits;
	int		idx;

	tag_num_nbits = util_lmbit(tag_num, 8 * sizeof(tag_num));
	tag_num_nbytes = CEIL(tag_num_nbits, 7) / 7;

	// additional octet for tag flags
	tag_buf_size = 1 + tag_num_nbytes;
	SSL_ALLOC(tag_buf, tag_buf_size);

	// since tag number is in big-ENDian, write tag number octets
	// to the buffer backwards
	idx = tag_buf_size - 1;

	if (idx > 0)
	{
		tag_buf[idx--] = 0x7F & tag_num;
		tag_num >>= 7;
	}
	while (idx > 0)
	{
		tag_buf[idx--] = (0x7F & tag_num) | 0x80;
		tag_num >>= 7;
	}
	tag_buf[0] = ASN_TAG_COMPLEX | tag_flags;

	der_append_content(der, tag_buf, tag_buf_size);
	SSL_FREE(tag_buf);
}

static void	__append_simple_tag(t_der *der, int tag_flags, uint32_t tag_num)
{
	const int		tag_buf_size = 1;
	unsigned char	tag_buf[tag_buf_size];

	tag_buf[0] = ASN_TAG_SIMPLE | tag_flags;
	tag_buf[0] |= tag_num & 0x7F;

	der_append_content(der, tag_buf, tag_buf_size);
}