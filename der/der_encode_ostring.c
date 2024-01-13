#include <ssl/ssl.h>
#include <ssl/error.h>
#include <ssl/der.h>

int der_encode_ostring(t_ostring *osbuf, void *content, size_t size)
{
	if (NULL == osbuf || NULL == content)
		return (DER_ERROR(INVALID_INPUT));

	ft_ostr_append(osbuf, content, size);

	return (SSL_OK);
}
