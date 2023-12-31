#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_rsa.h>
#include <ssl_asn.h>
#include <bnum.h>

static t_rsa	*__items;

static int	__check_pubexp(void)
{
	if (BNUM_EVEN(__items->pubexp))
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	if (bnum_lmbit(__items->pubexp) >= 256)
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__check_privexp(void)
{
	t_num	p1, q1;
	t_num	lcm;
	t_num	ed, edmod;
	int		res;

	res = SSL_OK;

	if (BNUM_EVEN(__items->privexp))
		res = SSL_FAIL;

	if (bnum_lmbit(__items->privexp) <= bnum_lmbit(__items->modulus) / 2)
		res = SSL_FAIL;

	bnum_init_multi(&p1, &q1, &lcm, &ed, &edmod, NULL);

	bnum_sub_dig(__items->prime1, 1, &p1);
	bnum_sub_dig(__items->prime2, 1, &q1);
	bnum_lcm(&p1, &q1, &lcm);
	bnum_mul(__items->pubexp, __items->privexp, &ed);
	bnum_divmod(&ed, &lcm, NULL, &edmod);

	if (bnum_cmp(__items->privexp, &lcm) >= 0)
		res = SSL_FAIL;
	else if (!BNUM_ONE(&edmod))
		res = SSL_FAIL;

	bnum_clear_multi(&p1, &q1, &lcm, &ed, &edmod, NULL);

	if (SSL_OK != res)
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__check_crt_comps(void)
{
	t_num	p1, q1;
	t_num	mul, mod;
	int		res;

	bnum_init_multi(&p1, &q1, &mul, &mod, &res, NULL);

	bnum_sub_dig(__items->prime1, 1, &p1);
	bnum_sub_dig(__items->prime2, 1, &q1);

	res = SSL_OK;

	if ((bnum_cmp_dig(__items->exponent1, 1) <= 0)
		|| (bnum_cmp(__items->exponent1, &p1) >= 0))
	{
		res = SSL_FAIL;
	}
	else if ((bnum_cmp_dig(__items->exponent2, 1) <= 0)
		|| (bnum_cmp(__items->exponent2, &q1) >= 0))
	{
		res = SSL_FAIL;
	}
	else if ((bnum_cmp_dig(__items->coeff, 1) <= 0)
		|| (bnum_cmp(__items->coeff, __items->prime1) >= 0))
	{
		res = SSL_FAIL;
	}

	bnum_mul(__items->exponent1, __items->pubexp, &mul);
	bnum_divmod(&mul, &p1, NULL, &mod);

	if (bnum_cmp_dig(&mod, 1))
	{
		res = SSL_FAIL;
	}

	bnum_mul(__items->exponent2, __items->pubexp, &mul);
	bnum_divmod(&mul, &q1, NULL, &mod);

	if (bnum_cmp_dig(&mod, 1))
	{
		res = SSL_FAIL;
	}

	bnum_mul(__items->coeff, __items->prime2, &mul);
	bnum_divmod(&mul, __items->prime1, NULL, &mod);

	if (bnum_cmp_dig(&mod, 1))
	{
		res = SSL_FAIL;
	}

	bnum_clear_multi(&p1, &q1, &mul, &mod, &res, NULL);

	if (SSL_OK != res)
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__check_modulus(void)
{
	t_num	tmod;
	int		res;

	res = SSL_OK;

	bnum_init(&tmod);
	bnum_mul(__items->prime1, __items->prime2, &tmod);

	if (bnum_cmp(__items->modulus, &tmod))
		res = SSL_FAIL;

	bnum_clear(&tmod);

	if (SSL_OK != res)
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__check_prime(t_num *prime)
{
	t_num	gcd, p1;
	int		res;

	res = SSL_OK;

	bnum_init_multi(&gcd, &p1, NULL);

	if (!bnum_prime_test(prime, bnum_lmbit(prime), RM_TRIALS, SSL_FALSE))
		res = SSL_FAIL;

	bnum_sub_dig(prime, 1, &p1);
	bnum_gcd(&p1, __items->pubexp, &gcd);

	if (bnum_cmp_dig(&gcd, 1))
		res = SSL_FAIL;

	bnum_clear_multi(&gcd, &p1, NULL);

	if (SSL_OK != res)
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

int	rsa_check(t_node *asn_key)
{
	int	ret;

	ret = SSL_OK;

	if (NULL == asn_key)
		return (RSA_ERROR(INVALID_INPUT));

	if (ft_strcmp(asn_key->key, "RSA_PRIVATE_KEY"))
		return (RSA_ERROR(INVALID_RSA_KEY_TYPE));

	if (SSL_OK != rsa_key_items(asn_key, &__items))
		return (RSA_ERROR(INVALID_RSA_KEY));

	ret |= __check_pubexp();
	ret |= __check_modulus();
	ret |= __check_prime(__items->prime1);
	ret |= __check_prime(__items->prime2);
	ret |= __check_privexp();
	ret |= __check_crt_comps();

	if (SSL_OK != ret)
		ret = RSA_ERROR(INVALID_RSA_KEY);

	SSL_FREE(__items);

	return (ret);
}
