#include <ssl.h>
#include <der.h>

int der_encode_ostring(t_ostring *osbuf, void *content, size_t size)
{
	if (NULL == osbuf || NULL == content) {
		DER_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	ft_ostr_append(osbuf, content, size);

	return (SSL_OK);
}
