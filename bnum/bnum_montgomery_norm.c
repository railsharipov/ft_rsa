#include <bnum.h>

void	montgomery_norm(const t_num *mod, t_num *res)
{
	int	idx, bits;

	bits = lmbit_num(mod) % BNUM_DIGIT_BIT;

	if (mod->len > 1)
	{
		exp2_num(res, (mod->len-1) * BNUM_DIGIT_BIT + bits-1);
	}
	else
	{
		set_num_ud(res, 1);
		bits = 1;
	}
	for (idx = bits-1; idx < BNUM_DIGIT_BIT; idx++)
	{
		lsh_num_b_inpl(res, 1);

		if (compare_num_u(res, mod) >= 0)
			sub_num_u(res, mod, res);
	}
	skip_zeros(res);
}
