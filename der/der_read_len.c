#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

static int	__is_short_len(unsigned char *);
static int	__read_len_short(size_t *, size_t *, unsigned char *, size_t);
static int	__read_len_long(size_t *, size_t *, unsigned char *, size_t);

int	der_read_len(unsigned char **derenc, size_t *dersize, size_t *len)
{
	unsigned char	*octets;
	size_t			osize;

	if (NULL == derenc || NULL == *derenc || NULL == len)
		return DER_ERROR(INVALID_INPUT);

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
	// else
	// {
	// 	if (SSL_OK != __read_len_long(&octets, &osize, len))
	// 		return (DER_ERROR(UNSPECIFIED_ERROR));
	// }

	*derenc = octets;
	*dersize = osize;

	return (SSL_OK);
}

int	der_read_len_octets(
	size_t *len, size_t *rsize, unsigned char *derenc, size_t dersize)
{
	unsigned char	*octets;
	size_t			osize;

	if (NULL == len || NULL == rsize || NULL == derenc)
		return DER_ERROR(INVALID_INPUT);

	if (dersize == 0)
		return (DER_ERROR(INVALID_DER_ENCODING));

	*rsize = 0;

	if (__is_short_len(octets))
	{
		if (SSL_OK != __read_len_short(len, rsize, derenc, dersize))
			return (DER_ERROR(UNSPECIFIED_ERROR));
	}
	else
	{
		if (SSL_OK != __read_len_long(len, rsize, derenc, dersize))
			return (DER_ERROR(UNSPECIFIED_ERROR));
	}

	return (SSL_OK);
}

static int	__is_short_len(unsigned char *octets)
{
	return (ASN_LEN_SHORT == (*octets & 0x80));
}

static int	__read_len_short(
	size_t *len, size_t *rsize, unsigned char *derenc, size_t dersize)
{
	*len = *derenc & 0x7F;
	*rsize += 1;

	return (SSL_OK);
}

static int	__read_len_long(
	size_t *len, size_t *rsize, unsigned char *derenc, size_t dersize)
{
	uint32_t	octets;
	int			osize;

	osize = *derenc & 0x7F;
	derenc++;
	dersize--;
	*rsize += 1;

	if (osize > dersize || osize <= 1 || osize > 4)
		return (DER_ERROR(INVALID_DER_ENCODING));

	octets = 0;

	while (osize-- > 0)
	{
		octets <<= CHAR_BIT;
		octets |= *derenc++;
		*rsize += 1;
	}

	*len = (size_t)octets;

	return (SSL_OK);
}
