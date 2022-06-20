#include <bnum.h>

void	exp_num(const t_num *base, uint64_t expo, t_num *res)
{
	if (expo == 0)
		set_num_d(res, 1);
	else if (expo == 1)
		copy_num(base, res, 0, base->len);
	else
	{
		int	sign;

		sign = (expo & 1u) ? (base->sign) : (BNUM_POS);
		set_num_d(res, 1);
		for (int i = 0; i < BNUM_INT_BIT; i++)
		{
			sqr_num_comba(res, res);
			if (expo & BNUM_MSB_64)
				mul_num(res, base, res);
			expo <<= 1;
		}
		res->sign = sign;
	}
}
