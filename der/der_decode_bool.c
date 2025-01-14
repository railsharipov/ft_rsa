#include <ssl.h>
#include <der.h>
#include <asn.h>

int	der_decode_bool(t_ostring *osbuf, uint8_t *enc, size_t size)
{
	if (NULL == osbuf || NULL == enc) {
		DER_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	if (size != 1) {
		DER_LOG(ERROR, "invalid der encoding");
		return (SSL_ERR);
	}

	ft_ostr_append(osbuf, enc, 1);

	return (SSL_OK);
}
