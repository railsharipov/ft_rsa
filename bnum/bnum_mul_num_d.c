#include <bnum.h>

void	mul_num_d(const t_num *a, uint64_t b, t_num *res)
{
	int	i;

	if (a->len+1 > res->size)
		increase_num_size(res, a->len+1);
	if (b == 0)
		set_num_ud(res, 0);
	else
	{
		t_uint128	val, carry;

		carry = 0;

		for (i = 0; i < a->len+1; i++)
		{
			val = (t_uint128) a->val[i] * b + carry;
			res->val[i] = val & BNUM_MAX_VAL;
			carry = val >> BNUM_DIGIT_BIT;
		}

		for (; i < res->size; i++)
			res->val[i] = 0;

		res->len = a->len+1;
		res->sign = a->sign;
		skip_zeros(res);
	}
}
