#include <bnum.h>

void	mul_num_d(const t_num *a, uint64_t b, t_num *res)
{
	int	i;

	if (a->len+1 > BNUM_MAX_DIG)
		BNUM_ERROR("big number size limit exceeded");
	if (b == 0)
		set_num(res, 0);
	else
	{
		t_uint128	value, carry;

		carry = 0;

		for (i = 0; i < a->len+1; i++)
		{
			value = (t_uint128) a->val[i] * b + carry;
			res->val[i] = value & BNUM_MAX_VAL;
			carry = value >> BNUM_DIGIT_BIT;
		}

		for (; i < BNUM_MAX_DIG; i++)
			res->val[i] = 0;

		res->len = a->len+1;
		res->sign = BNUM_SIGN(a);
		skip_zeros(res);
	}
}
