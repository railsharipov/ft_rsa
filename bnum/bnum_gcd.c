#include <util/bnum.h>

void	bnum_gcd(const t_num *a, const t_num *b, t_num *res)
{
	t_num	x, y;
	int		k, xk, yk;

	if ((a->sign == BNUM_NEG) || (b->sign == BNUM_NEG))
    {
		BNUM_ERROR("negative number is not supported");
		return ;
    };

	if (BNUM_ZERO(a) || BNUM_ZERO(b))
    {
		BNUM_ERROR("gcd is undefined for zero input");
		return ;
    };

	if (BNUM_ONE(a))
	{
		bnum_copy(a, res);
		return ;
	}
	if (BNUM_ONE(b))
	{
		bnum_copy(b, res);
		return ;
	}

	bnum_init(&x);
	bnum_init(&y);
	bnum_copy(a, &x);
	bnum_copy(b, &y);

	xk = bnum_rmbit(&x) - 1;
	yk = bnum_rmbit(&y) - 1;

	k = BNUM_MIN(xk, yk);

	bnum_rsh_bit_inpl(&x, xk);
	bnum_rsh_bit_inpl(&y, yk);

	while (!BNUM_ZERO(&y))
	{
		if (bnum_cmp_u(&x, &y) > 0)
			bnum_swap(&x, &y);

		bnum_sub_u(&y, &x, &y);

		yk = bnum_rmbit(&y) - 1;

		if (yk > 0)
			bnum_rsh_bit_inpl(&y, yk);
	}

	bnum_lsh_bit_inpl(&x, k);
	bnum_copy(&x, res);
	bnum_clear(&x);
	bnum_clear(&y);
}
