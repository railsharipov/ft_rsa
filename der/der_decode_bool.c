#include <ssl/ssl.h>
#include <ssl/error.h>
#include <ssl/der.h>
#include <ssl/asn.h>

int	der_decode_bool(t_ostring *osbuf, uint8_t *enc, size_t size)
{
	if (NULL == osbuf || NULL == enc)
		return (DER_ERROR(INVALID_INPUT));

	if (size != 1)
		return (DER_ERROR(INVALID_DER_ENCODING));

	ft_ostr_append(osbuf, enc, 1);

	return (SSL_OK);
}
