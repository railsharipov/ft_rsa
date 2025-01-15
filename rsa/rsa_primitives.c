#include <common.h>
#include <rsa.h>
#include <bnum.h>

int rsa_os2i(t_num *num, unsigned char *octets, size_t osize)
{
	bnum_from_bytes_u(num, (char *)octets, osize);

	return (SSL_OK);
}

int rsa_i2os(t_num *num, unsigned char **octets, size_t osize)
{
	char	*nstr;
	size_t	nsize;

	SSL_ALLOC(*octets, osize);
	bnum_to_bytes_u(num, &nstr, &nsize);

	if (nsize > CHAR_BIT * osize) {
		SSL_FREE(nstr);
		RSA_LOG(ERROR, "invalid octet buffer size");
		return (SSL_ERR);
	}

	ft_memcpy(*octets + osize-nsize, nstr, nsize);
	SSL_FREE(nstr);

	return (SSL_OK);
}
