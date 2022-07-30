#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_der.h>

int der_encode_ostring(t_ostring *osbuf, void *content, size_t size)
{
	if (NULL == osbuf || NULL == content)
		return (DER_ERROR(INVALID_INPUT));

	osbuf->content = ft_memdup(content, size);
	osbuf->size = size;

	return (SSL_OK);
}
