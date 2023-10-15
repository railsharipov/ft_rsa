#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_der.h>
#include <ssl_asn.h>

/*
/	Encoding shall be a bitstring encoding, that is, first encoding
/	octet shall be an octet containing the number of unused trailing
/	bits of the bitstring.
*/

int  der_decode_bitstring(t_ostring *osbuf, uint8_t *enc, size_t size)
{
	uint8_t	num_unused_bits;

	if (NULL == osbuf || NULL == enc)
		return (DER_ERROR(INVALID_INPUT));

	if (size < 1)
		return (DER_ERROR(INVALID_DER_ENCODING));

	num_unused_bits = enc[0];

	if (num_unused_bits > 7u)
		return (DER_ERROR(INVALID_DER_ENCODING));

	util_ostr_append(osbuf, enc, size);

	return (SSL_OK);
}
