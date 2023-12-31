#include <ft_ssl.h>
#include <ssl_error.h>
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

static void	__gen_prime(t_num *prime, int keysize)
{
	uint64_t	mask;
	int			idx;

	mask = (1ULL<<((keysize-1)%BNUM_DIGIT_BIT+1))-1;

	do {
		reset_num(prime);
		prime->len = keysize/BNUM_DIGIT_BIT + (int)(keysize%BNUM_DIGIT_BIT!=0);

		for (idx = 0; idx < prime->len; idx++)
			prime->val[idx] = BNUM_MAX_VAL & rand_mtw_extract();

		while (!(prime->val[prime->len-1] & ((mask>>1)+1)))
			prime->val[prime->len-1] = BNUM_MAX_VAL & rand_mtw_extract();

		prime->val[prime->len-1] &= mask;

		for (; idx < BNUM_MAX_DIG; idx++)
			prime->val[idx] = 0;

	} while (!prime_test(prime, keysize, RM_TRIALS, SSL_TRUE));
}

static void	__get_primes(int modsize, uint64_t seed)
{
	int	keysize;

	set_dig_u(__items->version, 0);

	rand_mtw_init(seed);
	keysize = modsize / 2;

	__gen_prime(__items->prime1, keysize);
	__gen_prime(__items->prime2, modsize - keysize);

	mul_num(__items->prime1, __items->prime2, __items->modulus);
	invmod_num(__items->prime2, __items->prime1, __items->coeff);
}

static void	__get_exponents(void)
{
	t_num	t1, t2, t3;

	init_num_multi(&t1, &t2, &t3, NULL);

	sub_dig(__items->prime1, 1, &t1);
	sub_dig(__items->prime2, 1, &t2);
	lcm_num(&t1, &t2, &t3);
	set_dig_u(__items->pubexp, RSA_EXPPUB);
	invmod_num(__items->pubexp, &t3, __items->privexp);
	divmod_num(__items->privexp, &t1, NULL, __items->exponent1);
	divmod_num(__items->privexp, &t2, NULL, __items->exponent2);

	clear_num_multi(&t1, &t2, &t3, NULL);
}

int	rsa_gen_key(t_node **asn_pkey, int modsize, const char *frand)
{
	t_node		*pkey_tree;
	uint64_t	seed;
	int			res;

	__items = NULL;
	pkey_tree = NULL;
	res = SSL_OK;

	if (NULL == asn_pkey)
		return (RSA_ERROR(INVALID_INPUT));

	if (modsize < 64)
		return (RSA_ERROR(INVALID_RSA_KEY_SIZE));

	if (NULL == (pkey_tree = asn_tree(MAP_RSA_PRIVATE_KEY)))
		res = SSL_FAIL;

	else if (SSL_OK != rsa_key_items(pkey_tree, &__items))
		res = SSL_FAIL;

	else if (SSL_OK != rand_fseed(&seed, frand))
		res = SSL_FAIL;

	if (SSL_OK != res)
	{
		SSL_FREE(pkey_tree);
		return (RSA_ERROR(FAILED_RSA_KEY_GENERATION));
	}

	__get_primes(modsize, seed);
	__get_exponents();

	*asn_pkey = pkey_tree;

	return (SSL_OK);
}
