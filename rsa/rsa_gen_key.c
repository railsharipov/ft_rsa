#include <ssl/ssl.h>
#include <ssl/asn.h>
#include <ssl/base64.h>
#include <ssl/der.h>
#include <ssl/pem.h>
#include <ssl/rsa.h>
#include <ssl/rand.h>
#include <ssl/map.h>
#include <util/bnum.h>
#include <util/parser.h>
#include <libft/node.h>

static t_rsa	*__items;

static void	__gen_prime(t_num *prime, int keysize)
{
	uint64_t	mask;
	int			idx;

	mask = (1ULL<<((keysize-1)%BNUM_DIGIT_BIT+1))-1;

	do {
		bnum_set_dig_u(prime, 0u);
		prime->len = keysize/BNUM_DIGIT_BIT + (int)(keysize%BNUM_DIGIT_BIT!=0);

		for (idx = 0; idx < prime->len; idx++)
			prime->val[idx] = BNUM_MAX_VAL & rand_mtw_extract();

		while (!(prime->val[prime->len-1] & ((mask>>1)+1)))
			prime->val[prime->len-1] = BNUM_MAX_VAL & rand_mtw_extract();

		prime->val[prime->len-1] &= mask;

		for (; idx < BNUM_MAX_DIG; idx++)
			prime->val[idx] = 0;

	} while (!bnum_prime_test(prime, keysize, RM_TRIALS, SSL_TRUE));
}

static void	__get_primes(int modsize, uint64_t seed)
{
	int	keysize;

	bnum_set_dig_u(__items->version, 0);

	rand_mtw_init(seed);
	keysize = modsize / 2;

	__gen_prime(__items->prime1, keysize);
	__gen_prime(__items->prime2, modsize - keysize);

	bnum_mul(__items->prime1, __items->prime2, __items->modulus);
	bnum_invmod(__items->prime2, __items->prime1, __items->coeff);
}

static void	__get_exponents(void)
{
	t_num	t1, t2, t3;

	bnum_init_multi(&t1, &t2, &t3, NULL);

	bnum_sub_dig(__items->prime1, 1, &t1);
	bnum_sub_dig(__items->prime2, 1, &t2);
	bnum_lcm(&t1, &t2, &t3);
	bnum_set_dig_u(__items->pubexp, RSA_EXPPUB);
	bnum_invmod(__items->pubexp, &t3, __items->privexp);
	bnum_divmod(__items->privexp, &t1, NULL, __items->exponent1);
	bnum_divmod(__items->privexp, &t2, NULL, __items->exponent2);

	bnum_clear_multi(&t1, &t2, &t3, NULL);
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
		return (RSA_ERROR(INVALID_INPUT_ERROR));

	if (modsize < 64)
		return (RSA_ERROR("invalid rsa key size"));

	if (NULL == (pkey_tree = asn_tree(MAP_RSA_PRIVATE_KEY)))
		res = SSL_FAIL;

	else if (SSL_OK != rsa_key_items(pkey_tree, &__items))
		res = SSL_FAIL;

	else if (SSL_OK != rand_fseed(&seed, frand))
		res = SSL_FAIL;

	if (SSL_OK != res)
	{
		SSL_FREE(pkey_tree);
		return (RSA_ERROR("failed to generate rsa key"));
	}

	__get_primes(modsize, seed);
	__get_exponents();

	*asn_pkey = pkey_tree;

	return (SSL_OK);
}
