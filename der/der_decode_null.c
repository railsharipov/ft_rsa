#include <common.h>
#include <der.h>
#include <asn.h>

int  der_decode_null(t_ostring *osbuf, uint8_t *enc, size_t size)
{
	if (!(NULL == enc && size == 0)) {
		DER_LOG(ERROR, "invalid der encoding");
		return (SSL_ERR);
	}

	return (SSL_OK);
}
