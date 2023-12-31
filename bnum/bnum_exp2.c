#include <bnum.h>

void	bnum_exp2(t_num *res, int n)
{
	if (n < 0)
		BNUM_ERROR_EXIT("negative exponent is not supported");

	if (n/BNUM_DIGIT_BIT+1 > res->size)
		bnum_increase_size(res, n/BNUM_DIGIT_BIT+1);

	res->val[n/BNUM_DIGIT_BIT] |= ((uint64_t)1 << (n % BNUM_DIGIT_BIT));
	res->len = n/BNUM_DIGIT_BIT+1;
	res->sign = BNUM_POS;
}
