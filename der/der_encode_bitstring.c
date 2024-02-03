#include <ssl/ssl.h>
#include <ssl/asn.h>
#include <ssl/der.h>

/*
/	Content shall be encoded as a bitstring, that is, first content
/	octet shall be an extra octet containing the number of unused
/	trailing bits.
*/

int der_encode_bitstring(t_ostring *osbuf, void *content, size_t size)
{
	unsigned char	num_unused_bits;
	size_t			cont_nbytes;

	// first content octet SHALL be an extra octet that contains
	// the number of unused trailing bits, thus size is
	// expected to be at least 1 byte

	if (NULL == osbuf || NULL == content || size < 1)
		return (DER_ERROR(INVALID_INPUT_ERROR));

	num_unused_bits = ((uint8_t *)content)[0];

	if (num_unused_bits > 7u)
		return (DER_ERROR(INVALID_INPUT_ERROR));

	SSL_ALLOC(osbuf->content, size);

	ft_ostr_append(osbuf, content, size);

	return (SSL_OK);
}
