#include <bnum.h>

void	increm_num_u(t_num *num)
{
	uint64_t	carry;

	carry = 1;
	for (int i = 0; carry && i < BNUM_MAX_DIG; i++)
	{
		num->val[i] += carry;
		carry = num->val[i] >> BNUM_DIGIT_BIT;
		num->val[i] &= BNUM_MAX_VAL;
	}
	if (carry)
  {
		BNUM_ERROR("big number size limit exceeded");
  }
}
