#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_der.h>
#include <ssl_asn.h>

int	der_encode_bool(t_ostring *osbuf, void *content, size_t size)
{
	if (NULL == content || size != 1)
		return (DER_ERROR(INVALID_INPUT));

	return (der_encode_ostring(osbuf, content, size));
}
