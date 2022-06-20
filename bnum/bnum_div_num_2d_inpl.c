#include <bnum.h>

void	div_num_2d_inpl(t_num *num)
{
	uint64_t	carry, val;

	if (BNUM_ZERO(num))
		return ;

	carry = 0;
	for (int i = num->len-1; i >= 0; i--)
	{
		val = (num->val[i] >> 1) | carry;
		carry = (num->val[i] << (BNUM_DIGIT_BIT-1)) & BNUM_MAX_VAL;
		num->val[i] = val & BNUM_MAX_VAL;
	}

	skip_zeros(num);
}
