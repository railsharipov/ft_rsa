#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

/*
/	Content shall be encoded as a bitstring, that is, first content
/	octet shall be an extra octet containing the number of unused
/	trailing bits. Encoding of the bitstring is a subject for
/	higher level functions.
*/

int der_encode_bitstring(t_ostring *osbuf, void *content, size_t size)
{
	unsigned char	num_unused_bits;
	size_t			cont_nbytes;

	// first content octet SHALL be an extra octet that contains
	// the number of unused trailing bits, thus size is
	// expected to be at least 1 byte

	if (NULL == osbuf || NULL == content || size < 1)
		return (DER_ERROR(INVALID_INPUT));

	num_unused_bits = ((uint8_t *)content)[0];

	if (num_unused_bits > 7u)
		return (DER_ERROR(INVALID_INPUT));

	SSL_ALLOC(osbuf->content, size);

	ft_memcpy(osbuf->content, content, size);
	osbuf->size = size;

	return (SSL_OK);
}
