#include <ssl/ssl.h>
#include <ssl/der.h>
#include <ssl/asn.h>

int	der_decode_ostring(t_ostring *osbuf, uint8_t *enc, size_t size)
{
	if (NULL == osbuf || NULL == enc)
		return (DER_ERROR(INVALID_INPUT_ERROR));

	if (size == 0)
		return (SSL_OK);

	ft_ostr_append(osbuf, enc, size);

	return (SSL_OK);
}
