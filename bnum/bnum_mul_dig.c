#include <util/bnum.h>

void	bnum_mul_dig(const t_num *a, uint64_t b, t_num *res)
{
	int	i;

	if (a->len+1 > res->size)
		bnum_increase_size(res, a->len+1);
	if (b == 0)
		bnum_set_dig_u(res, 0);
	else
	{
		uint128_t	val, carry;

		carry = 0;

		for (i = 0; i < a->len+1; i++)
		{
			val = (uint128_t) a->val[i] * b + carry;
			res->val[i] = val & BNUM_MAX_VAL;
			carry = val >> BNUM_DIGIT_BIT;
		}

		for (; i < res->size; i++)
			res->val[i] = 0;

		res->len = a->len+1;
		res->sign = a->sign;
		bnum_skip_zeros(res);
	}
}
