#include <ft_ssl.h>
#include <ssl_der.h>
#include <ssl_asn.h>

int  der_decode_null(t_ostring *osbuf, uint8_t *enc, size_t size)
{
	if (!(NULL == enc && size == 0))
		return (DER_ERROR(INVALID_DER_ENCODING));

	return (SSL_OK);
}
