#include <ft_ssl.h>
#include <ssl_rsa.h>
#include <bnum.h>

int rsa_os2i(t_num *num, unsigned char *octets, size_t osize)
{
	if (BNUM_DIGIT_BIT * BNUM_MAX_DIG < CHAR_BIT * osize)
	{
		return (SSL_ERROR("octet string too large"));
	}
	bytes_to_num(num, (char *)octets, osize);

	return (SSL_OK);
}

int rsa_i2os(t_num *num, unsigned char **octets, size_t osize)
{
	char	*nstr;
	int		nlen;

	nlen = CEIL(lmbit_num(num), 8) / 8;

	if (nlen > CHAR_BIT * osize)
	{
		return (SSL_ERROR("integer too large"));
	}
	SSL_ALLOC(*octets, osize);
	nstr = stringify_num(num);
	ft_memcpy(*octets + osize-nlen, nstr, nlen);

	return (SSL_OK);
}
