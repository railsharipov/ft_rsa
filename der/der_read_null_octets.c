#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

int  der_read_null_octets(t_iasn *item, unsigned char *derenc, size_t dersize)
{
	if (NULL == item || NULL == derenc)
		return (DER_ERROR(INVALID_INPUT));

	while (dersize-- != 0)
		if (*derenc++ != 0)
			return (DER_ERROR(INVALID_DER_ENCODING));

	item->content = NULL;
	item->bitsize = 0;

	return (SSL_OK);
}
