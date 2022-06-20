#include <bnum.h>

void	increm_num_u(t_num *num)
{
	uint64_t	carry, idx;

	carry = 1;
	for (idx = 0; carry && idx < num->len; idx++)
	{
		num->val[idx] += carry;
		carry = num->val[idx] >> BNUM_DIGIT_BIT;
		num->val[idx] &= BNUM_MAX_VAL;
	}
	if (carry)
	{
		increase_num_size(num, num->len + 1);
		num->val[idx] = carry;
		num->len += 1;
	}
}
