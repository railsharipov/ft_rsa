#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_rsa.h>
#include <bnum.h>

int rsa_os2i(t_num *num, unsigned char *octets, size_t osize)
{
	if (BNUM_DIGIT_BIT * BNUM_MAX_DIG < CHAR_BIT * osize)
	{
		return (RSA_ERROR(INVALID_OCTET_STRING_SIZE));
	}
	bytes_to_num(num, (char *)octets, osize);

	return (SSL_OK);
}

int rsa_i2os(t_num *num, unsigned char **octets, size_t osize)
{
	char	*nstr;
	int		nsize;

	nsize = CEIL(lmbit_num(num), 8) / 8;

	if (nsize > CHAR_BIT * osize)
	{
		return (RSA_ERROR(INVALID_INTEGER_SIZE));
	}
	SSL_ALLOC(*octets, osize);
	nstr = stringify_num(num);
	ft_memcpy(*octets + osize-nsize, nstr, nsize);
	SSL_FREE(nstr);

	return (SSL_OK);
}
