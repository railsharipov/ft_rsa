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

	if (lmbit_num(__items->pubexp) >= 256)
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

	if (lmbit_num(__items->privexp) <= lmbit_num(__items->modulus) / 2)
		res = SSL_FAIL;

	init_num_multi(&p1, &q1, &lcm, &ed, &edmod, NULL);

	sub_dig(__items->prime1, 1, &p1);
	sub_dig(__items->prime2, 1, &q1);
	lcm_num(&p1, &q1, &lcm);
	mul_num(__items->pubexp, __items->privexp, &ed);
	divmod_num(&ed, &lcm, NULL, &edmod);

	if (compare_num(__items->privexp, &lcm) >= 0)
		res = SSL_FAIL;
	else if (!BNUM_ONE(&edmod))
		res = SSL_FAIL;

	clear_num_multi(&p1, &q1, &lcm, &ed, &edmod, NULL);

	if (SSL_OK != res)
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__check_crt_comps(void)
{
	t_num	p1, q1;
	t_num	mul, mod;
	int		res;

	init_num_multi(&p1, &q1, &mul, &mod, &res, NULL);

	sub_dig(__items->prime1, 1, &p1);
	sub_dig(__items->prime2, 1, &q1);

	res = SSL_OK;

	if ((compare_dig(__items->exponent1, 1) <= 0)
		|| (compare_num(__items->exponent1, &p1) >= 0))
	{
		res = SSL_FAIL;
	}
	else if ((compare_dig(__items->exponent2, 1) <= 0)
		|| (compare_num(__items->exponent2, &q1) >= 0))
	{
		res = SSL_FAIL;
	}
	else if ((compare_dig(__items->coeff, 1) <= 0)
		|| (compare_num(__items->coeff, __items->prime1) >= 0))
	{
		res = SSL_FAIL;
	}

	mul_num(__items->exponent1, __items->pubexp, &mul);
	divmod_num(&mul, &p1, NULL, &mod);

	if (compare_dig(&mod, 1))
	{
		res = SSL_FAIL;
	}

	mul_num(__items->exponent2, __items->pubexp, &mul);
	divmod_num(&mul, &q1, NULL, &mod);

	if (compare_dig(&mod, 1))
	{
		res = SSL_FAIL;
	}

	mul_num(__items->coeff, __items->prime2, &mul);
	divmod_num(&mul, __items->prime1, NULL, &mod);

	if (compare_dig(&mod, 1))
	{
		res = SSL_FAIL;
	}

	clear_num_multi(&p1, &q1, &mul, &mod, &res, NULL);

	if (SSL_OK != res)
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__check_modulus(void)
{
	t_num	tmod;
	int		res;

	res = SSL_OK;

	init_num(&tmod);
	mul_num(__items->prime1, __items->prime2, &tmod);

	if (compare_num(__items->modulus, &tmod))
		res = SSL_FAIL;

	clear_num(&tmod);

	if (SSL_OK != res)
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__check_prime(t_num *prime)
{
	t_num	gcd, p1;
	int		res;

	res = SSL_OK;

	init_num_multi(&gcd, &p1, NULL);

	if (!prime_test(prime, lmbit_num(prime), RM_TRIALS, SSL_FALSE))
		res = SSL_FAIL;

	sub_dig(prime, 1, &p1);
	gcd_num(&p1, __items->pubexp, &gcd);

	if (compare_dig(&gcd, 1))
		res = SSL_FAIL;

	clear_num_multi(&gcd, &p1, NULL);

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
