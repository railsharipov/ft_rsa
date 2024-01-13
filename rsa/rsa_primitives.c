#include <ssl/ssl.h>
#include <ssl/error.h>
#include <ssl/rsa.h>
#include <util/bnum.h>

int rsa_os2i(t_num *num, unsigned char *octets, size_t osize)
{
	if (BNUM_DIGIT_BIT * BNUM_MAX_DIG < CHAR_BIT * osize)
	{
		return (RSA_ERROR(INVALID_OCTET_STRING_SIZE));
	}
	bnum_from_bytes_u(num, (char *)octets, osize);

	return (SSL_OK);
}

int rsa_i2os(t_num *num, unsigned char **octets, size_t osize)
{
	char	*nstr;
	size_t	nsize;

	if (nsize > CHAR_BIT * osize)
		return (RSA_ERROR(INVALID_INTEGER_SIZE));

	SSL_ALLOC(*octets, osize);
	bnum_to_bytes_u(num, &nstr, &nsize);
	ft_memcpy(*octets + osize-nsize, nstr, nsize);
	SSL_FREE(nstr);

	return (SSL_OK);
}
