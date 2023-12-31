#include <bnum.h>

void	montgomery_reduce(t_num *num, const t_num *mod, uint64_t rho)
{
	int idx, digs;

	digs = (mod->len * 2) + 1;

	if ((digs < BNUM_MAX_WDIG)
		&& (num->len <= BNUM_MAX_WDIG)
		&& (mod->len < BNUM_MAX_DIG_COMBA))
	{
		montgomery_fast_reduce(num, mod, rho);
		return ;
	}

	increase_num_size(num, digs);
	num->len = digs;

	for (idx = 0; idx < mod->len; idx++)
	{
		t_uint128	t, u, mu;
		int			idy;

		mu = (num->val[idx] * rho) & BNUM_MAX_VAL;
		u = 0;

		for (idy = 0; idy < mod->len; idy++)
		{
			t = mu * (t_uint128)mod->val[idy]
				+ u + (t_uint128)num->val[idx + idy];

			u = (t >> BNUM_DIGIT_BIT) & BNUM_MAX_VAL;
			num->val[idx + idy] = (uint64_t)(t & BNUM_MAX_VAL);
		}

		while (u != 0)
		{
			num->val[idx + idy] += (uint64_t)u;
			u = (num->val[idx + idy] >> BNUM_DIGIT_BIT) & BNUM_MAX_VAL;
			idy++;
		}
	}

	skip_zeros(num);
	rsh_dig_inpl(num, mod->len);

	if (compare_num_u(num, mod) >= 0)
		sub_num_u(num, mod, num);
}
