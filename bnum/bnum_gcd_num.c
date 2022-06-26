#include <bnum.h>

void	gcd_num(const t_num *a, const t_num *b, t_num *res)
{
	t_num	x, y;
	int		k, xk, yk;

	if ((a->sign == BNUM_NEG) || (b->sign == BNUM_NEG))
		BNUM_ERROR_EXIT("negative number is not supported");

	if (BNUM_ZERO(a) || BNUM_ZERO(b))
		BNUM_ERROR_EXIT("gcd is undefined for zero input");

	if (BNUM_ONE(a))
	{
		copy_num(a, res);
		return ;
	}
	if (BNUM_ONE(b))
	{
		copy_num(b, res);
		return ;
	}

	init_num(&x);
	init_num(&y);
	copy_num(a, &x);
	copy_num(b, &y);

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
	copy_num(&x, res);
	clear_num(&x);
	clear_num(&y);
}
