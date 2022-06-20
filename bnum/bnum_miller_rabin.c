#include <bnum.h>

int		miller_rabin(const t_num *num, const t_num *rnd)
{
	t_num		np, n1, e;
	int			idx, x;
	int			res;

	init_num_multi(&np, &n1, &e, NULL);

	sub_num_ud(num, 1, &n1);
	copy_num(&n1, &e, 0, n1.len);
	x = rmbit_num(&e);
	rsh_num_b_inpl(&e, --x);
	m_powmod_num(rnd, &e, num, &np);

	res = BNUM_TRUE;

	if (BNUM_ONE(&np) || compare_num_u(&np, &n1) == 0)
	{
		res = BNUM_TRUE;
	}
	else
	{
		for (idx = 1; (idx < x) && (compare_num_u(&np, &n1) != 0); idx++)
		{
			sqr_num(&np, &np);
			divmod_num(&np, num, NULL, &np);

			if (BNUM_ONE(&np))
			{
				res = BNUM_FALSE;
				break ;
			}
		}
		if ((BNUM_TRUE == res) && (compare_num_u(&np, &n1) != 0))
		{
			res = BNUM_FALSE;
		}
	}

	clear_num_multi(&np, &n1, &e, NULL);

	return (res);
}
