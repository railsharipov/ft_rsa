#include <bnum.h>

void	bnum_montgomery_norm(const t_num *mod, t_num *res)
{
	int	idx, bits;

	bits = bnum_lmbit(mod) % BNUM_DIGIT_BIT;

	if (mod->len > 1)
	{
		bnum_exp2(res, (mod->len-1) * BNUM_DIGIT_BIT + bits-1);
	}
	else
	{
		bnum_set_dig_u(res, 1);
		bits = 1;
	}
	for (idx = bits-1; idx < BNUM_DIGIT_BIT; idx++)
	{
		bnum_lsh_bit_inpl(res, 1);

		if (bnum_cmp_u(res, mod) >= 0)
			bnum_sub_u(res, mod, res);
	}
	bnum_skip_zeros(res);
}
