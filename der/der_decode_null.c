#include <ssl/ssl.h>
#include <ssl/der.h>
#include <ssl/asn.h>

int  der_decode_null(t_ostring *osbuf, uint8_t *enc, size_t size)
{
	if (!(NULL == enc && size == 0))
		return (DER_ERROR("invalid der encoding"));

	return (SSL_OK);
}
