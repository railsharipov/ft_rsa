#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_der.h>
#include <ssl_asn.h>

static int	__tag_is_complex(unsigned char *);
static int	__get_simple_tag_num(uint32_t *, size_t *, unsigned char *, size_t);
static int	__get_complex_tag_num(uint32_t *, size_t *, unsigned char *, size_t);

int	der_read_id_tagnum_octets(
	uint32_t *tagnum, size_t *tagnum_size, unsigned char *derenc, size_t dersize)
{
	if (NULL == tagnum_size || NULL == derenc)
		return (DER_ERROR(INVALID_INPUT));

	if (dersize == 0)
		return (DER_ERROR(INVALID_DER_ENCODING));

	if (__tag_is_complex(derenc))
		__get_simple_tag_num(tagnum, tagnum_size, derenc, dersize);
	else
		__get_complex_tag_num(tagnum, tagnum_size, derenc, dersize);

	return (SSL_OK);
}

static inline int	__tag_is_complex(unsigned char *derenc)
{
	return (SSL_FLAG(ASN_TAG_COMPLEX, *derenc));
}

static int	__get_simple_tag_num(
	uint32_t *tagnum, size_t *tagnum_size, unsigned char *derenc, size_t dersize)
{
	*tagnum_size = 1;
	*tagnum = *derenc & 0x1F;

	return (SSL_OK);
}

static int	__get_complex_tag_num(
	uint32_t *tagnum, size_t *tagnum_size, unsigned char *derenc, size_t dersize)
{
	uint32_t	octets;
	int			num_unused_bits;
	int			bitcnt;

	num_unused_bits = 8 - util_lmbit(*derenc, 8 * sizeof(*derenc));

	// get 7-bit blocks, except the last one
	while (dersize-- != 0 && *derenc & 0x80)
	{
		*tagnum <<= 7;
		*tagnum |= *derenc++ & 0x7F;
		bitcnt += 7;
	}

	if ((*derenc & 0x80) != 0)
		return (DER_ERROR(INVALID_DER_ENCODING));

	// get the last block
	*tagnum <<= 7;
	*tagnum |= *derenc;
	bitcnt += 7;

	bitcnt -= num_unused_bits;

	if (bitcnt > 8 * sizeof(octets))
		return (DER_ERROR(UNSPECIFIED_ERROR));

	*tagnum_size = NBITS_TO_NBYTES(bitcnt);

	return (SSL_OK);
}
