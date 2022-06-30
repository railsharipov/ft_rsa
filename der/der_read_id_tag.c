#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_der.h>
#include <ssl_asn.h>

int	der_read_id_tag(
	uint8_t *tag, unsigned char *derenc, size_t dersize)
{
	if (NULL == tag || NULL == derenc)
		return (DER_ERROR(INVALID_INPUT));

	if (dersize == 0)
		return (DER_ERROR(INVALID_DER_ENCODING));

	*tag = *derenc;

	return (SSL_OK);
}
