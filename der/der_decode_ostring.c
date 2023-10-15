#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_der.h>
#include <ssl_asn.h>

int	der_decode_ostring(t_ostring *osbuf, uint8_t *enc, size_t size)
{
	if (NULL == osbuf || NULL == enc)
		return (DER_ERROR(INVALID_INPUT));

	if (size == 0)
		return (SSL_OK);

	util_ostr_append(osbuf, enc, size);

	return (SSL_OK);
}
