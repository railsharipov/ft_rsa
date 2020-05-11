#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_base64.h>
#include <ssl_der.h>
#include <ssl_pem.h>
#include <ssl_rsa.h>
#include <ssl_rand.h>
#include <ssl_map.h>
#include <bnum.h>
#include <parser.h>

static t_rsa	*__items;

static int	__gen_prime(t_num *prime, int keysize)
{
	uint64_t	mask;
	int			idx;

	mask = (1ULL<<((keysize-1)%BNUM_DIGIT_BIT+1))-1;

	do {
		init_num(prime);
		prime->len = keysize/BNUM_DIGIT_BIT + (int)(keysize%BNUM_DIGIT_BIT!=0);

		for (idx = 0; idx < prime->len; idx++)
		{
			prime->val[idx] = BNUM_MAX_VAL & rand_mtw_extract();
		}
		while (!(prime->val[prime->len-1] & ((mask>>1)+1)))
		{
			prime->val[prime->len-1] = BNUM_MAX_VAL & rand_mtw_extract();
		}
		prime->val[prime->len-1] &= mask;

		for (; idx < BNUM_MAX_DIG; idx++)
		{
			prime->val[idx] = 0;
		}
	} while (!prime_test(prime, keysize, RM_TRIALS, SSL_TRUE));

	return (SSL_OK);
}

static int	__get_primes(int modsize, const char *frand)
{
	uint64_t	seed;
	int			keysize;

	set_num(__items->version, 0);

	if (SSL_OK != rand_fseed(&seed, frand))
	{
		return (SSL_ERR);
	}
	rand_mtw_init(seed);
	keysize = modsize / 2;

	if (SSL_OK != __gen_prime(__items->prime1, keysize))
	{
		return (SSL_ERR);
	}
	if (SSL_OK != __gen_prime(__items->prime2, modsize - keysize))
	{
		return (SSL_ERR);
	}
	mul_num(__items->prime1, __items->prime2, __items->modulus);
	invmod_num(__items->prime2, __items->prime1, __items->coeff);

	return (SSL_OK);
}

static int	__get_exponents(void)
{
	t_num	t1;
	t_num	t2;
	t_num	t3;

	init_num(&t1);
	init_num(&t2);
	init_num(&t3);
	sub_num_d(__items->prime1, 1, &t1);
	sub_num_d(__items->prime2, 1, &t2);
	lcm_num(&t1, &t2, &t3);
	set_num(__items->pubexp, RSA_EXPPUB);
	invmod_num(__items->pubexp, &t3, __items->privexp);
	divmod_num(__items->privexp, &t1, NULL, __items->exponent1);
	divmod_num(__items->privexp, &t2, NULL, __items->exponent2);

	return (SSL_OK);
}

int	rsa_gen_key(t_node **asn_pkey, int modsize, const char *frand)
{
	int	ret;

	ret = SSL_OK;

	if (NULL == asn_pkey)
	{
		return (SSL_ERROR("invalid rsa gen input"));
	}
	if (modsize < 64)
	{
		return (SSL_ERROR("rsa key size too small"));
	}
	if (NULL == (*asn_pkey = asn_tree(MAP_RSA_PRIVATE_KEY)))
	{
		return (SSL_ERR);
	}
	if (SSL_OK != rsa_key_items(*asn_pkey, &__items))
	{
		ret = SSL_ERROR("rsa key generation failed");
	}
	else if (SSL_OK != __get_primes(modsize, frand))
	{
		ret = SSL_ERR;
	}
	else if (SSL_OK != __get_exponents())
	{
		ret = SSL_ERR;
	}
	if (SSL_OK != ret)
	{
		asn_tree_del(*asn_pkey);
		*asn_pkey = NULL;
	}
	SSL_FREE(__items);

	return (ret);
}
