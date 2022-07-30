#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_der.h>
#include <ssl_asn.h>
#include <ssl_io.h>

static int		__tag_is_complex(int);
static ssize_t	__write_complex_tag(uint8_t, uint32_t, t_iodes *);
static ssize_t	__write_simple_tag(uint8_t, uint32_t, t_iodes *);

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

ssize_t	der_write_tag(uint8_t tag_flags, uint32_t tag_num, t_iodes *iodes)
{
	if (NULL == iodes || tag_num < 0)
		return (DER_ERROR(INVALID_INPUT));

	if (__tag_is_complex(tag_num))
		__write_complex_tag(tag_flags, tag_num, iodes);
	else
		__write_simple_tag(tag_flags, tag_num, iodes);

	return (SSL_OK);
}

static inline int	__tag_is_complex(int tag_num)
{
	return (tag_num > 30);
}

static ssize_t	__write_complex_tag(
	uint8_t tag_flags, uint32_t tag_num, t_iodes *iodes)
{
	ssize_t	wbytes;
	uint8_t	*tag_buf;
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
	tag_buf[0] = ASN_TAGNUM_COMPLEX | tag_flags;

	wbytes = der_write_octets((char *)tag_buf, tag_buf_size, iodes);
	SSL_FREE(tag_buf);

	return (wbytes);
}

static ssize_t	__write_simple_tag(
	uint8_t tag_flags, uint32_t tag_num, t_iodes *iodes)
{
	const int	tag_buf_size = 1;
	uint8_t		tag_buf[tag_buf_size];

	tag_buf[0] = ASN_TAGNUM_SIMPLE | tag_flags;
	tag_buf[0] |= tag_num & 0x7F;

	return (der_write_octets((char *)tag_buf, tag_buf_size, iodes));
}
