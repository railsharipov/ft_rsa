#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

static int	__is_short_len(unsigned char *);

int	der_read_len_size(size_t *lensize, unsigned char *derenc, size_t dersize)
{
	unsigned char	*octets;
	size_t			osize;

	if (NULL == lensize || NULL == derenc)
		return DER_ERROR(INVALID_INPUT);

	if (dersize == 0)
		return (DER_ERROR(INVALID_DER_ENCODING));

	*lensize = 0;

	if (__is_short_len(derenc))
		*lensize = 1;
	else
		*lensize = *derenc & 0x7F;

	if (*lensize > dersize || *lensize <= 1 || *lensize > 4)
		return (DER_ERROR(INVALID_DER_ENCODING));

	return (SSL_OK);
}

static int	__is_short_len(unsigned char *derenc)
{
	return (ASN_LEN_SHORT == (*derenc & 0x80));
}
