#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

static int	__is_short_len(unsigned char *);
static void	__read_len_short(size_t *, unsigned char *, size_t);
static void	__read_len_long(size_t *, unsigned char *, size_t);

int	der_read_len_octets(size_t *len, unsigned char *derenc, size_t dersize)
{
	if (NULL == len || NULL == derenc)
		return DER_ERROR(INVALID_INPUT);

	if (dersize == 0 || dersize > 4)
		return (DER_ERROR(INVALID_DER_ENCODING));

	if (__is_short_len(derenc))
		__read_len_short(len, derenc, dersize);
	else
		__read_len_long(len, derenc, dersize);

	return (SSL_OK);
}

static int	__is_short_len(unsigned char *derenc)
{
	return (ASN_LEN_SHORT == (*derenc & 0x80));
}

static void	__read_len_short(size_t *len, unsigned char *derenc, size_t dersize)
{
	*len = *derenc & 0x7F;
}

static void	__read_len_long(size_t *len, unsigned char *derenc, size_t dersize)
{
	uint32_t	octets;

	octets = 0;

	while (dersize-- > 0)
	{
		octets <<= CHAR_BIT;
		octets |= *derenc++;
	}

	*len = (size_t)octets;
}