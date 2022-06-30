#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <bnum.h>

static const int	ASN_TAG = (ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE);
static const int	ASN_PRIMITIVE = (ASN_ENCODE_PRIMITIVE | ASN_TAG);

int  der_read_bitstring(t_iasn *item, char **derenc, size_t *dersize)
{
	unsigned char	*octets;
	size_t			osize;
	size_t			olen;
	int				shift;

	if (NULL == item || NULL == derenc || NULL == *derenc || NULL == dersize)
		return DER_ERROR(INVALID_INPUT);

	octets = (unsigned char *)(*derenc);
	osize = *dersize;

	if ((ASN_PRIMITIVE | ASN_TAG_BIT_STRING) != *octets)
		return (DER_ERROR(INVALID_ASN_TYPE_TAG));

	octets++;
	osize--;

	if (SSL_OK != der_read_len(&octets, &osize, &olen))
		return (DER_ERROR(INVALID_ASN_LEN_TAG));

	if ((olen > osize) || (olen < 1))
		return (DER_ERROR(INVALID_ASN_LEN_TAG));

	shift = *octets++;
	olen--;

	if (shift > CHAR_BIT-1)
		return (DER_ERROR(INVALID_DER_ENCODING));

	SSL_ALLOC(item->content, olen);
	ft_memcpy(item->content, octets, olen);
	util_rshift_bytes((unsigned char *)item->content, olen, shift);
	item->bitsize = NBYTES_TO_NBITS(olen);

	*derenc = (char *)(octets) + olen;
	*dersize = osize - olen;

	return (SSL_OK);
}

int  der_read_bitstring_octets(t_iasn *item, unsigned char *derenc, size_t dersize)
{
	unsigned char	num_unused_bits;

	if (NULL == item || NULL == derenc)
		return (DER_ERROR(INVALID_INPUT));

	if (dersize == 0)
		return (DER_ERROR(INVALID_DER_ENCODING));

	num_unused_bits = *derenc;
	derenc++;
	dersize--;

	if (num_unused_bits != 0 && dersize == 0)
		return (DER_ERROR(INVALID_DER_ENCODING));

	if (num_unused_bits > CHAR_BIT-1)
		return (DER_ERROR(INVALID_DER_ENCODING));

	SSL_ALLOC(item->content, dersize);
	ft_memcpy(item->content, derenc, dersize);
	item->bitsize = NBYTES_TO_NBITS(dersize) - num_unused_bits;

	return (SSL_OK);
}
