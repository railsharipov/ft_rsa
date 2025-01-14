#include <ssl.h>
#include <der.h>
#include <asn.h>

int	der_encode_bool(t_ostring *osbuf, void *content, size_t size)
{
	if (NULL == content || size != 1) {
		DER_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	return (der_encode_ostring(osbuf, content, size));
}
