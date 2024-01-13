#include <ssl/ssl.h>
#include <ssl/error.h>
#include <ssl/der.h>

int der_encode_null(t_ostring *osbuf, void *content, size_t cont_nbits)
{
	if (NULL == osbuf)
		return (DER_ERROR(INVALID_INPUT));

	(void)content;
	(void)cont_nbits;

	return (SSL_OK);
}
