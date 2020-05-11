#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_rsa.h>
#include <ssl_rand.h>
#include <bnum.h>

static t_rsa	*__items;

// EME PKCS1 v1.5 encoding
// Generate PS ostring with non-zero pseudo-random octets
static int	__eme_pkcs1_v1_5_ps(
	uint8_t **ps, size_t *psize, int modsize, size_t messize)
{
	uint64_t	seed;
	uint64_t	prand;
	size_t		ix;

	if (SSL_OK != rand_useed(&seed, 8))
	{
		return (SSL_ERROR("rand error"));
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
	uint8_t **octets, size_t *osize, const char *mes, size_t messize)
{
	const uint8_t	EME_OCTET_0X00 = 0x00;
	const uint8_t	EME_OCTET_0X02 = 0x02;
	uint8_t			*enc;
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
		return (SSL_ERROR("message representative out of range"));
	}
	m_powmod_num(mes_rep, __items->pubexp, __items->modulus, ciph_rep);

	return (SSL_OK);
}

// RSA encryption scheme
static int  __encrypt(
	const char *mes, size_t messize, char **ciph, size_t *ciphsize)
{
	uint8_t	*octets;
	size_t	osize;
	t_num	mes_rep, ciph_rep;
	int		modsize;
	int		ret;

	modsize = CEIL(__items->keysize, 8) / 8;

	if (messize > modsize-11)
	{
		return (SSL_ERROR("message too long"));
	}
	if (SSL_OK != __eme_pkcs1_v1_5_ps(&octets, &osize, modsize, messize))
	{
		return (SSL_ERR);
	}
	__eme_pkcs1_v1_5_concat(&octets, &osize, mes, messize);

	ret = rsa_os2i(&mes_rep, octets, osize);
	SSL_FREE(octets);

	if (SSL_OK != ret)
	{
		return (SSL_ERR);
	}
	if (SSL_OK != __encrypt_prim(&mes_rep, &ciph_rep))
	{
		return (SSL_ERR);
	}
	if (SSL_OK != rsa_i2os(&ciph_rep, ciph, modsize))
	{
		return (SSL_ERR);
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
		return (SSL_ERROR("invalid input"));
	}
	ciph->content = NULL;

	if (SSL_OK != rsa_key_items(asn_key, &__items))
	{
		return (SSL_ERROR("bad rsa key"));
	}
	if (SSL_OK != __encrypt(
		mes->content, mes->size, &(ciph->content), &(ciph->size)))
	{
		return (SSL_ERROR("rsa encrypt error"));
	}
	return (SSL_OK);
}
