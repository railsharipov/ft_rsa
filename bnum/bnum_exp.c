#include <bnum.h>

void	bnum_exp(const t_num *base, uint64_t expo, t_num *res)
{
	if (expo == 0)
		bnum_set_dig_u(res, 1);
	else if (expo == 1)
		bnum_copy(base, res);
	else
	{
		int	sign;

		sign = (expo & 1u) ? (base->sign) : (BNUM_POS);
		bnum_set_dig_u(res, 1);
		for (int i = 0; i < BNUM_INT_BIT; i++)
		{
			bnum_sqr_comba(res, res);
			if (expo & BNUM_MSB_64)
				bnum_mul(res, base, res);
			expo <<= 1;
		}
		res->sign = sign;
	}
}
