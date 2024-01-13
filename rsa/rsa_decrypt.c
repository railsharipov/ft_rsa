#include <ssl/ssl.h>
#include <ssl/error.h>
#include <ssl/asn.h>
#include <ssl/rsa.h>
#include <ssl/rand.h>
#include <util/bnum.h>
#include <libft/node.h>

static t_rsa	*__items;

// EME PKCS1 v1.5 decoding
// Split PS ostring, message cstring and three intermediate octets
static int	__eme_pkcs1_v1_5_split(unsigned char **octets, int *osize)
{
	unsigned char	*optr;
	unsigned char	*mes;
	size_t			messize;
	int				check;
	int				ix;

	optr = *octets;
	ix = 0;

	if ((optr[ix++] != 0x00) || (optr[ix++] != 0x02))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	check = 0;

	while (ix < *osize)
	{
		if (optr[ix++] == 0x00)
		{
			check = 1;
			break ;
		}
	}
	if (check != 1)
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	messize = *osize - ix;

	if (messize == 0)
	{
		*octets = (unsigned char *)ft_strdup("");
		*osize = 0;
	}
	else
	{
		SSL_ALLOC(mes, messize);
		ft_memcpy(mes, optr + ix, messize);
		*octets = mes;
		*osize = messize;
	}
	SSL_FREE(optr);

	return (SSL_OK);
}

/* RSA decryption primitive (... refer to RFC 3447)
*
* If the first form (n, d) of K is used, let m = c^d mod n.
*
* If the second form (p, q, dP, dQ, qInv) of K is used,
* proceed as follows:
*
* 1. Let m_1 = c^dP mod p and m_2 = c^dQ mod q.
*
* 2. Let h = (m_1 - m_2) * qInv mod p.
*
* 3. Let m = m_2 + q * h.
*/

static int	__decrypt_prim(t_num *ciph_rep, t_num *mes_rep)
{
	if (bnum_cmp_u(ciph_rep, __items->modulus) >= 0)
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	bnum_m_powmod(ciph_rep, __items->privexp, __items->modulus, mes_rep);

	return (SSL_OK);
}

// RSA decryption scheme
static int	__decrypt(
	const char *ciph, size_t ciphsize, char **mes, size_t *messize)
{
	unsigned char	*octets;
	int				osize;
	t_num			mes_rep, ciph_rep;
	int				res;

	osize = NBITS_TO_NBYTES(__items->keysize);

	bnum_init_multi(&mes_rep, &ciph_rep, NULL);

	octets = NULL;
	res = SSL_OK;

	if ((ciphsize > osize) || (ciphsize < 11))
		res = SSL_FAIL;

	else if (SSL_OK != rsa_os2i(&ciph_rep, (unsigned char *)ciph, ciphsize))
		res = SSL_FAIL;

	else if (SSL_OK != __decrypt_prim(&ciph_rep, &mes_rep))
		res = SSL_FAIL;

	else if (SSL_OK != rsa_i2os(&mes_rep, &octets, osize))
		res = SSL_FAIL;

	else if (SSL_OK != __eme_pkcs1_v1_5_split(&octets, &osize))
		res = SSL_FAIL;

	if (SSL_OK == res)
	{
		*mes = (char *)octets;
		*messize = osize;
	}

	bnum_clear_multi(&mes_rep, &ciph_rep, NULL);
	SSL_FREE(octets);

	if (SSL_OK != res)
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

int rsa_decrypt(t_ostring *ciph, t_ostring *mes, t_node *asn_key)
{
	int	keysize;

	if ((NULL == ciph) || (NULL == ciph->content)
		|| (NULL == mes) || (NULL == asn_key))
	{
		return (RSA_ERROR(INVALID_INPUT));
	}
	mes->content = NULL;

	if (ft_strcmp(asn_key->key, "RSA_PRIVATE_KEY"))
	{
		return (RSA_ERROR(INVALID_RSA_KEY_TYPE));
	}
	if (SSL_OK != rsa_key_items(asn_key, &__items))
	{
		return (RSA_ERROR(INVALID_RSA_KEY));
	}
	if (SSL_OK != __decrypt(
		ciph->content, ciph->size, &(mes->content), &(mes->size)))
	{
		return (RSA_ERROR(INVALID_RSA_KEY));
	}
	return (SSL_OK);
}
