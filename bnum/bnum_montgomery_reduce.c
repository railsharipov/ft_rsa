#include <bnum.h>

void	bnum_montgomery_reduce(t_num *num, const t_num *mod, uint64_t rho)
{
	int idx, digs;

	digs = (mod->len * 2) + 1;

	if ((digs < BNUM_MAX_WDIG)
		&& (num->len <= BNUM_MAX_WDIG)
		&& (mod->len < BNUM_MAX_DIG_COMBA))
	{
		bnum_montgomery_fast_reduce(num, mod, rho);
		return ;
	}

	bnum_increase_size(num, digs);
	num->len = digs;

	for (idx = 0; idx < mod->len; idx++)
	{
		uint128_t	t, u, mu;
		int			idy;

		mu = (num->val[idx] * rho) & BNUM_MAX_VAL;
		u = 0;

		for (idy = 0; idy < mod->len; idy++)
		{
			t = mu * (uint128_t)mod->val[idy]
				+ u + (uint128_t)num->val[idx + idy];

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

	bnum_skip_zeros(num);
	bnum_rsh_dig_inpl(num, mod->len);

	if (bnum_cmp_u(num, mod) >= 0)
		bnum_sub_u(num, mod, num);
}
