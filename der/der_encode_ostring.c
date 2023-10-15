#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_der.h>

int der_encode_ostring(t_ostring *osbuf, void *content, size_t size)
{
	if (NULL == osbuf || NULL == content)
		return (DER_ERROR(INVALID_INPUT));

	util_ostr_append(osbuf, content, size);

	return (SSL_OK);
}
