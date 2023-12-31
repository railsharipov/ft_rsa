#include <bnum.h>

void	bnum_montgomery_fast_reduce(t_num *num, const t_num *mod, uint64_t rho)
{
	t_uint128	tw[BNUM_MAX_DIG];
	int			idx;
	int			slen;

	if (num->len > BNUM_MAX_DIG)
		BNUM_ERROR_EXIT("invalid number size");

	if (num->size < mod->len + 1)
		bnum_increase_size(num, mod->len + 1);

	for (idx = 0; idx < num->len; idx++)
		tw[idx] = num->val[idx];

	if (idx < 2 * mod->len + 1)
		ft_bzero(tw + num->len, sizeof(t_uint128) * (2 * mod->len + 1 - idx));

	for (idx = 0; idx < mod->len; idx++)
	{
		uint64_t	mu;

		mu = ((tw[idx] & BNUM_MAX_VAL) * rho) & BNUM_MAX_VAL;

		{
			const uint64_t	*tm;
			t_uint128		*_tw;
			int				idy;

			tm = mod->val;
			_tw = tw + idx;

			for (idy = 0; idy < mod->len; idy++)
				*_tw++ += (t_uint128) mu * *tm++;
		}

		tw[idx+1] += tw[idx] >> BNUM_DIGIT_BIT;
	}

	{
		t_uint128	*_tw, *_tw1;
		uint64_t	*tn;

		_tw = tw + idx;
		_tw1 = tw + ++idx;

		for (; idx <= (2 * mod->len)+1; idx++)
			*_tw1++ += *_tw++ >> BNUM_DIGIT_BIT;

		tn = num->val;
		_tw = tw + mod->len;

		for (idx = 0; idx < mod->len+1; idx++)
			*tn++ = *_tw++ & BNUM_MAX_VAL;

		for (; idx < num->len; idx++)
			*tn++ = 0;
	}

	num->len = mod->len+1;
	bnum_skip_zeros(num);

	if (bnum_cmp_u(num, mod) >= 0)
		bnum_sub_u(num, mod, num);
}
