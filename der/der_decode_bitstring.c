#include <ssl.h>
#include <der.h>
#include <asn.h>

/*
/	Encoding shall be a bitstring encoding, that is, first encoding
/	octet shall be an octet containing the number of unused trailing
/	bits of the bitstring.
*/

int  der_decode_bitstring(t_ostring *osbuf, uint8_t *enc, size_t size)
{
	uint8_t	num_unused_bits;

	if (NULL == osbuf || NULL == enc) {
		DER_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	if (size < 1) {
		DER_LOG(ERROR, "invalid der encoding");
		return (SSL_ERR);
	}

	num_unused_bits = enc[0];

	if (num_unused_bits > 7u) {
		DER_LOG(ERROR, "invalid der encoding");
		return (SSL_ERR);
	}

	ft_ostr_append(osbuf, enc, size);

	return (SSL_OK);
}
