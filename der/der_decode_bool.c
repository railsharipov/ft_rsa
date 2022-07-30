#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_der.h>
#include <ssl_asn.h>

int	der_decode_bool(t_ostring *osbuf, uint8_t *enc, size_t size)
{
	if (NULL == osbuf || NULL == enc)
		return (DER_ERROR(INVALID_INPUT));

	if (size != 1)
		return (DER_ERROR(INVALID_DER_ENCODING));

	osbuf->content = ft_memdup(enc, size);
	osbuf->size = 1;

	return (SSL_OK);
}
