#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

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
