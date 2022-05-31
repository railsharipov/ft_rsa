#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_rsa.h>
#include <ssl_asn.h>
#include <bnum.h>

static t_rsa	*__items;

static int	__check_pubexp(void)
{
	if (BNUM_EVEN(__items->pubexp))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	if (lmbit_num(__items->pubexp) >= 256)
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	return (SSL_OK);
}

static int	__check_privexp(void)
{
	t_num	p1, q1;
	t_num	lcm;
	t_num	ed, edmod;

	if (BNUM_EVEN(__items->privexp))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	if (lmbit_num(__items->privexp) <= lmbit_num(__items->modulus) / 2)
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	sub_num_d(__items->prime1, 1, &p1);
	sub_num_d(__items->prime2, 1, &q1);
	lcm_num(&p1, &q1, &lcm);
	mul_num(__items->pubexp, __items->privexp, &ed);
	divmod_num(&ed, &lcm, NULL, &edmod);

	if (compare_num(__items->privexp, &lcm) >= 0)
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	if (!BNUM_ONE(&edmod))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	return (SSL_OK);
}

static int	__check_crt_comps(void)
{
	t_num	p1, q1;
	t_num	mul, mod;

	sub_num_d(__items->prime1, 1, &p1);
	sub_num_d(__items->prime2, 1, &q1);

	if ((compare_num_d(__items->exponent1, 1) <= 0)
		|| (compare_num(__items->exponent1, &p1) >= 0))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	if ((compare_num_d(__items->exponent2, 1) <= 0)
		|| (compare_num(__items->exponent2, &q1) >= 0))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	if ((compare_num_d(__items->coeff, 1) <= 0)
		|| (compare_num(__items->coeff, __items->prime1) >= 0))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}

	mul_num(__items->exponent1, __items->pubexp, &mul);
	divmod_num(&mul, &p1, NULL, &mod);

	if (compare_num_d(&mod, 1))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}

	mul_num(__items->exponent2, __items->pubexp, &mul);
	divmod_num(&mul, &q1, NULL, &mod);

	if (compare_num_d(&mod, 1))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}

	mul_num(__items->coeff, __items->prime2, &mul);
	divmod_num(&mul, __items->prime1, NULL, &mod);

	if (compare_num_d(&mod, 1))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	return (SSL_OK);
}

static int	__check_modulus(void)
{
	t_num	tmod;

	mul_num(__items->prime1, __items->prime2, &tmod);

	if (compare_num(__items->modulus, &tmod))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	return (SSL_OK);
}

static int	__check_prime(t_num *prime)
{
	t_num	gcd;
	t_num	p1;

	if (!prime_test(prime, lmbit_num(prime), RM_TRIALS, SSL_FALSE))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	sub_num_d(prime, 1, &p1);
	gcd_num(&p1, __items->pubexp, &gcd);

	if (compare_num_d(&gcd, 1))
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	return (SSL_OK);
}

int	rsa_check(t_node *asn_key)
{
	int	ret;

	ret = SSL_OK;

	if (NULL == asn_key)
	{
		return (RSA_ERROR(INVALID_INPUT));
	}
	if (ft_strcmp(asn_key->key, "RSA_PRIVATE_KEY"))
	{
		return (RSA_ERROR(INVALID_RSA_KEY_TYPE));
	}
	if (SSL_OK != rsa_key_items(asn_key, &__items))
	{
		return (RSA_ERROR(INVALID_RSA_KEY));
	}
	ret |= __check_pubexp();
	ret |= __check_modulus();
	ret |= __check_prime(__items->prime1);
	ret |= __check_prime(__items->prime2);
	ret |= __check_privexp();
	ret |= __check_crt_comps();

	if (SSL_OK != ret)
	{
		ret = RSA_ERROR(INVALID_RSA_KEY);
	}
	SSL_FREE(__items);

	return (ret);
}
