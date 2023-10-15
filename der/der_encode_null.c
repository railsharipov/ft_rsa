#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_der.h>

int der_encode_null(t_ostring *osbuf, void *content, size_t cont_nbits)
{
	if (NULL == osbuf)
		return (DER_ERROR(INVALID_INPUT));

	(void)content;
	(void)cont_nbits;

	return (SSL_OK);
}
