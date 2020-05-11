#include <bnum.h>

int		miller_rabin(const t_num *num, const t_num *rnd)
{
	t_num		np, n1, e;
	int			idx, x;

	sub_num_ud(num, 1, &n1);

	copy_num(&n1, &e, 0, n1.len);
	x = rmbit_num(&e);
	rsh_num_b_inpl(&e, --x);

	m_powmod_num(rnd, &e, num, &np);

	if (BNUM_ONE(&np) || compare_num_u(&np, &n1) == 0)
		return (BNUM_TRUE);

	for (idx = 1; (idx < x) && (compare_num_u(&np, &n1) != 0); idx++)
	{
		sqr_num(&np, &np);
		divmod_num(&np, num, NULL, &np);

		if (BNUM_ONE(&np))
			return (BNUM_FALSE);
	}
	if (compare_num_u(&np, &n1) != 0)
		return (BNUM_FALSE);

	return (BNUM_TRUE);
}
