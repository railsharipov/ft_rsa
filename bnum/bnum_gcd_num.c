#include <bnum.h>

void	gcd_num(const t_num *a, const t_num *b, t_num *res)
{
	t_num	x, y;
	int		k, xk, yk;

	if ((a->sign == BNUM_NEG) || (b->sign == BNUM_NEG))
		BNUM_ERROR("negative number is not supported");

	if (BNUM_ZERO(a))
	{
		copy_num(a, res, 0, a->len);
		return ;
	}
	if (BNUM_ZERO(b))
	{
		copy_num(b, res, 0, b->len);
		return ;
	}

	copy_num(a, &x, 0, a->len);
	copy_num(b, &y, 0, b->len);

	xk = rmbit_num(&x) - 1;
	yk = rmbit_num(&y) - 1;

	k = BNUM_MIN(xk, yk);

	rsh_num_b_inpl(&x, xk);
	rsh_num_b_inpl(&y, yk);

	while (!BNUM_ZERO(&y))
	{
		if (compare_num_u(&x, &y) > 0)
			swap_num(&x, &y);

		sub_num_u(&y, &x, &y);

		yk = rmbit_num(&y) - 1;

		if (yk > 0)
			rsh_num_b_inpl(&y, yk);
	}

	lsh_num_b_inpl(&x, k);
	copy_num(&x, res, 0, x.len);
}
