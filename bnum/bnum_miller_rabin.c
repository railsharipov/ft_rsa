#include <bnum.h>

int		bnum_miller_rabin(const t_num *num, const t_num *rnd)
{
	t_num		np, n1, e;
	int			idx, x;
	int			res;

	bnum_init_multi(&np, &n1, &e, NULL);

	bnum_sub_dig_u(num, 1, &n1);
	bnum_copy(&n1, &e);
	x = bnum_rmbit(&e);
	bnum_rsh_bit_inpl(&e, --x);
	bnum_m_powmod(rnd, &e, num, &np);

	res = BNUM_TRUE;

	if (BNUM_ONE(&np) || bnum_cmp_u(&np, &n1) == 0)
	{
		res = BNUM_TRUE;
	}
	else
	{
		for (idx = 1; (idx < x) && (bnum_cmp_u(&np, &n1) != 0); idx++)
		{
			bnum_sqr(&np, &np);
			bnum_divmod(&np, num, NULL, &np);

			if (BNUM_ONE(&np))
			{
				res = BNUM_FALSE;
				break ;
			}
		}
		if ((BNUM_TRUE == res) && (bnum_cmp_u(&np, &n1) != 0))
		{
			res = BNUM_FALSE;
		}
	}

	bnum_clear_multi(&np, &n1, &e, NULL);

	return (res);
}
