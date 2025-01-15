#include <common.h>
#include <der.h>
#include <asn.h>

int	der_decode_int(t_ostring *osbuf, uint8_t *enc, size_t size)
{
	return (der_decode_ostring(osbuf, enc, size));
}
