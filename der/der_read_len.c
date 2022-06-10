#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

static int	__len_long(unsigned char **octets, size_t *osize, size_t *len);

int	der_read_len(unsigned char **derenc, size_t *dersize, size_t *len)
{
	unsigned char	*octets;
	size_t			osize;

	SSL_CHECK((NULL != derenc) && (NULL != *derenc));
	SSL_CHECK(NULL != dersize);
	SSL_CHECK(NULL != len);

	if (*dersize == 0)
		return (DER_ERROR(INVALID_DER_ENCODING));

	octets = (unsigned char *)(*derenc);
	osize = *dersize;
	*len = 0;

	if (ASN_LEN_SHORT == (*octets & (1<<7)))
	{
		*len = *octets & 0x7F;
		octets++;
		osize--;
	}
	else
	{
		if (SSL_OK != __len_long(&octets, &osize, len))
			return (DER_ERROR(UNSPECIFIED_ERROR));
	}

	*derenc = octets;
	*dersize = osize;

	return (SSL_OK);
}

static int	__len_long(unsigned char **octets, size_t *osize, size_t *len)
{
	uint32_t	len_octets;
	int			len_nbytes;

	len_nbytes = **octets & (~(1<<7));
	*octets = *octets + 1;
	*osize = *osize - 1;

	if (len_nbytes > *osize)
		return (DER_ERROR(INVALID_DER_ENCODING));

	len_octets = 0;
	while (len_nbytes-- > 0)
	{
		len_octets <<= 8;
		len_octets |= **octets;
		*octets = *octets + 1;
		*osize = *osize - 1;
	}

	*len = len_octets;

	return (SSL_OK);
}
