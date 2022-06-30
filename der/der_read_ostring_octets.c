#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

int  der_read_ostring_octets(t_iasn *item, unsigned char *derenc, size_t dersize)
{
	if (NULL == item || NULL == derenc)
		return (DER_ERROR(INVALID_INPUT));

	SSL_ALLOC(item->content, dersize);
	ft_memcpy(item->content, derenc, dersize);
	item->bitsize = NBYTES_TO_NBITS(dersize);

	return (SSL_OK);
}
