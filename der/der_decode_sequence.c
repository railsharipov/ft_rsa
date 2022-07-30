#include <ft_ssl.h>
#include <ssl_der.h>
#include <ssl_asn.h>

int	der_decode_sequence(t_ostring *osbuf, uint8_t *enc, size_t size)
{
	return (der_decode_ostring(osbuf, enc, size));
}
