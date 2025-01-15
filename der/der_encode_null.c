#include <common.h>
#include <der.h>

int der_encode_null(t_ostring *osbuf, void *content, size_t cont_nbits)
{
	if (NULL == osbuf) {
		DER_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	(void)content;
	(void)cont_nbits;

	return (SSL_OK);
}
