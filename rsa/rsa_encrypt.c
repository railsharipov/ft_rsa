#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_rsa.h>
#include <ssl_rand.h>
#include <bnum.h>

static t_rsa	*__items;

// EME PKCS1 v1.5 encoding
// Generate PS ostring with non-zero pseudo-random octets
static int	__eme_pkcs1_v1_5_ps(
	unsigned char **ps, size_t *psize, int modsize, size_t messize)
{
	uint64_t	seed;
	uint64_t	prand;
	size_t		ix;

	if (SSL_OK != rand_useed(&seed, 8))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	rand_mtw_init(seed);

	*psize = modsize - messize - 3;
	SSL_ALLOC(*ps, *psize);

	ix = 0;
	while (ix < *psize)
	{
		prand = rand_mtw_extract();
		while ((ix < *psize) && ((prand & 0xFF) != 0))
		{
			(*ps)[ix++] = prand & 0xFF;
			prand >>= 8;
		}
	}
	return (SSL_OK);
}

// Concatenate PS ostring, message and three intermediate octets
static void	__eme_pkcs1_v1_5_concat(
	unsigned char **octets, size_t *osize, const char *mes, size_t messize)
{
	const unsigned char	EME_OCTET_0X00 = 0x00;
	const unsigned char	EME_OCTET_0X02 = 0x02;
	unsigned char			*enc;
	size_t			encsize;

	encsize = *osize + messize + 3;
	SSL_ALLOC(enc, encsize);

	enc[0] = EME_OCTET_0X00;
	enc[1] = EME_OCTET_0X02;
	ft_memcpy(enc + 2, *octets, *osize);
	enc[*osize + 2] = EME_OCTET_0X00;
	ft_memcpy(enc + *osize + 3, mes, messize);

	SSL_FREE(*octets);
	*octets = enc;
	*osize = encsize;
}

// RSA encryption primitive
static int  __encrypt_prim(t_num *mes_rep, t_num *ciph_rep)
{
	if (compare_num_u(mes_rep, __items->modulus) >= 0)
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	m_powmod_num(mes_rep, __items->pubexp, __items->modulus, ciph_rep);

	return (SSL_OK);
}

// RSA encryption scheme
static int  __encrypt(
	const char *mes, size_t messize, char **ciph, size_t *ciphsize)
{
	unsigned char	*octets;
	size_t	osize;
	t_num	mes_rep, ciph_rep;
	int		modsize;
	int		ret;

	modsize = TO_NUM_BYTES(__items->keysize);

	if (messize > modsize-11)
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	if (SSL_OK != __eme_pkcs1_v1_5_ps(&octets, &osize, modsize, messize))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	__eme_pkcs1_v1_5_concat(&octets, &osize, mes, messize);

	ret = rsa_os2i(&mes_rep, octets, osize);
	SSL_FREE(octets);

	if (SSL_OK != ret)
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	if (SSL_OK != __encrypt_prim(&mes_rep, &ciph_rep))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	if (SSL_OK != rsa_i2os(&ciph_rep, (unsigned char **)ciph, modsize))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	*ciphsize = modsize;

	return (SSL_OK);
}

int rsa_encrypt(t_ostring *mes, t_ostring *ciph, t_node *asn_key)
{
	int	keysize;

	if ((NULL == mes) || (NULL == mes->content)
		|| (NULL == ciph) || (NULL == asn_key))
	{
		return (RSA_ERROR(INVALID_INPUT));
	}
	ciph->content = NULL;

	if (SSL_OK != rsa_key_items(asn_key, &__items))
	{
		return (RSA_ERROR(INVALID_RSA_KEY));
	}
	if (SSL_OK != __encrypt(
		mes->content, mes->size, &(ciph->content), &(ciph->size)))
	{
		return (RSA_ERROR(INVALID_INPUT));
	}
	return (SSL_OK);
}
