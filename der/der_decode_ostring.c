#include <ssl.h>
#include <der.h>
#include <asn.h>

int	der_decode_ostring(t_ostring *osbuf, uint8_t *enc, size_t size)
{
	if (NULL == osbuf || NULL == enc) {
		DER_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	if (size == 0) {
		return (SSL_OK);
	}

	ft_ostr_append(osbuf, enc, size);

	return (SSL_OK);
}
